<!-- ibm-bob/mode-pack/handoff-contracts.md -->
<!-- Maps the IBM Bob user-facing handoff names to the canonical schema and artifact names already present in the repo. -->
<!-- This exists so the Bob mode pack can use neutral names without forking the canonical contracts. -->
<!-- RELEVANT FILES: .copilot/schemas/bridge-request-packet.schema.json, .copilot/schemas/artifact-context-packet.schema.json, docs/copilot-studio/legacy-search/shared-io-contract.yaml -->
# Handoff Contracts

## Rule

Bob 向け docs では user-facing の呼び方だけ中立化します。

canonical source は `.copilot/schemas/` と `docs/copilot-studio/legacy-search/shared-io-contract.yaml` のままです。

## Alias Table

| Bob Alias | Canonical Name | Canonical File |
|---|---|---|
| `entry_request_packet` | `bridge_request_packet` | `.copilot/schemas/bridge-request-packet.schema.json` |
| `session_scope_context` | `bridge_identity_context` | `.copilot/schemas/bridge-identity-context.schema.json` |
| `entry_dispatch_packet` | `bridge_dispatch_packet` | `.copilot/schemas/bridge-dispatch-packet.schema.json` |
| `entry_response` | `bridge_response` | `.copilot/schemas/bridge-response.schema.json` |
| `entry_human_checkpoint` | `bridge_human_checkpoint` | `.copilot/schemas/bridge-human-checkpoint.schema.json` |
| `entry_trace` | `bridge_trace` | `.copilot/schemas/bridge-trace.schema.json` |

## Unchanged Canonical Names

以下は alias を増やしません。

- `evidence_bundle`
- `artifact_context_packet`
- `request_packet`
- `scope_packet`
- `implementation_packet`
- `fullstack_slice`
- `test_pack`
- `review_record`
- `eval_report`
- `handoff_packet`
- `candidate_sources`

## Legacy Search Contract

`LS-*` は canonical contract をそのまま使います。

特に次は変えません。

- `next_agent`
- `handoff_packet`
- `candidate_sources`
- `chat_response`
- `pm_copy_template`

## Bob Operating Rule

Bob mode の説明では alias を使ってもよいです。

ただし実際に返す JSON は canonical contract に合わせます。
