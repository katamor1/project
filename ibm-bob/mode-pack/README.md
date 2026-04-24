<!-- ibm-bob/mode-pack/README.md -->
<!-- Explains the IBM Bob custom mode pack that overlays the existing Copilot-oriented design assets. -->
<!-- This exists so the team can run the same C, K, P, and LS workflows inside Bob without replacing the canonical source files. -->
<!-- RELEVANT FILES: .bob/custom_modes.yaml, ibm-bob/mode-pack/mode-catalog.md, ibm-bob/mode-pack/routing-map.md -->
<!-- Explains the IBM-Bob SDLC custom mode pack, its generated Bob assets, and the local validation scripts. -->
<!-- This exists so the team can install, validate, and evaluate the mode pack without editing the sample originals. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/modes/custom_modes.source.yaml, ibm-bob/mode-pack/scripts/install_mode_pack.py, ibm-bob/mode-pack/scripts/evaluate_mode_pack.py -->
# IBM Bob Mode Pack

このフォルダは、既存の `Copilot / Claude` 向け設計資産を `IBM Bob` の custom mode として使うための overlay です。

canonical source は変えません。

`.copilot/prompts`, `.copilot/routing`, `.copilot/schemas`, `docs/copilot-studio`, `docs/external-runtime`, `docs/sdlc` はそのまま残します。

Bob 実運用の本体は `.bob/custom_modes.yaml` です。

cross-cutting rule は `.bob/rules/` に置きます。

## What This Pack Does

- `C*` を Bob entry bridge mode に写します。
- `K*` を legacy runtime mode に写します。
- `P*` を SDLC authoring / review / handoff mode に写します。
- `LS-*` を legacy search workflow mode に写します。

## What This Pack Does Not Do

- canonical prompt や schema を置き換えません。
- built-in `plan`, `code`, `advanced`, `orchestrator` を上書きしません。
- app code や runtime service code の新実装は含みません。

## Main Files

- `.bob/custom_modes.yaml`: Bob の custom mode 定義
- `.bob/rules/`: 全 mode にかかる共通ルール
- `mode-catalog.md`: 全 mode の一覧
- `routing-map.md`: Bob 上での流れ
- `tool-profile.md`: tool group と edit 制限
- `handoff-contracts.md`: Bob 向け alias と canonical schema の対応
- `legacy-search-guide.md`: `LS-*` の使い方
- `sdlc-guide.md`: `P*` の使い方
- `pilot-runbook.md`: manual pilot 手順
- `samples/`: generation / diff / review / eval / legacy-search の dry-run

## Default Operating Model

- Bob built-in `Orchestrator` から custom mode を提案・切替します。
- 各 custom mode は、対応する canonical prompt を最初に読みます。
- mode の出力は canonical prompt の JSON contract に合わせます。
- source-backed generation は `artifact_context_packet` を優先します。

# IBM-Bob SDLC Custom Mode Pack

このディレクトリは、IBM Bob 向けの project custom mode 一式の source of truth です。

Bob に読み込ませる `.bob/custom_modes.yaml` と `.bob/rules-{slug}/` は、ここから生成します。

## 目的
- 基本設計書を起点にした SDLC オーケストレーションを、Bob の custom mode 群として標準化する。
- 対象プロジェクトと入力基本設計書をワーク領域へ複製し、原本を直接変更しない。
- JSON を正本、Markdown を閲覧用として残す。

## ディレクトリ構成
- `modes/`: mode metadata の source of truth
- `rules/`: 各 mode に対応する Bob rule source
- `profiles/`: workspace profile の定義
- `routing/`: 固定ルーティング定義
- `schemas/`: packet schema
- `scripts/`: installer、renderer、validator、evaluation harness

## 生成先
- Bob mode 設定は copied workspace の `.bob/` に生成する
- run artifact は `artifacts/ibm-bob/runs/<run-id>/`
- workspace copy は `artifacts/ibm-bob/workspaces/<run-id>/project/`

## 主要コマンド
```powershell
py -3 ibm-bob/mode-pack/scripts/install_mode_pack.py --workspace <copied-workspace>
py -3 ibm-bob/mode-pack/scripts/validate_mode_pack.py
py -3 ibm-bob/mode-pack/scripts/evaluate_mode_pack.py --case all --execution-mode simulate
py -3 ibm-bob/mode-pack/scripts/evaluate_mode_pack.py --case 1 --execution-mode real
```

## サンプル評価の前提
- `ibm-bob/samples/base/` と `ibm-bob/samples/評価用基本設計書/` は read-only 扱いです。
- `execution-mode simulate` は mode pack の構成評価用です。
- `execution-mode real` は workspace profile の build/test command を実行します。
- sample workspace の real 実行は `clang-cl.exe` の `PATH` 検出、Visual Studio LLVM install path の直接検出、または `Launch-VsDevShell.ps1` を使った bootstrap を前提にします。
- real 実行では toolchain を自動導入しません。

## Bob mode の考え方
- built-in `orchestrator` は override しません。
- IBM-Bob 専用の entry mode として `ibmbob-orchestrator` を追加します。
- 子 mode は `whenToUse` を明示し、Orchestrator に切り替え判断を委譲します。
- review は `pass | revise | block` の 3 値です。
- `block` のみ human checkpoint に進みます。

## 参照した Bob docs
- [Custom modes](https://bob.ibm.com/docs/ide/configuration/custom-modes)
- [Modes](https://bob.ibm.com/docs/ide/features/modes)
