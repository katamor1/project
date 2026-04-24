<!-- docs/sdlc/templates/implementation-plan.md -->
<!-- Provides the implementation planning template used to define one bounded full-stack slice. -->
<!-- This exists so implementation prep can be handed off without writing code yet. -->
<!-- RELEVANT FILES: .copilot/prompts/sdlc/ibmbob-sdlc-implementation-planner.md, .copilot/schemas/implementation-packet.schema.json, docs/sdlc/templates/fullstack-slice.md -->
---
artifact_id: IMPL-REQ-XXXX
artifact_type: implementation_plan
status: draft
prompt_id: ibmbob-sdlc-implementation-planner
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - DETAIL-REQ-XXXX
review_record_refs: []
language: ja
---

# Implementation Plan

## 目的
- この slice で何を実装準備するかを書く

## Slice Boundary
- 1 機能としてどこまで含めるかを書く

## UI Changes
- 画面変更の責務を書く

## API Changes
- API 変更の責務を書く

## DB Changes
- 永続化変更の責務を書く

## Dependencies
- 依存する既存資産を書く

## Done Criteria
- handoff 完了条件を書く

## Open Questions
- 未解決事項を書く

