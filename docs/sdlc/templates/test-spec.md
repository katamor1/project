<!-- docs/sdlc/templates/test-spec.md -->
<!-- Provides the test spec template used for non-executable implementation test packs. -->
<!-- This exists so acceptance and regression intent can be handed off before test code exists. -->
<!-- RELEVANT FILES: .copilot/prompts/P6_test_author.md, .copilot/schemas/test-pack.schema.json, docs/sdlc/templates/fullstack-slice.md -->
---
artifact_id: TEST-SPEC-REQ-XXXX
artifact_type: test_spec
status: draft
prompt_id: P6_test_author
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - SLICE-REQ-XXXX
review_record_refs: []
language: ja
---

# Test Spec

## Happy Path
- 正常系の確認を書く

## Validation
- 入力検証の確認を書く

## Permission
- 権限制御の確認を書く

## Regression
- 既存挙動の崩れ確認を書く

## Fixture Notes
- fixture や前提データを書く
