<!-- ibm-bob/samples/base/source/README.md -->
<!-- Describes the IBM BOB baseline source workspace and the pinned build and test entrypoints. -->
<!-- This exists so evaluators can orient themselves quickly before inspecting source files or running scripts. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/AGENTS.md, ibm-bob/samples/base/source/.claude/config.toml, ibm-bob/samples/base/source/docs/workspace_profile_commands.md -->
# IBM BOB 基盤システムソースコード一式

この一式は、評価用基本設計書を入力したカスタムエージェントが「既存部分への影響調査」を行うためのベースラインコードです。

## 目的
- リアルタイム系とタイムシェア系の責務分割を示す
- 共有メモリ構造体と公開 API を示す
- 状態遷移、インターロック、ログ、先読み補正の既存実装位置を示す

## ディレクトリ構成
- `include/`: 公開ヘッダ、内部ヘッダ
- `src/`: 実装

## モジュール要点
- `system_shared.*`: 共有構造体とグローバル変数
- `control_api.*`: 外部アプリ向け API
- `rt_control.*`: リアルタイム周期処理
- `ts_service.*`: タイムシェア処理
- `main.c`: 評価用の簡易シミュレーション

## 注意
- 実機依存の割込制御や共有メモリマッピングは簡略化しています。
- 本コードは影響調査・レビュー評価用であり、製品コードそのものではありません。


## Build / test commands for Claude workspace profiles
- VC2026 build: `cmd /c scripts\build_vc2026.bat`
- VC2026 test: `cmd /c scripts\test_vc2026.bat`
- clang-win build: `cmd /c scripts\build_clang.bat`
- clang-win test: `cmd /c scripts\test_clang.bat`

The clang profile can use `clang-cl.exe` from `PATH`, from the Visual Studio LLVM install location, or after `Launch-VsDevShell.ps1`.

See also:
- `AGENTS.md`
- `.claude/config.toml`
- `docs/workspace_profile_commands.md`
