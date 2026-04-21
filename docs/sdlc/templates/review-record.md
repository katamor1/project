<!-- docs/sdlc/templates/review-record.md -->
<!-- Provides the review record template for storing design and implementation review history. -->
<!-- This exists so review activity remains traceable without adding a custom logging system. -->
<!-- RELEVANT FILES: .copilot/prompts/P8_review_record.md, .copilot/schemas/review-record.schema.json, reviews/code/README.md -->
---
artifact_id: RR-REQ-XXXX
artifact_type: review_record
status: draft
prompt_id: P8_review_record
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - RF-REQ-XXXX
review_record_refs: []
language: ja
---

# Review Record

## 対象 Artifact
- レビューした設計または実装準備 artifact を書く

## 判定履歴
- 何回目のレビューでどうなったかを書く

## 主要指摘
- 大事な指摘を簡潔に書く

## 未解決リスク
- まだ残っているリスクを書く

## 次工程メモ
- 次工程で見るべき点を書く
