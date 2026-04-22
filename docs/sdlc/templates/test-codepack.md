<!-- docs/sdlc/templates/test-codepack.md -->
<!-- Provides the test near-code pack template for unit, integration, and regression targets. -->
<!-- This exists so test implementation can be handed off with fixed files and case skeletons. -->
<!-- RELEVANT FILES: .copilot/schemas/test-codepack.schema.json, .copilot/prompts/P13_test_codepack_author.md, docs/sdlc/templates/frontend-codepack.md -->
---
artifact_id: TESTPACK-REQ-XXXX
artifact_type: test_codepack
status: draft
prompt_id: P13_test_codepack_author
prompt_version: "1.0"
model_id: claude-code
upstream_refs:
  - EXEC-BUNDLE-REQ-XXXX
review_record_refs: []
language: ja
---

# Test Codepack

## Target Files
- test file の path と役割を書く

## Unit
- unit test skeleton を書く

## Integration
- integration test skeleton を書く

## Regression
- regression test skeleton を書く

## Fixtures
- fixture と前提データを書く

## Execution Notes
- 実行前に確認することを書く
