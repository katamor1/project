<!-- ibm-bob/mode-pack/handoff-contracts.md -->
<!-- Maps the Bob user-facing aliases to the unchanged canonical packet and schema names. -->
<!-- This exists so the Bob docs stay readable without forking the canonical C, K, P, and LS contracts. -->
<!-- RELEVANT FILES: .copilot/schemas/bridge-request-packet.schema.json, .copilot/schemas/artifact-context-packet.schema.json, docs/copilot-studio/legacy-search/shared-io-contract.yaml -->
# Handoff Contracts

Bob 向け docs では alias を使います。

ただし canonical schema 名は変えません。

正本は `.copilot/schemas/` と `docs/copilot-studio/legacy-search/shared-io-contract.yaml` のままです。

## Alias Table

| Bob Alias | Canonical Name | Canonical File |
|---|---|---|
| `entry_request_packet` | `bridge_request_packet` | `.copilot/schemas/bridge-request-packet.schema.json` |
| `session_scope_context` | `bridge_identity_context` | `.copilot/schemas/bridge-identity-context.schema.json` |
| `entry_dispatch_packet` | `bridge_dispatch_packet` | `.copilot/schemas/bridge-dispatch-packet.schema.json` |
| `entry_response` | `bridge_response` | `.copilot/schemas/bridge-response.schema.json` |
| `entry_human_checkpoint` | `bridge_human_checkpoint` | `.copilot/schemas/bridge-human-checkpoint.schema.json` |
| `entry_trace` | `bridge_trace` | `.copilot/schemas/bridge-trace.schema.json` |

## Unchanged Names

次は alias を増やしません。

- `artifact_context_packet`
- `evidence_bundle`
- `request_packet`
- `scope_packet`
- `implementation_packet`
- `fullstack_slice`
- `test_pack`
- `review_record`
- `eval_report`
- `handoff_packet`
- `candidate_sources`

## Operating Rule

Bob docs では alias を使ってよいです。

実際の返却 shape は canonical 名に合わせます。

new family でも old family でも、このルールは同じです。

