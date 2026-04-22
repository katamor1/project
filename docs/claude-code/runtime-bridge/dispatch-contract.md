<!-- docs/claude-code/runtime-bridge/dispatch-contract.md -->
<!-- Defines how entry packets are transformed from Claude Code chat intent into external runtime dispatch packets. -->
<!-- This exists so runtime entry stays stable even if Claude Code prompt wording changes later. -->
<!-- RELEVANT FILES: .copilot/schemas/entry-request-packet.schema.json, .copilot/schemas/entry-dispatch-packet.schema.json, docs/external-runtime/legacy-kb/api-spec.md -->
# Dispatch Contract

## Transform
`chat request -> entry_request_packet -> entry_dispatch_packet -> runtime_request`

## Mapping Rules
- `user_intent` -> `runtime_task_type`
- `requested_lane` -> `runtime_target`
- `locale` -> `callback_contract.locale`
- `session_context_ref` -> `callback_contract.session_context_ref`

## Runtime Target Defaults
- `basic_design`, `detail_design`, `functional_spec`, `integration_spec`, `implementation`, `test_pack`:
  `runtime_target = legacy-kb-query`
- `legacy_diff`:
  `runtime_target = legacy-kb-query`, `runtime_task_type = diff`
- `review_record`:
  `runtime_target = review-lane`
- `eval`:
  `runtime_target = eval-lane`

## Callback Contract
- `response_agent`: `C4_entry_response_shaper`
- `expected_result_type`
- `session_context_ref`
- `locale`

## No Guessing Rule
- request が lane に落ちない時は dispatch しない
- `C0` に戻して checkpoint にする
