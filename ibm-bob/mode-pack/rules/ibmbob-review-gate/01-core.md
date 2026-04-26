<!-- ibm-bob/mode-pack/rules/ibmbob-review-gate/01-core.md -->
<!-- Defines the shared review logic for all IBM-Bob packet and structured artifact review stages. -->
<!-- This exists so pass, revise, and block decisions stay consistent and route to one explicit next agent. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/schemas/review-decision-packet.schema.json, ibm-bob/mode-pack/structured-templates/**, ibm-bob/mode-pack/routing/stage-flow.json -->
# IBM-Bob Review Gate Rules

## Decision Values
- `pass`
- `revise`
- `block`

## Structured Artifact Review
When the artifact has JSON/DOCX pair references, review both.

- Validate JSON against the matching schema.
- Confirm DOCX is generated from the matching template.
- Confirm `SECTION_ID` and `JSON Path` coverage.
- Confirm JSON and DOCX are consistent.
- Use the matching checklist:
  - basic design: `ibm_bob_pre_review_checklist.json`
  - detail design: `ibm_bob_detail_design_pre_review_checklist.json`
  - function test design: `ibm_bob_function_test_pre_review_checklist.json`

## Stage-Specific Checks
- `basic_design`: 目的、範囲、要件、RT/TS、共有メモリ、API、状態/シーケンス、安全、影響調査、テスト受け入れ観点
- `detail_design`: 設計整合性、API/共有メモリ/状態/RT-TS 分離、traceability、関数仕様、排他、性能予算、build/test
- `code_change`: touched files、変更理由、レビュー観点、設計との整合
- `unit_test_pack`: coverage、fixtures、期待結果、command source of truth
- `unit_test_run`: 失敗原因の切り分け、reroute_target の妥当性
- `functional_test_pack`: 業務シナリオ、前提条件、手順、期待結果、異常系
- `function_test_design`: traceability、coverage matrix、状態/シーケンス、共有メモリ/API、RT性能、フェイルセーフ、証跡、回帰

## Reroute Rules
- `basic_design` revise は `ibmbob-sdlc-basic-design-author`
- `detail_design` revise は `ibmbob-detail-design` または `ibmbob-sdlc-detail-design-author`
- `code_change` revise は `ibmbob-code-author`
- `unit_test_pack` revise は `ibmbob-unit-test-author`
- `unit_test_run` revise は `ibmbob-code-author`, `ibmbob-unit-test-author`, `ibmbob-intake` のいずれか
- `functional_test_pack` / `function_test_design` revise は `ibmbob-functional-test-author` または `ibmbob-sdlc-functional-spec-author`

## Block Rule
- 重大な前提崩れだけを `block` にする。
- `block` の時だけ `human_checkpoint = true`
- RT禁止処理混入、安全停止仕様欠落、JSON/DOCX重大不一致、schema不適合で後工程に進めない場合は `block` 候補とする。
