<!-- docs/sdlc/templates/frontend-draft-bundle.md -->
<!-- Provides the template for the frontend skeleton code bundle in the integration lane. -->
<!-- This exists so TSX and TS draft files follow one consistent handoff shape before repo implementation. -->
<!-- RELEVANT FILES: .copilot/prompts/P16_frontend_draft_author.md, .copilot/schemas/frontend-draft-bundle.schema.json, docs/sdlc/templates/integration-bundle-plan.md -->
---
artifact_id: FDRAFT-REQ-XXXX
artifact_type: frontend_draft_bundle
status: draft
prompt_id: P16_frontend_draft_author
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - DRAFT-BUNDLE-REQ-XXXX
review_record_refs: []
language: ja
---

# Frontend Draft Bundle

## Target Files
- `frontend/app/`, `frontend/components/`, `frontend/lib/` 配下の対象を書く

## Component Outline
- page、component、helper の役割を書く

## State Flow
- 入力、画面状態、API 呼び出しの流れを書く

## Skeleton Code Notes
- imports、props、TODO body、未解決事項を書く
