<!-- docs/sdlc/templates/repo-bridge.md -->
<!-- Provides the repo bridge template used to hand approved implementation assets into a future app repo. -->
<!-- This exists so manual implementation handoff can stay structured before repo automation exists. -->
<!-- RELEVANT FILES: .copilot/schemas/repo-bridge.schema.json, docs/sdlc/templates/fullstack-slice.md, artifacts/implementation/README.md -->
---
artifact_id: BRIDGE-REQ-XXXX
artifact_type: repo_bridge
status: draft
prompt_id: MANUAL_BRIDGE_TEMPLATE
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - SLICE-REQ-XXXX
review_record_refs: []
language: ja
---

# Repo Bridge

## Target Repo Paths
- app repo に渡す path と責務を書く

## File Intent
- 各 path に何を置くかを書く

## Execution Order
- 実装着手の順番を書く

## Manual Handoff Notes
- 人手で確認する点を書く
