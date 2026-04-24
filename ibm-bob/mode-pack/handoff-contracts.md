<!-- ibm-bob/mode-pack/handoff-contracts.md -->
<!-- Lists the migration map from the old direct-mode packet names to the Bob-native canonical names. -->
<!-- This exists so the team can migrate references without keeping a long-lived alias layer. -->
<!-- RELEVANT FILES: .copilot/schemas/entry-request-packet.schema.json, .copilot/schemas/session-scope-context.schema.json, docs/copilot-studio/legacy-search/shared-io-contract.yaml -->
# Handoff Migration Map

new direct-mode family では、Bob-native の canonical 名を使います。

old direct-mode 名は migration note としてだけ残します。

## Entry Packets

| Old Name | New Name | Canonical File |
| --- | --- | --- |
| `bridge_request_packet` | `entry_request_packet` | `.copilot/schemas/entry-request-packet.schema.json` |
| `bridge_identity_context` | `session_scope_context` | `.copilot/schemas/session-scope-context.schema.json` |
| `bridge_dispatch_packet` | `entry_dispatch_packet` | `.copilot/schemas/entry-dispatch-packet.schema.json` |
| `bridge_response` | `entry_response` | `.copilot/schemas/entry-response.schema.json` |
| `bridge_human_checkpoint` | `entry_human_checkpoint` | `.copilot/schemas/entry-human-checkpoint.schema.json` |
| `bridge_trace` | `entry_trace` | `.copilot/schemas/entry-trace.schema.json` |
| `bridge_route_decision` | `entry_route_decision` | `.copilot/prompts/entry/ibmbob-entry-router.md` |
| `bridge_eval_note` | `entry_eval_note` | `.copilot/prompts/entry/ibmbob-entry-eval-monitor.md` |

## Unchanged Names

次は変えません。

- `artifact_context_packet`
- `evidence_bundle`
- `handoff_packet`
- `request_packet`
- `scope_packet`
- `implementation_packet`
- `review_record`
- `eval_report`
- `candidate_sources`

## Operating Rule

new direct-mode family では、新名だけを使います。

old 名を新しい rule や sample へ戻さないでください。
