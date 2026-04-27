#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Validate DOCX generated from IBM-Bob JSON/template.

Checks:
  - valid zip
  - has word/document.xml
  - document.xml is parseable
  - text body is not empty
  - unresolved {{...}} or [[TABLE:...]] markers are absent when requested
  - optional expected title text exists

This script intentionally performs structural validation only. Visual layout QA should be
performed by opening the DOCX or rendering it with LibreOffice/Word where available.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
import zipfile
from pathlib import Path
import xml.etree.ElementTree as ET

UNRESOLVED_RE = re.compile(r"(\{\{[^{}]+\}\}|\[\[TABLE\s*:[^\]]+\]\])")


def extract_text(docx_path: Path) -> str:
    with zipfile.ZipFile(docx_path) as z:
        xml = z.read("word/document.xml")
    root = ET.fromstring(xml)
    ns = {"w": "http://schemas.openxmlformats.org/wordprocessingml/2006/main"}
    return "".join(node.text or "" for node in root.findall(".//w:t", ns))


def get_path(data, path: str):
    cur = data
    for part in path.split("."):
        if isinstance(cur, dict) and part in cur:
            cur = cur[part]
        else:
            return None
    return cur


def main() -> int:
    ap = argparse.ArgumentParser(description="Validate generated DOCX output.")
    ap.add_argument("--docx", required=True, help="DOCX file path")
    ap.add_argument("--json", help="Optional source JSON. Used to infer expected title.")
    ap.add_argument("--expected-title", help="Text expected to be included in document body")
    ap.add_argument("--fail-on-unresolved", action="store_true", help="Fail if {{...}} or [[TABLE:...]] remains")
    args = ap.parse_args()

    path = Path(args.docx)
    errors = []
    warnings = []

    if not path.exists():
        errors.append(f"File not found: {path}")
    elif path.stat().st_size == 0:
        errors.append("File is empty.")

    text = ""
    if not errors:
        try:
            with zipfile.ZipFile(path) as z:
                names = set(z.namelist())
                for required in ["[Content_Types].xml", "word/document.xml"]:
                    if required not in names:
                        errors.append(f"Missing required DOCX part: {required}")
        except zipfile.BadZipFile:
            errors.append("File is not a valid zip/DOCX.")

    if not errors:
        try:
            text = extract_text(path)
            if len(text.strip()) < 10:
                errors.append("Document body text is too short.")
        except Exception as e:
            errors.append(f"Failed to parse word/document.xml: {e}")

    if text and args.fail_on_unresolved:
        unresolved = sorted(set(UNRESOLVED_RE.findall(text)))
        if unresolved:
            errors.append("Unresolved placeholders remain: " + ", ".join(unresolved[:30]))

    expected = args.expected_title
    if not expected and args.json:
        try:
            with open(args.json, "r", encoding="utf-8") as f:
                data = json.load(f)
            for p in ["document.document_name", "document_control.title", "feature.name", "title"]:
                v = get_path(data, p)
                if isinstance(v, str) and v.strip():
                    expected = v.strip()
                    break
        except Exception as e:
            warnings.append(f"Could not read source JSON for title check: {e}")
    if expected and text and expected not in text:
        warnings.append(f"Expected title text not found exactly: {expected}")

    if errors:
        print("[NG] DOCX validation failed.")
        for e in errors:
            print(f"  - {e}")
        for w in warnings:
            print(f"  - WARN: {w}")
        return 2
    print("[OK] DOCX structure validation passed.")
    print(f"[OK] text_length={len(text.strip())}")
    if warnings:
        for w in warnings:
            print(f"[WARN] {w}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
