<!-- docs/sdlc/templates/eval-report.md -->
<!-- Provides the manual evaluation report template used after review record creation. -->
<!-- This exists so the team can capture coverage gaps before adding automation or test traces. -->
<!-- RELEVANT FILES: .copilot/prompts/sdlc/ibmbob-sdlc-eval-monitor.md, evals/datasets/README.md, evals/reports/README.md -->
---
artifact_id: EVAL-REQ-XXXX
artifact_type: eval_report
status: draft
prompt_id: ibmbob-sdlc-eval-monitor
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - RR-REQ-XXXX
review_record_refs:
  - RR-REQ-XXXX
language: ja
---

# Eval Report

## 評価対象
- どの仕様を確認するかを書く

## 既存カバー
- すでに見えている評価観点を書く

## 未カバー
- まだ足りない観点を書く

## 優先度
- 先に見るべきものを書く

## 次アクション
- 次の実務アクションを書く

