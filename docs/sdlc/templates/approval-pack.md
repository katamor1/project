<!-- docs/sdlc/templates/approval-pack.md -->
<!-- Provides the template for the explicit approval pack created after integration review. -->
<!-- This exists so ready-for-repo decisions and residual risks are documented separately from reviewer verdicts. -->
<!-- RELEVANT FILES: .copilot/prompts/P21_approval_pack_author.md, .copilot/schemas/approval-pack.schema.json, docs/sdlc/templates/review-summary.md -->
---
artifact_id: APPROVAL-REQ-XXXX
artifact_type: approval_pack
status: draft
prompt_id: P21_approval_pack_author
prompt_version: "1.0"
model_id: claude-code
upstream_refs:
  - IRF-REQ-XXXX
review_record_refs: []
language: ja
---

# Approval Pack

## Approval Status
- `approved`, `conditional`, `blocked` のどれかを書く

## Ready For Repo
- app repo handoff 可否を書く

## Artifact Refs
- 関連する draft bundle と review artifact を書く

## Residual Risks
- 残っているリスクを書く

## Manual Steps
- 人手で行う手順を書く
