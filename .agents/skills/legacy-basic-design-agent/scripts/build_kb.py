# .agents/skills/legacy-basic-design-agent/scripts/build_kb.py
# Builds a local SQLite knowledge base from legacy source documents.
# This exists so agents can inspect a compact map before reading source files.
# RELEVANT FILES: .agents/skills/legacy-basic-design-agent/scripts/kb_profile.py, .agents/skills/legacy-basic-design-agent/scripts/query_kb.py, .agents/skills/legacy-basic-design-agent/SKILL.md
from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
import sqlite3
import subprocess
import sys
import tempfile
from pathlib import Path
from xml.etree import ElementTree as ET
from zipfile import ZipFile

from kb_profile import (
    classify_document,
    extract_entities,
    infer_profile,
    is_heading_candidate,
    load_profile,
    profile_snapshot,
    write_profile,
)

W_NS = {"w": "http://schemas.openxmlformats.org/wordprocessingml/2006/main"}


def decode_bytes(data: bytes) -> str:
    for encoding in ("utf-8", "cp932", "shift_jis"):
        try:
            return data.decode(encoding)
        except UnicodeDecodeError:
            continue
    return data.decode("utf-8", errors="replace")


def run_command(args: list[str], timeout: int = 240) -> tuple[int, str, str]:
    result = subprocess.run(args, capture_output=True, timeout=timeout, check=False)
    return result.returncode, decode_bytes(result.stdout), decode_bytes(result.stderr)


def file_hash(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def read_docx_paragraphs(path: Path) -> list[dict]:
    with ZipFile(path) as archive:
        xml_bytes = archive.read("word/document.xml")
    root = ET.fromstring(xml_bytes)
    paragraphs: list[dict] = []
    for para in root.findall(".//w:p", W_NS):
        parts = [node.text or "" for node in para.findall(".//w:t", W_NS)]
        text = "".join(parts).strip()
        if not text:
            continue
        style = ""
        style_node = para.find("./w:pPr/w:pStyle", W_NS)
        if style_node is not None:
            style = style_node.attrib.get(f"{{{W_NS['w']}}}val", "")
        paragraphs.append({"text": text, "style": style})
    return paragraphs


def sectionize_paragraphs(paragraphs: list[dict], title: str, profile: dict) -> list[dict]:
    sections: list[dict] = []
    current_heading = title
    current: list[dict] = []
    order = 1
    for index, paragraph in enumerate(paragraphs):
        text = paragraph["text"]
        if is_heading_candidate(text, paragraph.get("style", ""), index, profile) and current:
            sections.append(make_section(title, current_heading, current, order))
            order += 1
            current_heading = text
            current = []
        elif is_heading_candidate(text, paragraph.get("style", ""), index, profile):
            current_heading = text
        else:
            current.append(paragraph)
    if current or not sections:
        sections.append(make_section(title, current_heading, current or paragraphs[:1], order))
    return sections


def make_section(title: str, heading: str, paragraphs: list[dict], order: int) -> dict:
    text = "\n".join(paragraph["text"] for paragraph in paragraphs).strip()
    return {
        "heading_path": [title, heading] if heading != title else [title],
        "page_or_order": str(order),
        "summary": compact(text, 260),
        "text": text,
    }


def compact(text: str, limit: int) -> str:
    return re.sub(r"\s+", " ", text).strip()[:limit]


def text_quality(text: str) -> float:
    visible = [ch for ch in text if not ch.isspace()]
    if len(visible) < 100:
        return 0.0
    controls = sum(1 for ch in visible if ord(ch) < 32 or ord(ch) == 0xFFFD)
    useful = sum(1 for ch in visible if ch.isalnum() or "\u3040" <= ch <= "\u30ff" or "\u4e00" <= ch <= "\u9fff")
    return max(0.0, min(1.0, (useful / len(visible)) - (controls / len(visible) * 2)))


def pdf_info(path: Path) -> dict:
    if not shutil.which("pdfinfo"):
        return {"title": path.stem, "pages": 0, "error": "pdfinfo not found"}
    code, out, err = run_command(["pdfinfo", str(path)], timeout=60)
    info = {"title": path.stem, "pages": 0, "error": err if code else ""}
    for line in out.splitlines():
        if ":" not in line:
            continue
        key, value = [part.strip() for part in line.split(":", 1)]
        if key == "Title" and value:
            info["title"] = value
        if key == "Pages" and value.isdigit():
            info["pages"] = int(value)
    return info


def extract_pdf_text(path: Path) -> tuple[str, str]:
    if not shutil.which("pdftotext"):
        return "", "pdftotext not found"
    with tempfile.TemporaryDirectory() as tmp:
        out_path = Path(tmp) / "out.txt"
        code, _, err = run_command(["pdftotext", "-layout", str(path), str(out_path)], timeout=300)
        if code != 0:
            return "", err or "pdftotext failed"
        return out_path.read_text(encoding="utf-8", errors="replace"), ""


def ocr_pdf(path: Path, pages: int, lang: str, max_pages: int) -> tuple[str, str]:
    if not shutil.which("tesseract"):
        return "", "tesseract not found; install tesseract and requested language data"
    if not shutil.which("pdftoppm"):
        return "", "pdftoppm not found"
    last_page = pages if max_pages <= 0 else min(pages or max_pages, max_pages)
    with tempfile.TemporaryDirectory() as tmp:
        prefix = str(Path(tmp) / "page")
        code, _, err = run_command(["pdftoppm", "-r", "200", "-png", "-f", "1", "-l", str(last_page), str(path), prefix], timeout=600)
        if code != 0:
            return "", err or "pdftoppm failed"
        texts = []
        for image in sorted(Path(tmp).glob("page-*.png")):
            code, out, err = run_command(["tesseract", str(image), "stdout", "-l", lang], timeout=180)
            texts.append(out if code == 0 else f"[OCR failed for {image.name}: {err}]")
        return "\f".join(texts), ""


def create_schema(conn: sqlite3.Connection) -> None:
    conn.executescript(
        """
        DROP TABLE IF EXISTS documents;
        DROP TABLE IF EXISTS sections;
        DROP TABLE IF EXISTS chunks;
        DROP TABLE IF EXISTS relations;
        DROP TABLE IF EXISTS runs;
        DROP TABLE IF EXISTS kb_metadata;
        DROP TABLE IF EXISTS chunks_fts;
        CREATE TABLE documents (
          id INTEGER PRIMARY KEY, path TEXT UNIQUE, type TEXT, role TEXT, title TEXT, hash TEXT,
          extraction_status TEXT, approval_status TEXT, authority_score REAL,
          interference_reason TEXT, text_quality REAL, error TEXT, metadata_json TEXT
        );
        CREATE TABLE sections (
          id INTEGER PRIMARY KEY, doc_id INTEGER, heading_path TEXT, page_or_order TEXT, summary TEXT
        );
        CREATE TABLE chunks (
          id INTEGER PRIMARY KEY, section_id INTEGER, doc_id INTEGER, text TEXT, source_ref TEXT
        );
        CREATE VIRTUAL TABLE chunks_fts USING fts5(text, title, path);
        CREATE TABLE relations (
          id INTEGER PRIMARY KEY, source_entity TEXT, target_entity TEXT, relation_type TEXT, source_ref TEXT, confidence REAL
        );
        CREATE TABLE runs (
          id INTEGER PRIMARY KEY, query TEXT, selected_docs TEXT, confidence REAL, open_questions TEXT,
          created_at TEXT DEFAULT CURRENT_TIMESTAMP
        );
        CREATE TABLE kb_metadata (key TEXT PRIMARY KEY, value TEXT);
        """
    )


def insert_document(conn: sqlite3.Connection, rel_path: str, digest: str, status: str, quality: float, error: str, metadata: dict) -> int:
    cur = conn.execute(
        """
        INSERT INTO documents(
          path, type, role, title, hash, extraction_status, approval_status,
          authority_score, interference_reason, text_quality, error, metadata_json
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        """,
        (
            rel_path, metadata["type"], metadata["role"], metadata["title"], digest, status,
            metadata.get("approval_status", "unknown"), metadata["authority_score"],
            metadata["interference_reason"], quality, error, json.dumps(metadata, ensure_ascii=False),
        ),
    )
    return int(cur.lastrowid)


def log_event(log_path: Path | None, event: dict) -> None:
    if log_path:
        with log_path.open("a", encoding="utf-8") as handle:
            handle.write(json.dumps(event, ensure_ascii=False) + "\n")


def chunk_text(text: str, limit: int = 1200) -> list[str]:
    if not text:
        return []
    chunks: list[str] = []
    current = ""
    for paragraph in text.splitlines():
        if len(current) + len(paragraph) + 1 > limit and current:
            chunks.append(current.strip())
            current = paragraph
        else:
            current = f"{current}\n{paragraph}" if current else paragraph
    if current:
        chunks.append(current.strip())
    return chunks


def insert_sections(conn: sqlite3.Connection, doc_id: int, rel_path: str, title: str, sections: list[dict], profile: dict) -> None:
    for section in sections:
        cur = conn.execute(
            "INSERT INTO sections(doc_id, heading_path, page_or_order, summary) VALUES (?, ?, ?, ?)",
            (doc_id, json.dumps(section["heading_path"], ensure_ascii=False), section["page_or_order"], section["summary"]),
        )
        section_id = int(cur.lastrowid)
        seeds = " ".join([title, *section["heading_path"]])
        for index, chunk in enumerate(chunk_text(section["text"]), start=1):
            source_ref = f"{rel_path}#{section['page_or_order']}.{index}"
            ccur = conn.execute(
                "INSERT INTO chunks(section_id, doc_id, text, source_ref) VALUES (?, ?, ?, ?)",
                (section_id, doc_id, chunk, source_ref),
            )
            conn.execute(
                "INSERT INTO chunks_fts(rowid, text, title, path) VALUES (?, ?, ?, ?)",
                (int(ccur.lastrowid), chunk, title, rel_path),
            )
            insert_relations(conn, extract_entities(chunk, seeds, profile), source_ref)


def insert_relations(conn: sqlite3.Connection, entities: list[str], source_ref: str) -> None:
    for index, source in enumerate(entities):
        for target in entities[index + 1:]:
            conn.execute(
                "INSERT INTO relations(source_entity, target_entity, relation_type, source_ref, confidence) VALUES (?, ?, ?, ?, ?)",
                (source, target, "co_occurs", source_ref, 0.72),
            )


def sections_from_pdf_text(text: str, title: str) -> list[dict]:
    sections = []
    for index, page_text in enumerate(text.split("\f"), start=1):
        cleaned = page_text.strip()
        if cleaned:
            sections.append({"heading_path": [title, f"page {index}"], "page_or_order": str(index), "summary": compact(cleaned, 260), "text": cleaned})
    return sections


def process_file(conn: sqlite3.Connection, path: Path, docs_root: Path, profile: dict, args: argparse.Namespace) -> None:
    rel_path = str(path.relative_to(docs_root)).replace("\\", "/")
    digest = file_hash(path)
    doc_meta = classify_document(rel_path, path.suffix, profile)
    error = ""
    if path.suffix.lower() == ".docx":
        paragraphs = read_docx_paragraphs(path)
        title = paragraphs[0]["text"] if paragraphs else path.stem
        doc_meta["title"] = title
        sections = sectionize_paragraphs(paragraphs, title, profile)
        status, quality = "extracted", 1.0 if paragraphs else 0.0
    elif path.suffix.lower() == ".pdf":
        pdf_meta = pdf_info(path)
        doc_meta.update({"title": pdf_meta.get("title") or path.stem, "pdf": pdf_meta})
        text, error = extract_pdf_text(path)
        quality = text_quality(text)
        status = "extracted"
        if quality < args.pdf_quality_threshold:
            ocr_text, ocr_error = ocr_pdf(path, int(pdf_meta.get("pages") or 0), args.ocr_langs, args.ocr_max_pages)
            if ocr_text:
                text, error, status, quality = ocr_text, "", "ocr_extracted", text_quality(ocr_text)
            else:
                text, status, error = "", "ocr_required_missing_tesseract" if "tesseract" in ocr_error else "ocr_required_failed", ocr_error
        sections = sections_from_pdf_text(text, doc_meta["title"])
    else:
        return
    doc_id = insert_document(conn, rel_path, digest, status, quality, error, doc_meta)
    if error or status != "extracted":
        log_event(args.log_file, {"path": rel_path, "status": status, "quality": quality, "error": error})
    insert_sections(conn, doc_id, rel_path, doc_meta["title"], sections, profile)


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    parser = argparse.ArgumentParser(description="Build a local SQLite KB from legacy docs.")
    parser.add_argument("--docs-root", required=True)
    parser.add_argument("--out-db", required=True)
    parser.add_argument("--profile", default="")
    parser.add_argument("--write-profile", default="")
    parser.add_argument("--ocr-langs", default="jpn+eng")
    parser.add_argument("--ocr-max-pages", type=int, default=0)
    parser.add_argument("--pdf-quality-threshold", type=float, default=0.85)
    parser.add_argument("--log-file", default="")
    args = parser.parse_args()
    docs_root = Path(args.docs_root).resolve()
    profile = infer_profile(docs_root, load_profile(args.profile))
    if args.write_profile:
        write_profile(Path(args.write_profile), profile)
    out_db = Path(args.out_db).resolve()
    out_db.parent.mkdir(parents=True, exist_ok=True)
    args.log_file = Path(args.log_file).resolve() if args.log_file else out_db.with_name(f"{out_db.stem}.log.jsonl")
    if args.log_file.exists():
        args.log_file.unlink()
    conn = sqlite3.connect(out_db)
    create_schema(conn)
    conn.execute("INSERT INTO kb_metadata(key, value) VALUES (?, ?)", ("profile_snapshot", profile_snapshot(profile)))
    for path in sorted(docs_root.rglob("*")):
        if path.suffix.lower() in {".docx", ".pdf"}:
            process_file(conn, path, docs_root, profile, args)
    conn.commit()
    counts = {name: conn.execute(f"SELECT COUNT(*) FROM {name}").fetchone()[0] for name in ["documents", "sections", "chunks", "relations"]}
    log_event(args.log_file, {"status": "complete", "out_db": str(out_db), "counts": counts})
    conn.close()
    print(json.dumps({"out_db": str(out_db), "log_file": str(args.log_file), "counts": counts}, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
