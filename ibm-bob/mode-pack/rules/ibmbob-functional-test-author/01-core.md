<!-- ibm-bob/mode-pack/rules/ibmbob-functional-test-author/01-core.md -->
<!-- Defines the functional test authoring rules for IBM-Bob v1. -->
<!-- This exists so the workflow ends with reviewed functional test artifacts even though execution is out of scope. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/schemas/functional-test-pack.schema.json, ibm-bob/mode-pack/rules/ibmbob-run-summary/01-core.md, ibm-bob/mode-pack/rules/ibmbob-review-gate/01-core.md -->
# IBM-Bob Functional Test Author Rules

## Scope
- 機能テストは作成だけに止める。
- 実行はしない。

## Required Output
- `scenarios`
- `preconditions`
- `steps`
- `expected_results`
- `negative_cases`

## Focus
- 基本設計書の業務シナリオを反映する。
- 正常系だけでなく異常系と停止系も含める。
