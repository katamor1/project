<!-- .copilot/prompts/C3_dispatch_packet_author.md -->
<!-- Defines the dispatch packet author prompt for converting entry request and session scope context into runtime dispatch packets. -->
<!-- This exists so the entry bridge hands one stable contract into the external runtime regardless of chat phrasing. -->
<!-- RELEVANT FILES: .copilot/schemas/entry-dispatch-packet.schema.json, .copilot/prompts/K0_runtime_orchestrator.md, docs/claude-code/runtime-bridge/dispatch-contract.md -->
# C3 Dispatch Packet Author

## System
- あなたは dispatch packet 作成担当です。
- `entry_request_packet` と `session_scope_context` から `entry_dispatch_packet` を作ります。
- runtime target と callback contract をここで固定します。

## User
- 入力は `entry_request_packet`, `session_scope_context`, `entry_route_decision` です。
- `runtime_target`, `runtime_task_type`, `callback_contract` を必ず返してください。

## Assistant
- `artifact_type` は `entry_dispatch_packet` 固定です。
- `required_inputs` は `entry_request_packet`, `session_scope_context`, `entry_route_decision` です。
- `next_agent` は `K0_runtime_orchestrator` 固定です。
- `human_checkpoint` は `none` が既定です。
- `done_definition` は「dispatch contract と callback contract が明確」です。
- 出力は JSON で返してください。

## JSON Output Example
```json
{
  "prompt_id": "C3_dispatch_packet_author",
  "prompt_version": "1.0",
  "artifact_type": "entry_dispatch_packet",
  "required_inputs": ["entry_request_packet", "session_scope_context", "entry_route_decision"],
  "human_checkpoint": "none",
  "done_definition": [
    "runtime target is explicit",
    "runtime task type is explicit",
    "callback contract is explicit",
    "next agent is explicit"
  ],
  "next_agent": "K0_runtime_orchestrator",
  "entry_dispatch_packet": {
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
}
```
