# .agents/skills/legacy-basic-design-agent/scripts/query_kb.py
# Queries the local legacy design KB and emits bounded evidence for basic design work.
# This exists so agents can inspect candidates before reading full legacy documents.
# RELEVANT FILES: .agents/skills/legacy-basic-design-agent/scripts/kb_profile.py, .agents/skills/legacy-basic-design-agent/scripts/build_kb.py, .agents/skills/legacy-basic-design-agent/SKILL.md
from __future__ import annotations

import argparse
import json
import re
import sqlite3
import sys
from collections import Counter

from kb_profile import extract_entities, load_profile, profile_from_db

ASCII_WORD_RE = re.compile(r"[A-Za-z][A-Za-z0-9_-]{1,}")


def terms_from_query(query: str, profile: dict) -> list[str]:
    terms = set(extract_entities(query, query, profile))
    terms.update(match.group(0) for match in ASCII_WORD_RE.finditer(query))
    if not terms:
        terms.update(part for part in re.split(r"\s+", query) if part)
    return sorted(terms)


def quote_fts(term: str) -> str:
    return '"' + term.replace('"', '""') + '"'


def fetch_rows(conn: sqlite3.Connection, terms: list[str], top_k: int) -> list[sqlite3.Row]:
    rows: list[sqlite3.Row] = []
    ascii_terms = [term for term in terms if re.search(r"[A-Za-z0-9]", term)]
    if ascii_terms:
        fts_query = " OR ".join(quote_fts(term) for term in ascii_terms)
        try:
            rows.extend(conn.execute(
                """
                SELECT c.id AS chunk_id, d.id AS doc_id, d.path, d.type, d.role, d.title,
                       d.extraction_status, d.authority_score, d.interference_reason,
                       c.text, c.source_ref, s.heading_path, bm25(chunks_fts) AS rank
                FROM chunks_fts
                JOIN chunks c ON c.id = chunks_fts.rowid
                JOIN documents d ON d.id = c.doc_id
                JOIN sections s ON s.id = c.section_id
                WHERE chunks_fts MATCH ?
                ORDER BY rank
                LIMIT ?
                """,
                (fts_query, top_k * 4),
            ).fetchall())
        except sqlite3.Error:
            pass
    for term in terms:
        pattern = f"%{term}%"
        rows.extend(conn.execute(
            """
            SELECT c.id AS chunk_id, d.id AS doc_id, d.path, d.type, d.role, d.title,
                   d.extraction_status, d.authority_score, d.interference_reason,
                   c.text, c.source_ref, s.heading_path, 0.0 AS rank
            FROM chunks c
            JOIN documents d ON d.id = c.doc_id
            JOIN sections s ON s.id = c.section_id
            WHERE c.text LIKE ? OR d.title LIKE ? OR d.path LIKE ?
            LIMIT ?
            """,
            (pattern, pattern, pattern, top_k * 3),
        ).fetchall())
    return rows


def expand_terms(conn: sqlite3.Connection, terms: list[str]) -> list[str]:
    expanded = set(terms)
    for term in terms:
        for row in conn.execute(
            "SELECT source_entity, target_entity FROM relations WHERE source_entity = ? OR target_entity = ? LIMIT 40",
            (term, term),
        ):
            expanded.add(row["source_entity"])
            expanded.add(row["target_entity"])
    return sorted(expanded)


def score_row(row: sqlite3.Row, base_terms: list[str], expanded_terms: list[str], profile: dict) -> tuple[float, list[str]]:
    ranking = profile.get("ranking", {})
    role_weights = ranking.get("role_weights", {})
    haystack = f"{row['title']}\n{row['path']}\n{row['text']}"
    matched_base = [term for term in base_terms if term and term in haystack]
    matched_expanded = [term for term in expanded_terms if term and term in haystack and term not in matched_base]
    matched = matched_base + matched_expanded
    score = float(len(matched_base)) * float(ranking.get("base_term_weight", 3.0))
    score += float(len(matched_expanded)) * float(ranking.get("expanded_term_weight", 0.15))
    score += float(role_weights.get(row["role"], 0.0))
    score += float(row["authority_score"] or 0.0)
    if any(term in row["title"] for term in matched):
        score += float(ranking.get("title_match_bonus", 1.5))
    if row["extraction_status"] not in {"extracted", "ocr_extracted"}:
        score -= float(ranking.get("unusable_text_penalty", 1.0))
    return score, matched


def select_candidates(rows: list[sqlite3.Row], base_terms: list[str], expanded_terms: list[str], profile: dict, top_k: int) -> list[dict]:
    best_by_chunk: dict[int, dict] = {}
    for row in rows:
        score, matched = score_row(row, base_terms, expanded_terms, profile)
        if not matched and score < 2:
            continue
        current = best_by_chunk.get(row["chunk_id"])
        if current and current["score"] >= score:
            continue
        best_by_chunk[row["chunk_id"]] = {
            "score": score,
            "matched_terms": matched,
            "doc_id": row["doc_id"],
            "chunk_id": row["chunk_id"],
            "title": row["title"],
            "type": row["type"],
            "role": row["role"],
            "path": row["path"],
            "extraction_status": row["extraction_status"],
            "authority_score": float(row["authority_score"] or 0.0),
            "interference_reason": row["interference_reason"],
            "source_ref": row["source_ref"],
            "heading_path": json.loads(row["heading_path"]),
            "snippet": snippet(row["text"], base_terms),
        }
    return sorted(best_by_chunk.values(), key=lambda item: item["score"], reverse=True)[:top_k]


def snippet(text: str, terms: list[str], limit: int = 260) -> str:
    clean = re.sub(r"\s+", " ", text).strip()
    positions = [clean.find(term) for term in terms if term in clean]
    start = max(0, min(positions) - 60) if positions else 0
    return clean[start:start + limit]


def confidence_for(candidates: list[dict], profile: dict) -> float:
    if not candidates:
        return 0.0
    ranking = profile.get("ranking", {})
    confidence = 0.45 + min(candidates[0]["score"], 8.0) / 16
    roles = {item["role"] for item in candidates[:8]}
    for bonus in ranking.get("confidence_bonuses", []):
        if set(bonus.get("roles", [])).issubset(roles):
            confidence += float(bonus.get("bonus", 0.0))
    return round(min(confidence, 0.95), 2)


def group_docs(candidates: list[dict]) -> list[str]:
    seen = []
    for item in candidates:
        if item["path"] not in seen:
            seen.append(item["path"])
    return seen


def coverage_for(candidates: list[dict]) -> str:
    if not candidates:
        return "No matching evidence found."
    role_counts = Counter(item["role"] for item in candidates)
    parts = [f"{role}:{count}" for role, count in sorted(role_counts.items())]
    return "Retrieved evidence by role: " + ", ".join(parts)


def build_output(query: str, domain: str, expanded_terms: list[str], candidates: list[dict], profile: dict) -> dict:
    output_cfg = profile.get("output", {})
    confidence = confidence_for(candidates, profile)
    risk_flags = []
    open_questions = []
    if confidence < float(profile.get("ranking", {}).get("minimum_confidence", 0.65)):
        risk_flags.append("low_confidence")
        open_questions.append("Evidence is weak. Ask a human to narrow the feature name or source scope.")
    if any(item.get("extraction_status", "").startswith("ocr_required") for item in candidates):
        risk_flags.append("ocr_gap")
    evidence_items = [
        {
            "doc_id": item["doc_id"],
            "chunk_id": item["chunk_id"],
            "authority_score": item["authority_score"],
            "source_ref": item["source_ref"],
            "title": item["title"],
            "type": item["type"],
            "role": item["role"],
            "extraction_status": item["extraction_status"],
            "matched_terms": item["matched_terms"],
            "snippet": item["snippet"],
        }
        for item in candidates
    ]
    doc_types = sorted({item["type"] for item in candidates})
    return {
        "retrieval_plan": {
            "task_id": output_cfg.get("task_id", "LEGACY-KB-QUERY"),
            "query": query,
            "metadata_filters": {"domain": domain or "unspecified", "doc_types": doc_types},
            "retrieve_top_k": max(len(candidates), 1),
            "rerank_top_k": min(len(candidates), 12),
            "leaf_read_max": min(len(candidates), 8),
            "fallback_strategy": ["relation_expand", "ask_for_narrower_scope"],
            "next_agent": output_cfg.get("context_packer", "artifact-context-packer"),
        },
        "evidence_bundle": {
            "task_id": output_cfg.get("task_id", "LEGACY-KB-QUERY"),
            "query_plan": "map -> candidate sections -> relation expansion -> bounded chunks",
            "retrieved_items": evidence_items,
            "coverage_assessment": coverage_for(candidates),
            "confidence": confidence,
            "risk_flags": risk_flags,
            "next_agent": output_cfg.get("context_packer", "artifact-context-packer"),
        },
        "artifact_context_packet": {
            "artifact_request": {
                "artifact_type": output_cfg.get("artifact_type", "basic_design"),
                "target_agent": output_cfg.get("target_agent", "legacy-basic-design-agent"),
            },
            "evidence_bundle_ref": "inline:evidence_bundle",
            "resolved_entities": expanded_terms,
            "open_questions": open_questions,
            "allowed_assertions": [f"Use {item['title']} as evidence for {', '.join(item['matched_terms'])}" for item in candidates[:8]],
            "evidence_coverage": "Use only retrieved_items unless a human requests another pass.",
            "authority_conflict_note": "not checked beyond local source ranking in v1",
            "next_agent": output_cfg.get("next_agent", "design-author"),
        },
        "impact_candidates": [
            {
                "title": item["title"],
                "type": item["type"],
                "role": item["role"],
                "source_ref": item["source_ref"],
                "matched_terms": item["matched_terms"],
                "interference_reason": item["interference_reason"],
            }
            for item in candidates
        ],
    }


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    parser = argparse.ArgumentParser(description="Query the local legacy basic design KB.")
    parser.add_argument("--db", required=True)
    parser.add_argument("--query", required=True)
    parser.add_argument("--domain", default="")
    parser.add_argument("--top-k", type=int, default=12)
    parser.add_argument("--profile", default="")
    args = parser.parse_args()
    conn = sqlite3.connect(args.db)
    conn.row_factory = sqlite3.Row
    profile = load_profile(args.profile) if args.profile else profile_from_db(conn)
    base_terms = terms_from_query(args.query, profile)
    expanded_terms = expand_terms(conn, base_terms)
    rows = fetch_rows(conn, expanded_terms, args.top_k)
    candidates = select_candidates(rows, base_terms, expanded_terms, profile, args.top_k)
    output = build_output(args.query, args.domain, expanded_terms, candidates, profile)
    conn.execute(
        "INSERT INTO runs(query, selected_docs, confidence, open_questions) VALUES (?, ?, ?, ?)",
        (args.query, json.dumps(group_docs(candidates), ensure_ascii=False), output["evidence_bundle"]["confidence"], json.dumps(output["artifact_context_packet"]["open_questions"], ensure_ascii=False)),
    )
    conn.commit()
    conn.close()
    print(json.dumps(output, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
