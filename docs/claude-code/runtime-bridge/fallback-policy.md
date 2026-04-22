<!-- docs/claude-code/runtime-bridge/fallback-policy.md -->
<!-- Explains why the Claude Code entry bridge is the primary path and how other connection styles are treated as fallbacks. -->
<!-- This exists so future bridge work does not accidentally split effort across multiple primary entry mechanisms. -->
<!-- RELEVANT FILES: docs/claude-code/runtime-bridge/README.md, docs/external-runtime/legacy-kb/architecture.md, docs/claude-code/runtime-bridge/pilot-runbook.md -->
# Fallback Policy

## Primary Path
- `Claude Code entry bridge` を主経路にする
- Claude Code 側は薄い router と session scope guard に徹する
- retrieval / diff / source-backed generation は外部 runtime に寄せる

## Fallback Only
### `MCP`
- 補助的な tool / resource 連携としてだけ扱う
- 主入口にはしない

### `plugin / OpenAPI action`
- 将来の代替案として文書化だけする
- 今回の bridge pack では主契約にしない

## Why
- routing が 1 本で済む
- checkpoint と abstain の品質を 1 箇所で評価できる
- bridge contract の再設計を避けられる
