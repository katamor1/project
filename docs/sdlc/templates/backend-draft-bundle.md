<!-- docs/sdlc/templates/backend-draft-bundle.md -->
<!-- Provides the template for the backend skeleton code bundle in the integration lane. -->
<!-- This exists so route, service, and model drafts stay aligned before repo implementation. -->
<!-- RELEVANT FILES: .copilot/prompts/P17_backend_draft_author.md, .copilot/schemas/backend-draft-bundle.schema.json, docs/sdlc/templates/sqlite-draft-bundle.md -->
---
artifact_id: BDRAFT-REQ-XXXX
artifact_type: backend_draft_bundle
status: draft
prompt_id: P17_backend_draft_author
prompt_version: "1.0"
model_id: claude-code
upstream_refs:
  - DRAFT-BUNDLE-REQ-XXXX
review_record_refs: []
language: ja
---

# Backend Draft Bundle

## Route Outline
- `backend/api/` の route と責務を書く

## Service Outline
- `backend/services/` と `backend/models/` の責務を書く

## Auth Rules
- 認可、入力制約、権限前提を書く

## Skeleton Code Notes
- imports、signature、TODO body、未解決事項を書く
