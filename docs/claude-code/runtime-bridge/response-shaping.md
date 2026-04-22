<!-- docs/claude-code/runtime-bridge/response-shaping.md -->
<!-- Defines how runtime outputs are turned into short Japanese Claude Code-facing messages. -->
<!-- This exists so answer, abstain, checkpoint, and handoff states are presented consistently to users. -->
<!-- RELEVANT FILES: .copilot/schemas/entry-response.schema.json, .copilot/prompts/C4_entry_response_shaper.md, docs/claude-code/runtime-bridge/samples/email-notification-abstain-dry-run.md -->
# Response Shaping

## Response Modes
- `answer`
- `abstain`
- `handoff`
- `checkpoint`

## Japanese Output Rule
- 短く書く
- まず `何が分かったか`
- 次に `何が足りないか`
- 最後に `次に何をするか`

## Mode Rules
### `answer`
- runtime や `P*` が十分な結果を返した時

### `abstain`
- confidence 不足
- ACL mismatch
- authoritative conflict

### `checkpoint`
- session scope の問題
- user action が必要な時

### `handoff`
- downstream lane は走るが、ユーザー向けには開始報告だけ返す時

## Citation Policy
- source-backed 応答だけ citation note を返す
- weak evidence では citation を飾りに使わない
