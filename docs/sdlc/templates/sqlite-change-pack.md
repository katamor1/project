<!-- docs/sdlc/templates/sqlite-change-pack.md -->
<!-- Provides the SQLite change pack template for schema and migration draft intent. -->
<!-- This exists so DB responsibility stays explicit and reviewable before any migration is written. -->
<!-- RELEVANT FILES: .copilot/schemas/sqlite-change-pack.schema.json, .copilot/prompts/P12_backend_codepack_author.md, docs/sdlc/templates/backend-codepack.md -->
---
artifact_id: SQLPACK-REQ-XXXX
artifact_type: sqlite_change_pack
status: draft
prompt_id: P12_backend_codepack_author
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - EXEC-BUNDLE-REQ-XXXX
review_record_refs: []
language: ja
---

# SQLite Change Pack

## Target Files
- schema と migration の path を書く

## Schema Diff
- 追加、更新する column や制約を書く

## Migration Draft
- migration の下書きを書く

## Rollback Notes
- rollback 時の扱いを書く
