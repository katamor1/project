<!-- docs/copilot-studio/custom-engine-bridge/dispatch-contract.md -->
<!-- Defines how bridge packets are transformed from Copilot chat intent into external runtime dispatch packets. -->
<!-- This exists so runtime entry stays stable even if Copilot prompt wording changes later. -->
<!-- RELEVANT FILES: .copilot/schemas/bridge-request-packet.schema.json, .copilot/schemas/bridge-dispatch-packet.schema.json, docs/external-runtime/legacy-kb/api-spec.md -->
# Dispatch Contract

## Transform
`chat request -> bridge_request_packet -> bridge_dispatch_packet -> runtime_request`

## Mapping Rules
- `user_intent` -> `runtime_task_type`
- `requested_lane` -> `runtime_target`
- `locale` -> `callback_contract.locale`
- `conversation_context_ref` -> `callback_contract.context_ref`

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
- `response_agent`: `C4_bridge_response_shaper`
- `expected_result_type`
- `context_ref`
- `locale`

## No Guessing Rule
- request が lane に落ちない時は dispatch しない
- `C0` に戻して checkpoint にする
