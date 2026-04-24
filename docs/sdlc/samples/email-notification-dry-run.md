<!-- docs/sdlc/samples/email-notification-dry-run.md -->
<!-- Shows one dry-run scenario from request packet to review record using the email notification example. -->
<!-- This exists so the team can verify prompt, schema, and template connections by hand. -->
<!-- RELEVANT FILES: .copilot/schemas/request-packet.schema.json, .copilot/schemas/review-record.schema.json, docs/sdlc/templates/basic-design.md -->
# Email Notification Dry Run

## Scenario
`agent-research.md` にあるメール通知設定の例を、上流 SDLC の最小フローでなぞります。

## request_packet
```json
{
  "request_id": "REQ-EMAIL-NOTIFY-001",
  "request_title": "通知設定メールの仕様整理",
  "request_summary": "通知設定画面からメール通知条件を設計対象として整理する",
  "business_goal": "PM が基本設計へ進めるための要求整理を終える",
  "change_type": "change_request",
  "constraints": ["配信基盤の置き換えは今回対象外"],
  "success_signals": ["対象と非対象が明確", "基本設計へ渡せる"],
  "source_refs": ["agent-research.md"],
  "requested_by": "project-team",
  "language": "ja",
  "open_questions": [],
  "next_agent": "ibmbob-sdlc-scope"
}
```

## scope_packet
```json
{
  "request_id": "REQ-EMAIL-NOTIFY-001",
  "artifact_manifest": {
    "artifact_id": "SCOPE-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "scope",
    "status": "draft",
    "prompt_id": "ibmbob-sdlc-scope",
    "prompt_version": "1.0",
    "model_id": "copilot",
    "upstream_refs": ["REQ-EMAIL-NOTIFY-001"],
    "review_record_refs": [],
    "language": "ja"
  },
  "scope_summary": "通知設定のメール通知条件、送信契機、入力責務を整理する",
  "in_scope": ["通知 ON/OFF 条件", "送信契機", "画面入力項目"],
  "out_of_scope": ["メール配信基盤の再設計", "通知テンプレート管理画面"],
  "acceptance_criteria": ["基本設計の入力として十分"],
  "assumptions": ["送信処理は既存ジョブを利用"],
  "glossary": ["通知設定", "メール通知"],
  "open_questions": ["複数通知種別の優先順位は既存仕様を確認"],
  "next_agent": "ibmbob-sdlc-basic-design-author"
}
```

## work_packet
```json
{
  "request_id": "REQ-EMAIL-NOTIFY-001",
  "artifact_manifest": {
    "artifact_id": "BASIC-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "basic_design",
    "status": "draft",
    "prompt_id": "ibmbob-sdlc-basic-design-author",
    "prompt_version": "1.0",
    "model_id": "copilot",
    "upstream_refs": ["SCOPE-REQ-EMAIL-NOTIFY-001"],
    "review_record_refs": [],
    "language": "ja"
  },
  "stage": "basic_design",
  "objective": "通知設定メール機能の基本設計を作る",
  "inputs": ["scope packet", "既存画面メモ"],
  "decisions": ["送信基盤は既存再利用", "設定変更は即時反映"],
  "constraints": ["画面構成の全面刷新はしない"],
  "traceability": [
    { "source_id": "in_scope[0]", "target_section": "業務ルール" }
  ],
  "next_agent": "ibmbob-sdlc-spec-reviewer"
}
```

## basic_design excerpt
```md
---
artifact_id: BASIC-REQ-EMAIL-NOTIFY-001
artifact_type: basic_design
status: draft
prompt_id: ibmbob-sdlc-basic-design-author
prompt_version: "1.0"
model_id: copilot
upstream_refs:
  - SCOPE-REQ-EMAIL-NOTIFY-001
review_record_refs: []
language: ja
---

# Basic Design

## 目的
- 通知設定の責務を整理する

## 画面
- 通知 ON/OFF と通知種別を設定する

## API
- 設定保存 API を 1 本定義する

## 業務ルール
- OFF の時はメール送信を行わない
```

## review_findings
```json
{
  "request_id": "REQ-EMAIL-NOTIFY-001",
  "artifact_type": "basic_design",
  "review_decision": "revise",
  "summary": "基本設計の方向性は妥当だが、権限制約が不足している",
  "findings": [
    {
      "severity": "medium",
      "title": "権限制御の記載不足",
      "detail": "誰が通知設定を変更できるかが未定義",
      "owner_section": "業務ルール"
    }
  ],
  "revise_to": "ibmbob-sdlc-basic-design-author",
  "human_checkpoint": false,
  "next_agent": "ibmbob-sdlc-basic-design-author"
}
```

## review_record
```json
{
  "request_id": "REQ-EMAIL-NOTIFY-001",
  "artifact_manifest": {
    "artifact_id": "RR-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "review_record",
    "status": "draft",
    "prompt_id": "ibmbob-sdlc-review-record",
    "prompt_version": "1.0",
    "model_id": "copilot",
    "upstream_refs": ["RF-REQ-EMAIL-NOTIFY-001-BASIC"],
    "review_record_refs": [],
    "language": "ja"
  },
  "reviewed_artifacts": ["BASIC-REQ-EMAIL-NOTIFY-001"],
  "review_cycles": [
    {
      "artifact_type": "basic_design",
      "decision": "revise",
      "reviewed_at": "2026-04-22",
      "reviewer_prompt_id": "ibmbob-sdlc-spec-reviewer",
      "notes": "権限制御を追記して再レビュー"
    }
  ],
  "unresolved_risks": ["権限制御の未確定"],
  "approval_note": "再レビュー待ち",
  "next_agent": "ibmbob-sdlc-eval-monitor"
}
```

