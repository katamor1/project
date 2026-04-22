<!-- docs/copilot-studio/custom-engine-bridge/fallback-policy.md -->
<!-- Explains why custom engine is the primary bridge path and how other connection styles are treated as fallbacks. -->
<!-- This exists so future bridge work does not accidentally split effort across multiple primary entry mechanisms. -->
<!-- RELEVANT FILES: docs/copilot-studio/custom-engine-bridge/README.md, docs/external-runtime/legacy-kb/architecture.md, docs/copilot-studio/custom-engine-bridge/pilot-runbook.md -->
# Fallback Policy

## Primary Path
- `Custom Engine` を主経路にする
- Copilot 側は薄い router と identity guard に徹する
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
