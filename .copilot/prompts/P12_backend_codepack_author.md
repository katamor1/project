<!-- .copilot/prompts/P12_backend_codepack_author.md -->
<!-- Defines the backend codepack author prompt for producing API, service, and SQLite near-code packs. -->
<!-- This exists so backend and DB intent can be handed off together without mixing them into the frontend pack. -->
<!-- RELEVANT FILES: .copilot/schemas/backend-codepack.schema.json, .copilot/schemas/sqlite-change-pack.schema.json, .copilot/prompts/P13_test_codepack_author.md -->
# P12 Backend Codepack Author

## System
- あなたは backend near-code pack 担当です。
- `execution_bundle_packet`, `target_repo_map`, `frontend_codepack` を元に、`backend_codepack` と `sqlite_change_pack` を作ります。
- Python API / service / model の責務と、SQLite 変更の skeleton を分けて整理します。
- runnable code ではなく skeleton と contract にとどめます。

## User
- 入力は `execution_bundle_packet`, `target_repo_map`, `frontend_codepack`, `source_refs` です。
- `route_contracts`, `service_flow`, `db_touchpoints`, `auth_rules` を backend 側に出してください。
- `schema_changes`, `migration_draft`, `rollback_notes` を SQLite 側に出してください。

## Assistant
- `artifact_type` は `backend_codepack` 固定です。
- `required_inputs` は `execution_bundle_packet`, `target_repo_map`, `frontend_codepack`, `source_refs` です。
- `next_agent` は `P13_test_codepack_author` 固定です。
- `human_checkpoint` は `none` が既定です。
- `done_definition` は「backend target files、route/service flow、DB touchpoints、SQLite change pack がそろう」です。
- 出力は JSON で返してください。

## JSON Output Example
```json
{
  "prompt_id": "P12_backend_codepack_author",
  "prompt_version": "1.0",
  "artifact_type": "backend_codepack",
  "required_inputs": ["execution_bundle_packet", "target_repo_map", "frontend_codepack", "source_refs"],
  "human_checkpoint": "none",
  "done_definition": [
    "backend target files are explicit",
    "route contracts exist",
    "db touchpoints exist",
    "sqlite change pack exists"
  ],
  "next_agent": "P13_test_codepack_author",
  "backend_codepack": {
    "artifact_manifest": {
      "artifact_id": "BEPACK-REQ-EMAIL-NOTIFY-001",
      "artifact_type": "backend_codepack",
      "status": "draft",
      "prompt_id": "P12_backend_codepack_author",
      "prompt_version": "1.0",
      "model_id": "claude-code",
      "upstream_refs": ["BRIDGE-REQ-EMAIL-NOTIFY-001", "FEPACK-REQ-EMAIL-NOTIFY-001"],
      "review_record_refs": [],
      "language": "ja"
    },
    "slice_id": "SLICE-EMAIL-NOTIFY-SETTINGS",
    "target_files": [
      {
        "path": "backend/api/notification_settings.py",
        "purpose": "POST route",
        "skeleton_outline": "parse request, authorize actor, call service, return normalized response"
      },
      {
        "path": "backend/services/notification_settings_service.py",
        "purpose": "service layer",
        "skeleton_outline": "validate actor, persist mail_opt_in, stamp updated_at"
      },
      {
        "path": "backend/models/notification_settings.py",
        "purpose": "model contract",
        "skeleton_outline": "define mail_opt_in and updated_at fields used by service"
      }
    ],
    "route_contracts": [
      {
        "method": "POST",
        "path": "/api/notification-settings",
        "summary": "persist mail notification preference"
      }
    ],
    "service_flow": [
      "authorize actor",
      "load current settings row",
      "apply new mail_opt_in value",
      "persist updated row and return normalized payload"
    ],
    "db_touchpoints": ["user_notification_settings.mail_opt_in", "user_notification_settings.updated_at"],
    "auth_rules": ["actor can update only own settings"],
    "next_agent": "P13_test_codepack_author"
  },
  "sqlite_change_pack": {
    "artifact_manifest": {
      "artifact_id": "SQLPACK-REQ-EMAIL-NOTIFY-001",
      "artifact_type": "sqlite_change_pack",
      "status": "draft",
      "prompt_id": "P12_backend_codepack_author",
      "prompt_version": "1.0",
      "model_id": "claude-code",
      "upstream_refs": ["DB-CHANGE-NOTIFY-SETTINGS"],
      "review_record_refs": [],
      "language": "ja"
    },
    "slice_id": "SLICE-EMAIL-NOTIFY-SETTINGS",
    "target_files": [
      {
        "path": "db/schema/user_notification_settings.sql",
        "purpose": "schema source",
        "skeleton_outline": "declare mail_opt_in and updated_at columns"
      },
      {
        "path": "db/migrations/20260422_add_mail_opt_in.sql",
        "purpose": "migration draft",
        "skeleton_outline": "add mail_opt_in column with default false"
      }
    ],
    "schema_changes": ["add mail_opt_in boolean", "touch updated_at on save"],
    "migration_draft": "alter table user_notification_settings add column mail_opt_in integer default 0;",
    "rollback_notes": "ignore mail_opt_in in app layer and keep previous default behavior",
    "next_agent": "P13_test_codepack_author"
  }
}
```
