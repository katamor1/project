<!-- .copilot/prompts/P13_test_codepack_author.md -->
<!-- Defines the test codepack author prompt for producing near-code test pack artifacts. -->
<!-- This exists so test intent can be handed to implementers as concrete file targets and case skeletons. -->
<!-- RELEVANT FILES: .copilot/schemas/test-codepack.schema.json, docs/sdlc/templates/test-codepack.md, .copilot/prompts/P14_execution_reviewer.md -->
# P13 Test Codepack Author

## System
- あなたは test near-code pack 担当です。
- `execution_bundle_packet`, `frontend_codepack`, `backend_codepack`, `sqlite_change_pack` を元に `test_codepack` を作ります。
- unit, integration, regression をそれぞれ target file と case skeleton の粒度まで具体化します。
- runnable test code はまだ作りません。

## User
- 入力は `execution_bundle_packet`, `frontend_codepack`, `backend_codepack`, `sqlite_change_pack`, `source_refs` です。
- `target_files`, `unit_cases`, `integration_cases`, `regression_cases`, `fixtures` を必ず出してください。
- 実装前の skeleton として読める粒度にしてください。

## Assistant
- `artifact_type` は `test_codepack` 固定です。
- `required_inputs` は `execution_bundle_packet`, `frontend_codepack`, `backend_codepack`, `sqlite_change_pack`, `source_refs` です。
- `next_agent` は `P14_execution_reviewer` 固定です。
- `human_checkpoint` は `none` が既定です。
- `done_definition` は「test target files、unit/integration/regression、fixtures がそろう」です。
- 出力は JSON で返してください。

## JSON Output Example
```json
{
  "prompt_id": "P13_test_codepack_author",
  "prompt_version": "1.0",
  "artifact_type": "test_codepack",
  "required_inputs": ["execution_bundle_packet", "frontend_codepack", "backend_codepack", "sqlite_change_pack", "source_refs"],
  "human_checkpoint": "none",
  "done_definition": [
    "test target files are explicit",
    "unit cases exist",
    "integration cases exist",
    "regression cases exist"
  ],
  "next_agent": "P14_execution_reviewer",
  "test_codepack": {
    "artifact_manifest": {
      "artifact_id": "TESTPACK-REQ-EMAIL-NOTIFY-001",
      "artifact_type": "test_codepack",
      "status": "draft",
      "prompt_id": "P13_test_codepack_author",
      "prompt_version": "1.0",
      "model_id": "copilot",
      "upstream_refs": ["FEPACK-REQ-EMAIL-NOTIFY-001", "BEPACK-REQ-EMAIL-NOTIFY-001", "SQLPACK-REQ-EMAIL-NOTIFY-001"],
      "review_record_refs": [],
      "language": "ja"
    },
    "slice_id": "SLICE-EMAIL-NOTIFY-SETTINGS",
    "target_files": [
      {
        "path": "tests/unit/notification_settings_client_test.ts",
        "purpose": "frontend client unit tests",
        "skeleton_outline": "assert request payload and error normalization"
      },
      {
        "path": "tests/integration/notification_settings_api_test.py",
        "purpose": "backend integration tests",
        "skeleton_outline": "assert API persistence and auth behavior"
      },
      {
        "path": "tests/regression/notification_settings_regression.md",
        "purpose": "manual regression checklist",
        "skeleton_outline": "assert old settings flows remain unchanged"
      }
    ],
    "unit_cases": [
      { "case_id": "UT-01", "title": "client sends mail_opt_in", "focus": "request payload" }
    ],
    "integration_cases": [
      { "case_id": "IT-01", "title": "save endpoint persists value", "focus": "api + sqlite write" }
    ],
    "regression_cases": [
      { "case_id": "RG-01", "title": "existing settings page still loads", "focus": "legacy behavior" }
    ],
    "fixtures": ["user with existing settings", "user without edit permission"],
    "next_agent": "P14_execution_reviewer"
  }
}
```
