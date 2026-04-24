<!-- docs/sdlc/samples/email-notification-execution-dry-run.md -->
<!-- Shows one dry-run scenario from repo bridge to execution review using the email notification example. -->
<!-- This exists so the team can verify the execution pack lane by hand before touching a real app repo. -->
<!-- RELEVANT FILES: .copilot/schemas/execution-bundle-packet.schema.json, .copilot/schemas/execution-review-findings.schema.json, docs/sdlc/templates/frontend-codepack.md -->
# Email Notification Execution Dry Run

## Scenario
`repo_bridge` の先を、single app repo 向け Near-code Pack として handoff する流れをなぞります。

## repo_bridge excerpt
```json
{
  "artifact_manifest": {
    "artifact_id": "BRIDGE-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "repo_bridge",
    "status": "draft",
    "prompt_id": "MANUAL_BRIDGE_TEMPLATE",
    "prompt_version": "1.0",
    "model_id": "copilot",
    "upstream_refs": ["SLICE-REQ-EMAIL-NOTIFY-001", "TEST-REQ-EMAIL-NOTIFY-001"],
    "review_record_refs": [],
    "language": "ja"
  },
  "target_stack": ["Next.js", "TypeScript", "Python", "SQLite"],
  "target_paths": [
    { "path": "frontend/app/settings/notifications/page.tsx", "purpose": "通知設定 UI", "owner": "frontend" },
    { "path": "backend/api/notification_settings.py", "purpose": "保存 API", "owner": "backend" },
    { "path": "db/schema/user_notification_settings.sql", "purpose": "SQLite schema", "owner": "db" },
    { "path": "tests/integration/notification_settings_api_test.py", "purpose": "統合テスト", "owner": "test" }
  ],
  "handoff_order": ["frontend", "backend", "db", "tests"],
  "manual_steps": ["path 規約を app repo で最終確認する"],
  "open_questions": [],
  "next_agent": "ibmbob-sdlc-review-record"
}
```

## execution_bundle_packet
```json
{
  "request_id": "REQ-EMAIL-NOTIFY-001",
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTINGS",
  "repo_bridge_ref": "BRIDGE-REQ-EMAIL-NOTIFY-001",
  "target_repo_shape": "single_app_layout",
  "target_paths": [
    { "path": "frontend/app/settings/notifications/page.tsx", "purpose": "通知設定ページ", "owner": "frontend" },
    { "path": "backend/api/notification_settings.py", "purpose": "保存 API", "owner": "backend" },
    { "path": "db/schema/user_notification_settings.sql", "purpose": "schema source", "owner": "db" },
    { "path": "tests/integration/notification_settings_api_test.py", "purpose": "integration tests", "owner": "test" }
  ],
  "bundle_targets": ["frontend_codepack", "backend_codepack", "sqlite_change_pack", "test_codepack"],
  "constraints": ["single app layout を守る", "path 規約は frontend/backend/db/tests に固定する"],
  "done_criteria": ["各 pack に target file がある", "path と ownership が一意", "test skeleton がある"],
  "next_agent": "ibmbob-sdlc-frontend-codepack-author"
}
```

## target_repo_map
```json
{
  "repo_shape": "single_app_layout",
  "path_style": "frontend_backend_db",
  "frontend_roots": ["frontend/app", "frontend/components", "frontend/lib"],
  "backend_roots": ["backend/api", "backend/services", "backend/models"],
  "db_roots": ["db/schema", "db/migrations"],
  "test_roots": ["tests/unit", "tests/integration", "tests/regression"],
  "naming_rules": ["page file は kebab-case", "API file は snake_case"],
  "next_agent": "ibmbob-sdlc-frontend-codepack-author"
}
```

## frontend_codepack
```json
{
  "artifact_manifest": {
    "artifact_id": "FEPACK-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "frontend_codepack",
    "status": "draft",
    "prompt_id": "ibmbob-sdlc-frontend-codepack-author",
    "prompt_version": "1.0",
    "model_id": "copilot",
    "upstream_refs": ["BRIDGE-REQ-EMAIL-NOTIFY-001"],
    "review_record_refs": [],
    "language": "ja"
  },
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTINGS",
  "target_files": [
    {
      "path": "frontend/app/settings/notifications/page.tsx",
      "purpose": "通知設定ページ",
      "skeleton_outline": "load current settings, render form shell, call save action"
    },
    {
      "path": "frontend/components/notification-settings-form.tsx",
      "purpose": "form component",
      "skeleton_outline": "hold local toggle state and submit"
    }
  ],
  "component_contracts": [
    {
      "component_name": "NotificationSettingsForm",
      "props": ["initialMailOptIn:boolean", "canEdit:boolean"],
      "responsibility": "通知設定 UI を扱う"
    }
  ],
  "state_flow": ["page loads initial state", "form updates local state", "submit sends API call"],
  "api_calls": [
    { "client_name": "saveNotificationSettings", "endpoint": "POST /api/notification-settings", "usage": "persist toggle value" }
  ],
  "acceptance_refs": ["AC-01", "AC-02"],
  "next_agent": "ibmbob-sdlc-backend-codepack-author"
}
```

## backend_codepack
```json
{
  "artifact_manifest": {
    "artifact_id": "BEPACK-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "backend_codepack",
    "status": "draft",
    "prompt_id": "ibmbob-sdlc-backend-codepack-author",
    "prompt_version": "1.0",
    "model_id": "copilot",
    "upstream_refs": ["BRIDGE-REQ-EMAIL-NOTIFY-001", "FEPACK-REQ-EMAIL-NOTIFY-001"],
    "review_record_refs": [],
    "language": "ja"
  },
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTINGS",
  "target_files": [
    {
      "path": "backend/api/notification_settings.py",
      "purpose": "POST route",
      "skeleton_outline": "parse request, authorize actor, call service, return response"
    }
  ],
  "route_contracts": [
    { "method": "POST", "path": "/api/notification-settings", "summary": "persist mail notification preference" }
  ],
  "service_flow": ["authorize actor", "load current settings", "persist mail_opt_in", "return normalized payload"],
  "db_touchpoints": ["user_notification_settings.mail_opt_in", "user_notification_settings.updated_at"],
  "auth_rules": ["actor can update only own settings"],
  "next_agent": "ibmbob-sdlc-test-codepack-author"
}
```

## sqlite_change_pack
```json
{
  "artifact_manifest": {
    "artifact_id": "SQLPACK-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "sqlite_change_pack",
    "status": "draft",
    "prompt_id": "ibmbob-sdlc-backend-codepack-author",
    "prompt_version": "1.0",
    "model_id": "copilot",
    "upstream_refs": ["DB-CHANGE-NOTIFY-SETTINGS"],
    "review_record_refs": [],
    "language": "ja"
  },
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTINGS",
  "target_files": [
    {
      "path": "db/migrations/20260422_add_mail_opt_in.sql",
      "purpose": "migration draft",
      "skeleton_outline": "add mail_opt_in column with default false"
    }
  ],
  "schema_changes": ["add mail_opt_in boolean", "touch updated_at on save"],
  "migration_draft": "alter table user_notification_settings add column mail_opt_in integer default 0;",
  "rollback_notes": "ignore mail_opt_in in app layer and keep previous behavior",
  "next_agent": "ibmbob-sdlc-test-codepack-author"
}
```

## test_codepack
```json
{
  "artifact_manifest": {
    "artifact_id": "TESTPACK-REQ-EMAIL-NOTIFY-001",
    "artifact_type": "test_codepack",
    "status": "draft",
    "prompt_id": "ibmbob-sdlc-test-codepack-author",
    "prompt_version": "1.0",
    "model_id": "copilot",
    "upstream_refs": ["FEPACK-REQ-EMAIL-NOTIFY-001", "BEPACK-REQ-EMAIL-NOTIFY-001", "SQLPACK-REQ-EMAIL-NOTIFY-001"],
    "review_record_refs": [],
    "language": "ja"
  },
  "slice_id": "SLICE-EMAIL-NOTIFY-SETTINGS",
  "target_files": [
    {
      "path": "tests/integration/notification_settings_api_test.py",
      "purpose": "API integration tests",
      "skeleton_outline": "assert persistence and auth behavior"
    }
  ],
  "unit_cases": [{ "case_id": "UT-01", "title": "client sends mail_opt_in", "focus": "request payload" }],
  "integration_cases": [{ "case_id": "IT-01", "title": "save endpoint persists value", "focus": "api + sqlite write" }],
  "regression_cases": [{ "case_id": "RG-01", "title": "settings page still loads", "focus": "legacy behavior" }],
  "fixtures": ["user with existing settings", "user without edit permission"],
  "next_agent": "ibmbob-sdlc-execution-reviewer"
}
```

## execution_review_findings
```json
{
  "artifact_refs": ["FEPACK-REQ-EMAIL-NOTIFY-001", "BEPACK-REQ-EMAIL-NOTIFY-001", "SQLPACK-REQ-EMAIL-NOTIFY-001", "TESTPACK-REQ-EMAIL-NOTIFY-001"],
  "review_decision": "pass",
  "summary": "path mapping と near-code bundle は app repo handoff 可能",
  "findings": [],
  "revise_to": "none",
  "human_checkpoint": false,
  "next_agent": "ibmbob-sdlc-review-record"
}
```

