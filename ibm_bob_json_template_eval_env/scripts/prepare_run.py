#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse, shutil, json
from pathlib import Path

STAGES = [
  '01_basic_design_review',
  '02_detail_design_author',
  '03_detail_design_review',
  '04_function_test_design_author',
  '05_function_test_review'
]
PROMPT_MAP = {
  '01_basic_design_review':'01_basic_design_review.md',
  '02_detail_design_author':'02_detail_design_author.md',
  '03_detail_design_review':'03_detail_design_review.md',
  '04_function_test_design_author':'04_function_test_design_author.md',
  '05_function_test_review':'05_function_test_review.md',
}

def main():
    ap=argparse.ArgumentParser(description='Prepare a JSON-template evaluation run directory.')
    ap.add_argument('--run-id', required=True)
    ap.add_argument('--basic-docx', required=True)
    ap.add_argument('--basic-json')
    ap.add_argument('--force', action='store_true')
    args=ap.parse_args()
    base=Path(__file__).resolve().parents[1]
    run=base/'runs'/args.run_id
    if run.exists() and args.force:
        shutil.rmtree(run)
    if run.exists():
        raise SystemExit(f'Run directory already exists: {run}. Use --force to overwrite.')
    (run/'input').mkdir(parents=True)
    (run/'outputs').mkdir()
    (run/'prompts').mkdir()
    shutil.copy2(args.basic_docx, run/'input'/'basic_design.docx')
    if args.basic_json:
        shutil.copy2(args.basic_json, run/'input'/'basic_design.json')
    for st in STAGES:
        (run/'outputs'/st).mkdir(parents=True)
        prompt=(base/'prompts'/PROMPT_MAP[st]).read_text(encoding='utf-8')
        prompt = prompt.replace('`input/', f'`runs/{args.run_id}/input/')
        prompt = prompt.replace('`outputs/', f'`runs/{args.run_id}/outputs/')
        (run/'prompts'/PROMPT_MAP[st]).write_text(prompt, encoding='utf-8')
    manifest={
      'run_id':args.run_id,
      'flow':'json-template-eval-only',
      'input':{'basic_design_docx':'input/basic_design.docx','basic_design_json':'input/basic_design.json' if args.basic_json else None},
      'stages':STAGES
    }
    (run/'run_manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2), encoding='utf-8')
    print(f'Prepared run: {run}')
    print(f'Next: open prompts in {run/"prompts"}')
if __name__=='__main__': main()
