# IBM-Bob エージェントへの依頼文サンプル

以下のJSONとテンプレートからWord設計書を生成してください。
生成AIだけでDOCXを直接作らず、必ず `ibm-bob-json-docx` Skill のスクリプトを使用してください。

## 入力

- JSON: `<path/to/design.json>`
- テンプレートDOCX: `<path/to/template.docx>`
- 出力DOCX: `<path/to/output.docx>`

## 必須手順

```bash
python .ibm-bob/skills/json-docx/scripts/generate_docx_from_json.py --json <JSON> --template <TEMPLATE> --out <OUT> --strict
python .ibm-bob/skills/json-docx/scripts/validate_docx_output.py --docx <OUT> --json <JSON> --fail-on-unresolved
```

## 失敗時

テンプレート差し込みに失敗した場合は、同じJSONからフォールバック構造生成を行い、失敗理由と未解決プレースホルダを報告してください。
