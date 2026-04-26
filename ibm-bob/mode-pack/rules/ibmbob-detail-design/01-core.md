<!-- ibm-bob/mode-pack/rules/ibmbob-detail-design/01-core.md -->
<!-- Defines the detail design authoring rules for IBM-Bob runs. -->
<!-- This exists so the detail design packet and the human DOCX design stay synchronized through structured JSON. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/schemas/detail-design-packet.schema.json, ibm-bob/mode-pack/structured-templates/detail-design/**, ibm-bob/mode-pack/rules/ibmbob-review-gate/01-core.md -->
# IBM-Bob Detail Design Rules

## Inputs
- 入力基本設計書
- `project-context-packet`
- 既存 docs と source の根拠

## Required Structured Artifacts
- Intermediate JSON: `artifact_kind = detail_design`
- DOCX: `IBM-Bob_詳細設計書テンプレート_v2.docx` から生成する詳細設計書
- Checklist: `ibm_bob_detail_design_pre_review_checklist.json`

Installed references:
- `.bob/ibm-bob/references/sdlc/structured-templates/detail-design/ibm_bob_detail_design_schema.json`
- `.bob/ibm-bob/references/sdlc/structured-templates/detail-design/IBM-Bob_詳細設計書テンプレート_v2.docx`
- `.bob/ibm-bob/references/sdlc/structured-templates/detail-design/ibm_bob_detail_design_pre_review_checklist.json`

## Required Packet Sections
- `design_summary`
- `api_changes`
- `shared_memory_changes`
- `state_changes`
- `threading_rules`
- `error_rules`
- `traceability`

## Required DOCX/JSON Coverage
- source basic design traceability
- source code impact analysis
- module and function specs
- global/shared memory changes
- API changes
- RT/TS/API processing placement
- concurrency and consistency rules
- error/safety behavior
- performance budget
- build/test/static-analysis handoff

## Scope
- ここでは詳細設計までに止める。
- 実コードはまだ書かない。
- C サンプル v1 では RT/TS の責務分離を必ず明記する。
- JSONを先に作成し、DOCXはJSONから生成または転記する。

## Review Handoff
- 出力先は `ibmbob-review-gate`
- 不明点は traceability か error_rules に隠さず書く。
- handoff には `detail_design_json_ref`, `detail_design_docx_ref`, `schema_ref`, `template_ref`, `checklist_ref` を含める。
