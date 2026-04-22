<!-- docs/copilot-studio/custom-engine-bridge/samples/email-notification-diff-dry-run.md -->
<!-- Shows a minimal bridge flow for a legacy diff request that returns a user-facing diff summary. -->
<!-- This exists so the team can verify diff dispatch, gate review, and Copilot response shaping without starting a P-lane. -->
<!-- RELEVANT FILES: .copilot/schemas/bridge-request-packet.schema.json, .copilot/schemas/bridge-response.schema.json, docs/external-runtime/legacy-kb/samples/email-notification-diff-dry-run.md -->
# Email Notification Diff Dry Run

## bridge_request_packet
```json
{
  "request_id": "BRREQ-EMAIL-NOTIFY-DIFF-001",
  "user_intent": "旧仕様との差分を見たい",
  "requested_outcome": "legacy_diff",
  "requested_lane": "diff",
  "locale": "ja-JP",
  "conversation_context_ref": "thread://email-notify-diff"
}
```

## bridge_dispatch_packet
```json
{
  "bridge_request_ref": "BRREQ-EMAIL-NOTIFY-DIFF-001",
  "identity_context_ref": "BID-EMAIL-NOTIFY-001",
  "runtime_target": "legacy-kb-query",
  "runtime_task_type": "diff",
  "callback_contract": {
    "response_agent": "C4_bridge_response_shaper",
    "expected_result_type": "diff_report",
    "context_ref": "thread://email-notify-diff",
    "locale": "ja-JP"
  },
  "next_agent": "K0_runtime_orchestrator"
}
```

## artifact_context_packet
```json
null
```

## diff_report
```json
{
  "diff_items": [
    {
      "area": "UI",
      "summary": "旧仕様は email のみ、新要求は digest frequency を追加",
      "source_refs": ["DOC-EMAIL-NOTIFY-SPEC-001#p.4"]
    }
  ],
  "carry_over_items": [
    {
      "area": "API",
      "summary": "PATCH で設定更新する構造は踏襲",
      "source_refs": ["DOC-EMAIL-NOTIFY-SPEC-001#p.6"]
    }
  ],
  "unknown_items": [],
  "impact_estimate": "medium",
  "next_agent": "K8_evidence_reviewer"
}
```

## bridge_response
```json
{
  "response_mode": "answer",
  "user_message": "旧仕様との差分は 1 点です。UI では digest frequency が新規追加で、更新 API の基本構造は踏襲できます。不明点は今のところありません。",
  "downstream_target": "none",
  "handoff_state": "diff_completed",
  "human_checkpoint": null,
  "citations_policy": "source-backed summary only"
}
```
