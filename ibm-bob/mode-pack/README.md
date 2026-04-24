<!-- ibm-bob/mode-pack/README.md -->
<!-- Explains the IBM Bob mode pack that keeps the old orchestrator family and the new direct-mode family side by side. -->
<!-- This exists so the team can generate one Bob config from one source without mirroring repo trees into `.bob/references`. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/modes/custom_modes.source.yaml, ibm-bob/mode-pack/scripts/install_mode_pack.py, ibm-bob/mode-pack/routing/direct-mode-flow.json -->
# IBM Bob Mode Pack

このディレクトリが、IBM Bob 向け mode pack の source of truth です。

repo 直下の `.bob/` は source ではありません。

`.bob/custom_modes.yaml` と `.bob/rules-{slug}/` は、ここから生成する output です。

`.bob/ibm-bob/references/` は curated bundle です。

source tree の mirror ではありません。

## Two Families

### `ibmbob-orchestrator` family

これは既存 family です。

copied workspace を作り、固定 stage flow で run を進めます。

`routing/stage-flow.json` と既存 evaluator をそのまま使います。

### `ibmbob` direct-mode family

これは新 family です。

`C / K / P / LS` を Bob custom mode に直接写します。

legacy-search と source-backed authoring を、固定 run なしで扱います。

正本の flow は `routing/direct-mode-flow.json` です。

## Canonical Sources

Bob pack 自体は overlay です。

正本の設計資産は次のまま使います。

- `.copilot/prompts`
- `.copilot/routing`
- `.copilot/schemas`
- `docs/copilot-studio`
- `docs/external-runtime`
- `docs/sdlc`

new direct-mode family の canonical naming は、Bob slug と Bob packet 名へ統一しています。

## Main Source Files

- `modes/custom_modes.source.yaml`: 全 mode の正本
- `rules/`: 各 mode の rule source
- `rules/shared/`: new family へ展開する共通 rule
- `routing/stage-flow.json`: old family の固定 run
- `routing/direct-mode-flow.json`: new family の direct flow
- `scripts/install_mode_pack.py`: generated `.bob/` を作る installer
- `scripts/reference_manifest.py`: curated reference install path の正本
- `scripts/validate_mode_pack.py`: 後方互換と静的整合を確認する validator
- `scripts/evaluate_mode_pack.py`: old family の simulate / real evaluator

## Generated Output

installer は 1 つの `.bob/custom_modes.yaml` を生成します。

その中に old family と new family の両方を入れます。

同時に `.bob/rules-{slug}/`、`.bob/ibm-bob/stage-flow.json`、`.bob/ibm-bob/references/**` を生成します。

## Main Commands

```powershell
py -3 ibm-bob/mode-pack/scripts/install_mode_pack.py --workspace <workspace>
py -3 ibm-bob/mode-pack/scripts/validate_mode_pack.py
py -3 ibm-bob/mode-pack/scripts/evaluate_mode_pack.py --case all --execution-mode simulate
```

## Docs In This Pack

- `mode-catalog.md`: old / new family の mode 一覧
- `routing-map.md`: 2 family の flow 整理
- `tool-profile.md`: permission と edit 制限
- `handoff-contracts.md`: Bob alias と canonical contract の対応
- `legacy-search-guide.md`: `ibmbob-legacy-search-*` の運用
- `sdlc-guide.md`: `ibmbob-sdlc-*` の運用
- `pilot-runbook.md`: manual pilot 手順
- `samples/direct-modes/`: new family 用 dry-run
