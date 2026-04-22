<!-- docs/sdlc/templates/review-summary.md -->
<!-- Provides the short review summary template used by spec, code, execution, and integration review. -->
<!-- This exists so pass, revise, and block decisions are communicated in a consistent way across all lanes. -->
<!-- RELEVANT FILES: .copilot/prompts/P3_spec_reviewer.md, .copilot/prompts/P20_integration_reviewer.md, docs/sdlc/templates/review-record.md -->
---
artifact_id: REVIEW-SUMMARY-REQ-XXXX
artifact_type: review_summary
status: draft
prompt_id: REVIEWER_PROMPT_ID
prompt_version: "1.0"
model_id: claude-code
upstream_refs:
  - ARTIFACT-REQ-XXXX
review_record_refs: []
language: ja
---

# Review Summary

## 判定
- `pass`, `revise`, `block` のどれかを書く

## 根拠
- 判定の理由を短く書く

## 指摘
- 重要な指摘だけを書く

## 差戻し先
- 戻す agent、次工程、または human checkpoint を書く
