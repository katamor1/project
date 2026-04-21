<!-- docs/sdlc/templates/detail-design.md -->
<!-- Provides the detail design artifact template used after basic design review. -->
<!-- This exists so the team can turn approved basic design into concrete design decisions without writing code yet. -->
<!-- RELEVANT FILES: .copilot/prompts/P2_detail_design_author.md, .copilot/schemas/artifact-manifest.schema.json, docs/sdlc/templates/basic-design.md -->
---
artifact_id: DETAIL-REQ-XXXX
artifact_type: detail_design
status: draft
prompt_id: P2_detail_design_author
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - BASIC-REQ-XXXX
review_record_refs: []
language: ja
---

# Detail Design

## API 契約
- request, response, validation を書く

## 永続化
- 保存先と主な項目を書く

## 状態
- 状態遷移と更新条件を書く

## 例外
- 異常系と失敗時の扱いを書く

## 権限
- 誰が何をできるかを書く

## 前提
- 既存依存や未変更前提を書く

## トレーサビリティ
- 基本設計のどの項目をどう具体化したかを書く
