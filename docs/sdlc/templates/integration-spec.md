<!-- docs/sdlc/templates/integration-spec.md -->
<!-- Provides the integration test specification template for API, DB, and dependency boundaries. -->
<!-- This exists so integration verification intent is documented before executable tests are created. -->
<!-- RELEVANT FILES: .copilot/prompts/P2_integration_spec_author.md, docs/sdlc/templates/functional-spec.md, docs/sdlc/templates/review-summary.md -->
---
artifact_id: INTSPEC-REQ-XXXX
artifact_type: integration_spec
status: draft
prompt_id: P2_integration_spec_author
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - DETAIL-REQ-XXXX
review_record_refs: []
language: ja
---

# Integration Spec

## 対象境界
- API、DB、外部依存の境界を書く

## 依存関係
- 前提になるサービス、データ、認証を書く

## データ整合
- 更新内容と整合確認を書く

## 異常系と復旧
- 障害、異常入力、復旧観点を書く

## トレーサビリティ
- 詳細設計との対応を書く
