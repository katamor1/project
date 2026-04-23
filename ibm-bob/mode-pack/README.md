<!-- ibm-bob/mode-pack/README.md -->
<!-- Explains the IBM-Bob SDLC custom mode pack, its generated Bob assets, and the local validation scripts. -->
<!-- This exists so the team can install, validate, and evaluate the mode pack without editing the sample originals. -->
<!-- RELEVANT FILES: ibm-bob/mode-pack/modes/custom_modes.source.yaml, ibm-bob/mode-pack/scripts/install_mode_pack.py, ibm-bob/mode-pack/scripts/evaluate_mode_pack.py -->
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
