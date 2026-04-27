#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse, subprocess, sys
from pathlib import Path

STAGE_SPEC = {
  '01_basic_design_review': ('review_result.json', 'review_result.docx', 'templates/review_result_template_v1.docx'),
  '02_detail_design_author': ('detail_design.json', 'detail_design.docx', 'templates/detail_design_template_v2.docx'),
  '03_detail_design_review': ('review_result.json', 'review_result.docx', 'templates/review_result_template_v1.docx'),
  '04_function_test_design_author': ('function_test_design.json', 'function_test_design.docx', 'templates/function_test_design_template_v1.docx'),
  '05_function_test_review': ('review_result.json', 'review_result.docx', 'templates/review_result_template_v1.docx'),
}

def run(cmd):
    print('+ ' + ' '.join(str(c) for c in cmd))
    subprocess.run(cmd, check=True)

def main():
    ap=argparse.ArgumentParser(description='Render and validate DOCX for one evaluation stage.')
    ap.add_argument('--run', required=True, help='Run directory, e.g. runs/ILK001')
    ap.add_argument('--stage', required=True, choices=STAGE_SPEC.keys())
    args=ap.parse_args()
    base=Path(__file__).resolve().parents[1]
    run_dir=(base/args.run) if not Path(args.run).is_absolute() else Path(args.run)
    json_name, docx_name, template_rel = STAGE_SPEC[args.stage]
    stage_dir=run_dir/'outputs'/args.stage
    json_path=stage_dir/json_name
    out_path=stage_dir/docx_name
    template_path=base/template_rel
    if not json_path.exists():
        raise SystemExit(f'Missing JSON: {json_path}')
    gen=base/'skills/json-docx/scripts/generate_docx_from_json.py'
    val=base/'skills/json-docx/scripts/validate_docx_output.py'
    run([sys.executable, str(gen), '--json', str(json_path), '--template', str(template_path), '--out', str(out_path), '--strict'])
    run([sys.executable, str(val), '--docx', str(out_path), '--json', str(json_path), '--fail-on-unresolved'])
    print(f'Generated: {out_path}')
if __name__=='__main__': main()
