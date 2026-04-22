<!-- docs/sdlc/templates/target-repo-map.md -->
<!-- Provides the target repo map template used to freeze canonical roots, naming rules, and ownership. -->
<!-- This exists so implementers do not need to re-decide the target layout when connecting to a real app repo later. -->
<!-- RELEVANT FILES: .copilot/schemas/target-repo-map.schema.json, docs/sdlc/templates/execution-bundle-plan.md, docs/sdlc/templates/repo-bridge.md -->
---
artifact_id: TARGET-MAP-REQ-XXXX
artifact_type: target_repo_map
status: draft
prompt_id: P10_execution_bundle_planner
prompt_version: "1.0"
model_id: claude-code
upstream_refs:
  - EXEC-BUNDLE-REQ-XXXX
review_record_refs: []
language: ja
---

# Target Repo Map

## Canonical Roots
- `frontend/`, `backend/`, `db/`, `tests/` の root を書く

## Naming Rules
- path と file 名の規約を書く

## File Ownership
- frontend, backend, db, test の責務分担を書く

## Handoff Order
- 実装着手の順番を書く
