<!-- ibm-bob/mode-pack/rules/ibmbob-detail-design/01-core.md -->
<!-- Defines the detail design authoring rules for IBM-Bob runs. -->
<!-- This exists so the detail design packet always covers API, shared memory, state, threading, and error rules. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/schemas/detail-design-packet.schema.json, ibm-bob/mode-pack/rules/ibmbob-review-gate/01-core.md, ibm-bob/mode-pack/rules/ibmbob-context-scout/01-core.md -->
# IBM-Bob Detail Design Rules

## Inputs
- 入力基本設計書
- `project-context-packet`
- 既存 docs と source の根拠

## Required Sections
- `design_summary`
- `api_changes`
- `shared_memory_changes`
- `state_changes`
- `threading_rules`
- `error_rules`
- `traceability`

## Scope
- ここでは詳細設計までに止める。
- 実コードはまだ書かない。
- C サンプル v1 では RT/TS の責務分離を必ず明記する。

## Review Handoff
- 出力先は `ibmbob-review-gate`
- 不明点は traceability か error_rules に隠さず書く。
