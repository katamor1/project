<!-- docs/sdlc/templates/backend-codepack.md -->
<!-- Provides the backend near-code pack template for canonical API, service, and model roots. -->
<!-- This exists so backend implementation can be handed off with route, service, and DB intent fixed. -->
<!-- RELEVANT FILES: .copilot/schemas/backend-codepack.schema.json, .copilot/prompts/P12_backend_codepack_author.md, docs/sdlc/templates/sqlite-change-pack.md -->
---
artifact_id: BEPACK-REQ-XXXX
artifact_type: backend_codepack
status: draft
prompt_id: P12_backend_codepack_author
prompt_version: "1.0"
model_id: claude-code
upstream_refs:
  - EXEC-BUNDLE-REQ-XXXX
review_record_refs: []
language: ja
---

# Backend Codepack

## Route Outline
- API route の責務を書く

## Service Outline
- service flow を書く

## DB Touchpoints
- 触るテーブルや column を書く

## Auth Rules
- 認可前提を書く

## Error Notes
- 主な失敗ケースを書く
