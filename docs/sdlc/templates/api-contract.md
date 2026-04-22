<!-- docs/sdlc/templates/api-contract.md -->
<!-- Provides the API contract template used by implementation slices. -->
<!-- This exists so backend intent can be handed off as a small, reviewable contract artifact. -->
<!-- RELEVANT FILES: .copilot/schemas/api-contract.schema.json, docs/sdlc/templates/fullstack-slice.md, docs/sdlc/templates/repo-bridge.md -->
---
artifact_id: API-CONTRACT-REQ-XXXX
artifact_type: api_contract
status: draft
prompt_id: P5_fullstack_slice_author
prompt_version: "1.0"
model_id: claude-code
upstream_refs:
  - SLICE-REQ-XXXX
review_record_refs: []
language: ja
---

# API Contract

## Endpoint
- endpoint を書く

## Auth
- 認可前提を書く

## Request
- request field と validation を書く

## Response
- response field を書く

## Errors
- 主な失敗ケースを書く

## Source Refs
- 根拠となる設計参照を書く
