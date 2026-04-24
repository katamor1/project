<!-- docs/sdlc/templates/db-change.md -->
<!-- Provides the DB change template used by implementation slices. -->
<!-- This exists so SQLite change intent can be reviewed before migrations are written. -->
<!-- RELEVANT FILES: .copilot/schemas/db-change.schema.json, docs/sdlc/templates/fullstack-slice.md, docs/sdlc/templates/repo-bridge.md -->
---
artifact_id: DB-CHANGE-REQ-XXXX
artifact_type: db_change
status: draft
prompt_id: ibmbob-sdlc-fullstack-slice-author
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - SLICE-REQ-XXXX
review_record_refs: []
language: ja
---

# DB Change

## Tables
- 対象テーブルを書く

## Fields
- 追加、更新、参照する field を書く

## Migration
- migration 方針を書く

## Backfill
- backfill の有無を書く

## Rollback
- rollback 方針を書く

## Source Refs
- 根拠となる設計参照を書く

