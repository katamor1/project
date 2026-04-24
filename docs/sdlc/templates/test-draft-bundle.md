<!-- docs/sdlc/templates/test-draft-bundle.md -->
<!-- Provides the template for test skeleton drafts in the integration lane. -->
<!-- This exists so unit, integration, and regression test intent can be handed off before real test code exists. -->
<!-- RELEVANT FILES: .copilot/prompts/sdlc/ibmbob-sdlc-test-draft-author.md, .copilot/schemas/test-draft-bundle.schema.json, docs/sdlc/templates/frontend-draft-bundle.md -->
---
artifact_id: TDRAFT-REQ-XXXX
artifact_type: test_draft_bundle
status: draft
prompt_id: ibmbob-sdlc-test-draft-author
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - DRAFT-BUNDLE-REQ-XXXX
review_record_refs: []
language: ja
---

# Test Draft Bundle

## Target Files
- `tests/unit/`, `tests/integration/`, `tests/regression/` の対象を書く

## Unit
- unit test の対象と skeleton を書く

## Integration
- integration test の対象と skeleton を書く

## Regression
- regression 観点と skeleton を書く

## Skeleton Notes
- fixture、TODO、未解決事項を書く

