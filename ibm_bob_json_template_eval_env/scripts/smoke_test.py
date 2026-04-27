#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Lightweight environment validation. This does not run AI generation."""
from pathlib import Path
import json, zipfile, subprocess, sys, shutil, tempfile

base=Path(__file__).resolve().parents[1]
required=[
 'templates/detail_design_template_v2.docx',
 'templates/function_test_design_template_v1.docx',
 'templates/review_result_template_v1.docx',
 'schemas/detail_design.schema.json',
 'schemas/function_test_design.schema.json',
 'schemas/review_result.schema.json',
 'skills/json-docx/scripts/generate_docx_from_json.py',
 'skills/json-docx/scripts/validate_docx_output.py',
 'routing/eval_only_flow.json'
]
for r in required:
    p=base/r
    assert p.exists(), f'missing {r}'
    if p.suffix=='.json': json.load(open(p, encoding='utf-8'))
    if p.suffix=='.docx':
        with zipfile.ZipFile(p) as z: assert 'word/document.xml' in z.namelist()
print('OK: environment smoke test passed')
