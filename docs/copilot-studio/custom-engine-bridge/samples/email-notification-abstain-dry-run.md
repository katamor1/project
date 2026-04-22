<!-- docs/copilot-studio/custom-engine-bridge/samples/email-notification-abstain-dry-run.md -->
<!-- Shows a minimal bridge flow where low confidence or access mismatch returns an abstain and checkpoint response. -->
<!-- This exists so the team can verify user-facing abstain behavior before any downstream generation starts. -->
<!-- RELEVANT FILES: .copilot/schemas/bridge-human-checkpoint.schema.json, .copilot/schemas/bridge-response.schema.json, .copilot/prompts/C4_bridge_response_shaper.md -->
# Email Notification Abstain Dry Run

## bridge_request_packet
```json
{
  "request_id": "BRREQ-EMAIL-NOTIFY-ABSTAIN-001",
  "user_intent": "通知設定変更の詳細設計を作りたい",
  "requested_outcome": "detail_design",
  "requested_lane": "design",
  "locale": "ja-JP",
  "conversation_context_ref": "thread://email-notify-abstain"
}
```

## bridge_human_checkpoint
```json
{
  "reason": "authoritative_conflict",
  "required_user_action": "承認済みの通知設定仕様書を 1 つに絞って指定してください。",
  "blocked_target": "P2_detail_design_author",
  "retry_entry": "C0_copilot_entry_router"
}
```

## artifact_context_packet
```json
null
```

## bridge_response
```json
{
  "response_mode": "abstain",
  "user_message": "旧仕様の根拠が競合しているため、今は詳細設計を開始しません。承認済みの基準文書を 1 つ指定してもらえれば再実行できます。",
  "downstream_target": "none",
  "handoff_state": "human_checkpoint_required",
  "human_checkpoint": {
    "reason": "authoritative_conflict",
    "required_user_action": "承認済みの通知設定仕様書を 1 つに絞って指定してください。",
    "blocked_target": "P2_detail_design_author",
    "retry_entry": "C0_copilot_entry_router"
  },
  "citations_policy": "no citation on abstain"
}
```
