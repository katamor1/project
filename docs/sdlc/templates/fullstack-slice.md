<!-- docs/sdlc/templates/fullstack-slice.md -->
<!-- Provides the full-stack slice template for frontend, backend, DB, and traceability intent. -->
<!-- This exists so one feature slice can be handed to a future app repo with minimal ambiguity. -->
<!-- RELEVANT FILES: .copilot/prompts/P5_fullstack_slice_author.md, .copilot/schemas/fullstack-slice.schema.json, docs/sdlc/templates/repo-bridge.md -->
---
artifact_id: SLICE-REQ-XXXX
artifact_type: fullstack_slice
status: draft
prompt_id: P5_fullstack_slice_author
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - IMPL-REQ-XXXX
review_record_refs: []
language: ja
---

# Fullstack Slice

## Frontend Files
- Next.js / TypeScript 側の file intent を書く

## Backend Files
- Python API や service 側の file intent を書く

## DB Changes
- SQLite 前提の DB 変更を書く

## Contract Refs
- API contract や DB change の参照を書く

## Traceability
- どの detail design をどう受けたかを書く

## Risks
- 実装前に確認が必要な点を書く
