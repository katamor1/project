<!-- docs/sdlc/templates/basic-design.md -->
<!-- Provides the basic design artifact template for upper-level product and system design. -->
<!-- This exists so basic design output stays compact, readable, and reviewable. -->
<!-- RELEVANT FILES: .copilot/prompts/P2_basic_design_author.md, .copilot/schemas/artifact-manifest.schema.json, docs/sdlc/templates/detail-design.md -->
---
artifact_id: BASIC-REQ-XXXX
artifact_type: basic_design
status: draft
prompt_id: P2_basic_design_author
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - SCOPE-REQ-XXXX
review_record_refs: []
language: ja
---

# Basic Design

## 目的
- この基本設計が解決することを書く

## 画面
- 対象画面と責務を書く

## API
- 外から見える API と責務を書く

## 業務ルール
- 条件分岐や制約を書く

## 非対象
- 詳細設計や実装に回す論点を書く

## 受入条件
- 基本設計としてレビュー通過できる条件を書く

## トレーサビリティ
- どの scope 項目をどの section に反映したかを書く
