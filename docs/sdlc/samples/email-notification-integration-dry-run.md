<!-- docs/sdlc/samples/email-notification-integration-dry-run.md -->
<!-- Shows a minimal integration lane dry run for the email notification setting example. -->
<!-- This exists so the team can verify how execution artifacts become code draft bundles and an approval pack. -->
<!-- RELEVANT FILES: .copilot/prompts/P15_integration_bundle_planner.md, .copilot/routing/integration-matrix.yaml, docs/sdlc/templates/approval-pack.md -->
# Email Notification Integration Dry Run

この sample は、`execution_bundle_packet` から `approval_pack` までを最小構成でつなぎます。

## execution_bundle_packet excerpt
```json
{
  "request_id": "REQ-EMAIL-NOTIFY-001",
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTING",
  "target_paths": {
    "frontend": ["frontend/app/settings/notifications/page.tsx"],
    "backend": ["backend/api/notification_settings.py"],
    "db": ["db/migrations/20260422_add_notification_setting.sql"],
    "tests": ["tests/integration/test_notification_settings.py"]
  },
  "next_agent": "P15_integration_bundle_planner"
}
```

## code_draft_packet
```json
{
  "request_id": "REQ-EMAIL-NOTIFY-001",
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTING",
  "execution_bundle_ref": "EXEC-BUNDLE-REQ-EMAIL-NOTIFY-001",
  "target_repo_map_ref": "TREEMAP-SINGLE-APP-001",
  "draft_targets": [
    "frontend/app/settings/notifications/page.tsx",
    "backend/api/notification_settings.py",
    "db/migrations/20260422_add_notification_setting.sql",
    "tests/integration/test_notification_settings.py"
  ],
  "constraints": ["single app layout", "skeleton code only"],
  "done_criteria": ["draft bundles exist", "approval pack is explicit"],
  "next_agent": "P16_frontend_draft_author"
}
```

## frontend_draft_bundle
```json
{
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTING",
  "files": [
    {
      "path": "frontend/app/settings/notifications/page.tsx",
      "language": "tsx",
      "purpose": "通知設定画面の skeleton page",
      "skeleton_code": "import { NotificationToggle } from '@/components/notification-toggle';\n\nexport default function NotificationSettingsPage() {\n  // TODO: load current setting and wire actions\n  return <NotificationToggle enabled={false} />;\n}",
      "open_questions": ["初回表示で server action を使うか"]
    }
  ],
  "component_contracts": ["NotificationToggle accepts enabled and onChange"],
  "state_flow": ["load setting", "toggle local state", "submit update"],
  "next_agent": "P17_backend_draft_author"
}
```

## backend_draft_bundle
```json
{
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTING",
  "files": [
    {
      "path": "backend/api/notification_settings.py",
      "language": "python",
      "purpose": "通知設定 API route の skeleton",
      "skeleton_code": "from backend.services.notification_settings import update_notification_setting\n\nasync def patch_notification_setting(user_id: str, enabled: bool) -> dict:\n    # TODO: validate actor and persist update\n    return {'status': 'todo'}",
      "open_questions": ["PATCH endpoint を既存 router に置くか"]
    }
  ],
  "route_contracts": ["PATCH /notification-settings { enabled: boolean }"],
  "service_flow": ["validate request", "load user setting", "persist update"],
  "auth_rules": ["本人のみ更新可能"],
  "next_agent": "P18_sqlite_draft_author"
}
```

## sqlite_draft_bundle
```json
{
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTING",
  "files": [
    {
      "path": "db/migrations/20260422_add_notification_setting.sql",
      "language": "sql",
      "purpose": "通知設定列の migration skeleton",
      "skeleton_code": "ALTER TABLE user_settings ADD COLUMN email_notifications_enabled INTEGER NOT NULL DEFAULT 1;\n-- TODO: backfill only if legacy null rows exist",
      "open_questions": ["既存設定値の backfill が必要か"]
    }
  ],
  "schema_changes": ["user_settings.email_notifications_enabled を追加"],
  "migration_notes": ["default 1 で開始"],
  "rollback_notes": ["column drop 可否を SQLite 制約で再確認"],
  "next_agent": "P19_test_draft_author"
}
```

## test_draft_bundle
```json
{
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTING",
  "files": [
    {
      "path": "tests/integration/test_notification_settings.py",
      "language": "python",
      "purpose": "通知設定更新の integration test skeleton",
      "skeleton_code": "async def test_user_can_disable_email_notifications(client, seeded_user):\n    # TODO: submit patch and verify persisted state\n    assert True",
      "open_questions": ["fixture で seeded_user_settings を分けるか"]
    }
  ],
  "unit_cases": ["toggle helper updates local state"],
  "integration_cases": ["PATCH updates persisted setting"],
  "regression_cases": ["既存通知送信は disabled user に送られない"],
  "fixtures": ["seeded_user", "seeded_user_settings"],
  "next_agent": "P20_integration_reviewer"
}
```

## integration_review_findings
```json
{
  "artifact_refs": [
    "FDRAFT-REQ-EMAIL-NOTIFY-001",
    "BDRAFT-REQ-EMAIL-NOTIFY-001",
    "SQLDRAFT-REQ-EMAIL-NOTIFY-001",
    "TDRAFT-REQ-EMAIL-NOTIFY-001"
  ],
  "review_decision": "pass",
  "findings": [],
  "revise_to": null,
  "human_checkpoint": "none",
  "next_agent": "P21_approval_pack_author"
}
```

## approval_pack
```json
{
  "approval_status": "approved",
  "ready_for_repo": true,
  "artifact_refs": [
    "FDRAFT-REQ-EMAIL-NOTIFY-001",
    "BDRAFT-REQ-EMAIL-NOTIFY-001",
    "SQLDRAFT-REQ-EMAIL-NOTIFY-001",
    "TDRAFT-REQ-EMAIL-NOTIFY-001",
    "IRF-REQ-EMAIL-NOTIFY-001"
  ],
  "residual_risks": ["SQLite rollback 手順は実 repo で再確認が必要"],
  "manual_steps": [
    "draft bundle を app repo の対応 path に写す",
    "実装時に auth rule と fixture 前提を最終確認する"
  ],
  "next_agent": "P8_review_record"
}
```
