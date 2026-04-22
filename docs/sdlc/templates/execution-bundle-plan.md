<!-- docs/sdlc/templates/execution-bundle-plan.md -->
<!-- Provides the execution bundle planning template used before generating near-code packs. -->
<!-- This exists so execution prep can freeze bundle boundaries and target repo shape before pack authoring starts. -->
<!-- RELEVANT FILES: .copilot/prompts/P10_execution_bundle_planner.md, .copilot/schemas/execution-bundle-packet.schema.json, docs/sdlc/templates/target-repo-map.md -->
---
artifact_id: EXEC-BUNDLE-REQ-XXXX
artifact_type: execution_bundle_packet
status: draft
prompt_id: P10_execution_bundle_planner
prompt_version: "1.0"
model_id: claude-code
upstream_refs:
  - BRIDGE-REQ-XXXX
review_record_refs: []
language: ja
---

# Execution Bundle Plan

## Bundle Target
- どの機能 slice を near-code pack にするかを書く

## Repo Shape
- single app layout と canonical root を書く

## Bundle Scope
- 今回含める frontend, backend, db, test の範囲を書く

## Constraints
- path, ownership, non-goals を書く

## Done Criteria
- handoff 完了条件を書く

## Open Questions
- 未解決事項を書く
