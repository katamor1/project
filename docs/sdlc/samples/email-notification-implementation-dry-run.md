<!-- docs/sdlc/samples/email-notification-implementation-dry-run.md -->
<!-- Shows one dry-run scenario from detail design to repo bridge using the email notification example. -->
<!-- This exists so the team can verify the implementation lane by hand before touching a real app repo. -->
<!-- RELEVANT FILES: .copilot/schemas/implementation-packet.schema.json, .copilot/schemas/repo-bridge.schema.json, docs/sdlc/templates/repo-bridge.md -->
# Email Notification Implementation Dry Run

## Scenario
`detail_design` が通過した後、メール通知設定を 1 つの full-stack slice として handoff する流れをなぞります。

## detail_design excerpt
```md
---
artifact_id: DETAIL-REQ-EMAIL-NOTIFY-001
artifact_type: detail_design
status: approved
prompt_id: P2_detail_design_author
prompt_version: "1.0"
model_id: claude-code
upstream_refs:
  - BASIC-REQ-EMAIL-NOTIFY-001
review_record_refs:
  - RR-REQ-EMAIL-NOTIFY-001
language: ja
---

# Detail Design

## API 契約
- `POST /api/notification-settings` で mail opt-in を保存する

## 永続化
- `user_notification_settings` に mail_opt_in を保持する

## 権限
- 自分自身の通知設定だけ変更できる
```

## implementation_packet
```json
{
  "request_id": "REQ-EMAIL-NOTIFY-001",
  "detail_design_ref": "DETAIL-REQ-EMAIL-NOTIFY-001",
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTINGS",
  "ui_scope": ["通知設定画面のメール通知トグル", "保存ボタン状態管理"],
  "api_scope": ["POST /api/notification-settings"],
  "db_scope": ["user_notification_settings table"],
  "constraints": ["1 slice に閉じる", "既存 auth を使う"],
  "done_criteria": ["UI と API の責務が明確", "DB 変更が明確"],
  "next_agent": "P5_fullstack_slice_author"
}
```

## fullstack_slice
```json
{
  "artifact_manifest": {
    "artifact_id": "SLICE-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "fullstack_slice",
    "status": "draft",
    "prompt_id": "P5_fullstack_slice_author",
    "prompt_version": "1.0",
    "model_id": "claude-code",
    "upstream_refs": ["DETAIL-REQ-EMAIL-NOTIFY-001", "SLICE-EMAIL-NOTIFY-SETTINGS"],
    "review_record_refs": [],
    "language": "ja"
  },
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTINGS",
  "frontend_changes": [
    {
      "area": "notification-settings-page",
      "file_intent": "トグルと保存状態を扱う",
      "notes": "Next.js / TypeScript で form state を持つ"
    }
  ],
  "backend_changes": [
    {
      "area": "notification-settings-api",
      "file_intent": "保存 API と認可を担う",
      "notes": "Python handler で更新処理を行う"
    }
  ],
  "db_changes": [
    {
      "table_name": "user_notification_settings",
      "fields": ["mail_opt_in", "updated_at"],
      "migration": "mail_opt_in を追加する",
      "backfill": "既存ユーザーは false で初期化する",
      "rollback": "column を参照しない旧処理へ戻す",
      "source_refs": ["DETAIL-REQ-EMAIL-NOTIFY-001"]
    }
  ],
  "contract_refs": ["API-CONTRACT-NOTIFY-UPDATE", "DB-CHANGE-NOTIFY-SETTINGS"],
  "risk_flags": ["既存 API path 命名を app repo 側で確認する"],
  "next_agent": "P6_test_author"
}
```

## test_pack
```json
{
  "artifact_manifest": {
    "artifact_id": "TEST-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "test_pack",
    "status": "draft",
    "prompt_id": "P6_test_author",
    "prompt_version": "1.0",
    "model_id": "claude-code",
    "upstream_refs": ["SLICE-REQ-EMAIL-NOTIFY-001"],
    "review_record_refs": [],
    "language": "ja"
  },
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTINGS",
  "test_levels": ["unit", "integration", "regression"],
  "acceptance_cases": [
    {
      "case_id": "AC-01",
      "level": "integration",
      "title": "通知設定を保存できる",
      "expected_result": "mail opt-in が保存される"
    },
    {
      "case_id": "AC-02",
      "level": "regression",
      "title": "権限なしユーザーは他人の設定を更新できない",
      "expected_result": "更新が拒否される"
    }
  ],
  "fixtures": ["通知設定ありユーザー", "権限なしユーザー"],
  "coverage_focus": ["UI validation", "API authorization", "DB write integrity"],
  "next_agent": "P7_code_reviewer"
}
```

## code_review_findings
```json
{
  "artifact_ref": "SLICE-REQ-EMAIL-NOTIFY-001",
  "review_decision": "pass",
  "summary": "slice boundary と test coverage は handoff 可能",
  "findings": [],
  "revise_to": "none",
  "human_checkpoint": false,
  "next_agent": "P8_review_record"
}
```

## repo_bridge
```json
{
  "artifact_manifest": {
    "artifact_id": "BRIDGE-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "repo_bridge",
    "status": "draft",
    "prompt_id": "MANUAL_BRIDGE_TEMPLATE",
    "prompt_version": "1.0",
    "model_id": "claude-code",
    "upstream_refs": ["SLICE-REQ-EMAIL-NOTIFY-001", "TEST-REQ-EMAIL-NOTIFY-001"],
    "review_record_refs": [],
    "language": "ja"
  },
  "target_stack": ["Next.js", "TypeScript", "Python", "SQLite"],
  "target_paths": [
    {
      "path": "app/settings/notifications/page.tsx",
      "purpose": "通知設定 UI",
      "owner": "frontend"
    },
    {
      "path": "api/notification_settings.py",
      "purpose": "保存 API",
      "owner": "backend"
    },
    {
      "path": "db/schema/user_notification_settings.sql",
      "purpose": "永続化変更",
      "owner": "backend"
    }
  ],
  "handoff_order": ["frontend", "backend", "db", "tests"],
  "manual_steps": [
    "slice artifact を app repo の実装担当へ渡す",
    "API contract と DB change を app repo の命名に合わせて確認する"
  ],
  "open_questions": ["既存 API path の最終命名を app repo で確認する"],
  "next_agent": "P8_review_record"
}
```
