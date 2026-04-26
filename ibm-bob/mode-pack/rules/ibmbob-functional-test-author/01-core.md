<!-- ibm-bob/mode-pack/rules/ibmbob-functional-test-author/01-core.md -->
<!-- Defines the functional test authoring rules for IBM-Bob v1. -->
<!-- This exists so the workflow ends with reviewed functional test design JSON and DOCX artifacts even though execution is out of scope. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/schemas/functional-test-pack.schema.json, ibm-bob/mode-pack/structured-templates/function-test-design/**, ibm-bob/mode-pack/rules/ibmbob-review-gate/01-core.md -->
# IBM-Bob Functional Test Author Rules

## Scope
- 機能テストは設計書作成だけに止める。
- 実行はしない。
- JSONを先に作成し、DOCXはJSONから生成または転記する。

## Required Structured Artifacts
- Intermediate JSON: `artifact_kind = function_test_design`
- DOCX: `IBM-Bob_機能テスト設計書テンプレート_v1.docx` から生成する機能テスト設計書
- Checklist: `ibm_bob_function_test_pre_review_checklist.json`

Installed references:
- `.bob/ibm-bob/references/sdlc/structured-templates/function-test-design/ibm_bob_function_test_design_schema.json`
- `.bob/ibm-bob/references/sdlc/structured-templates/function-test-design/IBM-Bob_機能テスト設計書テンプレート_v1.docx`
- `.bob/ibm-bob/references/sdlc/structured-templates/function-test-design/ibm_bob_function_test_pre_review_checklist.json`

## Required Packet Output
- `scenarios`
- `preconditions`
- `steps`
- `expected_results`
- `negative_cases`

## Required DOCX/JSON Coverage
- basic/detail design traceability
- functional and non-functional test viewpoints
- coverage matrix
- normal, abnormal, boundary, state transition, sequence, fail-safe, and regression cases
- shared memory/API integration observations
- RT performance observation method
- evidence log/output files
- VC2026/clang build-test references

## Focus
- 基本設計書の業務シナリオを反映する。
- 詳細設計書の関数/API/共有メモリ/状態設計をテスト観点へ展開する。
- 正常系だけでなく異常系と停止系も含める。

## Review Handoff
- 出力先は `ibmbob-review-gate`
- handoff には `function_test_design_json_ref`, `function_test_design_docx_ref`, `schema_ref`, `template_ref`, `checklist_ref` を含める。
