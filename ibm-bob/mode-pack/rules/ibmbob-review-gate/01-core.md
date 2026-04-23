<!-- ibm-bob/mode-pack/rules/ibmbob-review-gate/01-core.md -->
<!-- Defines the shared review logic for all IBM-Bob packet review stages. -->
<!-- This exists so pass, revise, and block decisions stay consistent and route to one explicit next agent. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/schemas/review-decision-packet.schema.json, ibm-bob/mode-pack/routing/stage-flow.json, ibm-bob/mode-pack/rules/orchestrator/01-core.md -->
# IBM-Bob Review Gate Rules

## Decision Values
- `pass`
- `revise`
- `block`

## Stage-Specific Checks
- `detail_design`: 設計整合性、API/共有メモリ/状態/RT-TS 分離、traceability
- `code_change`: touched files、変更理由、レビュー観点、設計との整合
- `unit_test_pack`: coverage、fixtures、期待結果、command source of truth
- `unit_test_run`: 失敗原因の切り分け、reroute_target の妥当性
- `functional_test_pack`: 業務シナリオ、前提条件、手順、期待結果、異常系

## Reroute Rules
- `detail_design` revise は `ibmbob-detail-design`
- `code_change` revise は `ibmbob-code-author`
- `unit_test_pack` revise は `ibmbob-unit-test-author`
- `unit_test_run` revise は `ibmbob-code-author`, `ibmbob-unit-test-author`, `ibmbob-intake` のいずれか
- `functional_test_pack` revise は `ibmbob-functional-test-author`

## Block Rule
- 重大な前提崩れだけを `block` にする。
- `block` の時だけ `human_checkpoint = true`
