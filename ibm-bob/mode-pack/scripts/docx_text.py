# ibm-bob/mode-pack/scripts/docx_text.py
# Extracts plain paragraph text from `.docx` files without external dependencies.
# This exists so the IBM-Bob evaluation scripts can read design documents on a clean Python install.
# RELEVANT FILES: ibm-bob/mode-pack/scripts/context_builder.py, ibm-bob/mode-pack/scripts/stage_packets.py, ibm-bob/mode-pack/scripts/evaluate_mode_pack.py
from __future__ import annotations

from pathlib import Path
from xml.etree import ElementTree as ET
from zipfile import ZipFile

W_NS = {"w": "http://schemas.openxmlformats.org/wordprocessingml/2006/main"}


def read_docx_paragraphs(path: Path) -> list[str]:
    with ZipFile(path) as archive:
        xml_bytes = archive.read("word/document.xml")
    root = ET.fromstring(xml_bytes)
    paragraphs: list[str] = []
    for para in root.findall(".//w:p", W_NS):
        parts = [node.text or "" for node in para.findall(".//w:t", W_NS)]
        text = "".join(parts).strip()
        if text:
            paragraphs.append(text)
    return paragraphs


def summarize_docx(path: Path, line_limit: int = 8) -> str:
    paragraphs = read_docx_paragraphs(path)
    return " / ".join(paragraphs[:line_limit])
