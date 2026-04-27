#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse, json, zipfile, re
from pathlib import Path

REQUIRED = {
  '01_basic_design_review':['review_result.json','review_result.docx'],
  '02_detail_design_author':['detail_design.json','detail_design.docx'],
  '03_detail_design_review':['review_result.json','review_result.docx'],
  '04_function_test_design_author':['function_test_design.json','function_test_design.docx'],
  '05_function_test_review':['review_result.json','review_result.docx'],
}
FORBIDDEN_SUFFIXES = {'.c','.h','.cpp','.hpp','.obj','.exe','.dll'}
UNRESOLVED_RE = re.compile(rb'(\{\{[^{}]+\}\}|\[\[TABLE\s*:[^\]]+\]\])')

def check_docx(path: Path):
    with zipfile.ZipFile(path) as z:
        names=set(z.namelist())
        if 'word/document.xml' not in names:
            raise AssertionError(f'{path}: missing word/document.xml')
        xml=z.read('word/document.xml')
        if UNRESOLVED_RE.search(xml):
            raise AssertionError(f'{path}: unresolved template token remains')

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--run', required=True)
    args=ap.parse_args()
    run=Path(args.run)
    if not run.is_absolute():
        run=Path(__file__).resolve().parents[1]/run
    errors=[]
    for stage, files in REQUIRED.items():
        for f in files:
            p=run/'outputs'/stage/f
            if not p.exists():
                errors.append(f'Missing: {p}')
            elif p.suffix == '.json':
                try:
                    json.load(open(p, encoding='utf-8'))
                except Exception as e:
                    errors.append(f'Invalid JSON: {p}: {e}')
            elif p.suffix == '.docx':
                try:
                    check_docx(p)
                except Exception as e:
                    errors.append(str(e))
    for p in (run/'outputs').rglob('*'):
        if p.suffix.lower() in FORBIDDEN_SUFFIXES:
            errors.append(f'Forbidden implementation artifact found: {p}')
    if errors:
        print('\n'.join(errors))
        raise SystemExit(1)
    print(f'OK: required outputs exist and DOCX files have no unresolved placeholders: {run}')
if __name__=='__main__': main()
