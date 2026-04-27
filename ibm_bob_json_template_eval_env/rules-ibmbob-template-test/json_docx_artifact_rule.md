# json_docx_artifact_rule

- DOCX成果物は必ず構造化JSONから生成する。
- テンプレートに直接文章を書き込む作業をAIだけで完結しない。
- テンプレート差し込みは `skills/json-docx/scripts/generate_docx_from_json.py` を使用する。
- 検証は `skills/json-docx/scripts/validate_docx_output.py --fail-on-unresolved` を使用する。
- 生成AIは、JSONの内容品質、レビュー判断、未確定事項抽出に集中する。
