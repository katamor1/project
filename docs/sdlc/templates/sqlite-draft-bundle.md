<!-- docs/sdlc/templates/sqlite-draft-bundle.md -->
<!-- Provides the template for SQLite schema and migration draft artifacts in the integration lane. -->
<!-- This exists so database change responsibility stays explicit and separate from backend code drafts. -->
<!-- RELEVANT FILES: .copilot/prompts/sdlc/ibmbob-sdlc-sqlite-draft-author.md, .copilot/schemas/sqlite-draft-bundle.schema.json, docs/sdlc/templates/backend-draft-bundle.md -->
---
artifact_id: SQLDRAFT-REQ-XXXX
artifact_type: sqlite_draft_bundle
status: draft
prompt_id: ibmbob-sdlc-sqlite-draft-author
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - DRAFT-BUNDLE-REQ-XXXX
review_record_refs: []
language: ja
---

# SQLite Draft Bundle

## Schema File
- `db/schema/` の対象ファイルを書く

## Migration File
- `db/migrations/` の対象ファイルを書く

## SQL Skeleton
- schema diff、migration draft、前提を書く

## Rollback Notes
- rollback 方針と注意点を書く

