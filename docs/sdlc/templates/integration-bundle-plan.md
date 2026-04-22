<!-- docs/sdlc/templates/integration-bundle-plan.md -->
<!-- Provides the template for planning a code draft bundle before draft authors create skeleton files. -->
<!-- This exists so repo refs, draft targets, and done criteria stay explicit in the integration lane. -->
<!-- RELEVANT FILES: .copilot/prompts/P15_integration_bundle_planner.md, .copilot/schemas/code-draft-packet.schema.json, docs/sdlc/templates/approval-pack.md -->
---
artifact_id: DRAFT-BUNDLE-REQ-XXXX
artifact_type: code_draft_packet
status: draft
prompt_id: P15_integration_bundle_planner
prompt_version: "1.0"
model_id: claude-code
upstream_refs:
  - EXEC-BUNDLE-REQ-XXXX
review_record_refs: []
language: ja
---

# Integration Bundle Plan

## Draft Target
- 対象 slice と draft bundle の範囲を書く

## Repo Refs
- `execution_bundle_ref` と `target_repo_map_ref` を書く

## Constraints
- path 制約、技術制約、approval 条件を書く

## Done Criteria
- skeleton code と approval pack の完了条件を書く

## Open Questions
- まだ決まっていない点を書く
