<!-- docs/claude-code/runtime-bridge/samples/email-notification-entry-dry-run.md -->
<!-- Shows a minimal bridge flow for a source-backed design request from Claude Code into the external runtime. -->
<!-- This exists so the team can verify request normalization, dispatch, runtime handoff, and user-facing response shaping. -->
<!-- RELEVANT FILES: .copilot/schemas/entry-request-packet.schema.json, .copilot/schemas/entry-dispatch-packet.schema.json, .copilot/schemas/entry-response.schema.json -->
# Email Notification Entry Dry Run

## entry_request_packet
```json
{
  "request_id": "BRREQ-EMAIL-NOTIFY-001",
  "user_intent": "通知設定変更の基本設計を根拠付きで作りたい",
  "requested_outcome": "basic_design",
  "requested_lane": "design",
  "locale": "ja-JP",
  "session_context_ref": "thread://email-notify"
}
```

## session_scope_context
```json
{
  "user_principal": "pm@example.com",
  "workspace_roots": ["workspace://project"],
  "repo_refs": ["repo://project"],
  "connector_scopes": ["sharepoint.read", "onedrive.read"],
  "execution_surface": "claude-code",
  "allowed_targets": ["legacy_kb.query", "sdlc.basic_design"]
}
```

## entry_dispatch_packet
```json
{
  "entry_request_ref": "BRREQ-EMAIL-NOTIFY-001",
  "session_scope_ref": "SSC-EMAIL-NOTIFY-001",
  "runtime_target": "legacy-kb-query",
  "runtime_task_type": "source_backed_generation",
  "callback_contract": {
    "response_agent": "C4_entry_response_shaper",
    "expected_result_type": "artifact_context_packet",
    "session_context_ref": "thread://email-notify",
    "locale": "ja-JP"
  },
  "next_agent": "K0_runtime_orchestrator"
}
```

## artifact_context_packet
```json
{
  "artifact_request": {
    "artifact_type": "basic_design",
    "target_agent": "P2_basic_design_author"
  },
  "evidence_bundle_ref": "EVB-BASIC-EMAIL-NOTIFY-001",
  "resolved_entities": ["screen:notification_settings", "api:patch_notification_setting"],
  "open_questions": [],
  "allowed_assertions": ["通知設定画面の主要 UI", "更新 API の存在"],
  "evidence_coverage": "approved legacy sources cover core scope",
  "authority_conflict_note": "no conflict",
  "next_agent": "P2_basic_design_author"
}
```

## entry_response
```json
{
  "response_mode": "handoff",
  "user_message": "旧仕様の根拠を確認できたので、基本設計の下書きを開始します。主要画面と更新 API は根拠付きで渡せます。",
  "downstream_target": "P2_basic_design_author",
  "handoff_state": "artifact_context_packet_ready",
  "human_checkpoint": null,
  "citations_policy": "source-backed summary only"
}
```
