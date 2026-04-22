<!-- docs/sdlc/templates/scope.md -->
<!-- Provides the scope artifact template used after request normalization. -->
<!-- This exists so scope design always captures the same minimum sections before basic design starts. -->
<!-- RELEVANT FILES: .copilot/prompts/P1_scope.md, .copilot/schemas/scope-packet.schema.json, docs/sdlc/templates/basic-design.md -->
---
artifact_id: SCOPE-REQ-XXXX
artifact_type: scope
status: draft
prompt_id: P1_scope
prompt_version: "1.0"
model_id: claude-code
upstream_refs:
  - REQ-XXXX
review_record_refs: []
language: ja
---

# Scope

## 目的
- この要求で何を決めるかを書く

## 対象
- 今回含める画面、業務、ルールを書く

## 非対象
- 今回やらないものを書く

## 受入条件
- この scope が次工程へ渡せる条件を書く

## 前提
- 既存資産や運用前提を書く

## 用語
- 用語の定義を書く

## 未解決
- 推測せずに残す論点を書く
