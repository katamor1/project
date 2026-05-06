<!-- ibm-bob/samples/base/source/docs/integration_note_v39_10.md -->
<!-- Archives the pre-normalization v39_10 source evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_tool_utility_monitors.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_tool_utility_monitors.c -->
# v39_10 統合メモ

> **Archive note:** This is pre-normalization archive evidence. The current tree uses meaning-based files `inc/nc_feature_backlog_tool_utility_monitors.h`, `src/nc_feature_backlog_tool_utility_monitors.c`, and `tests/test_nc_feature_backlog_tool_utility_monitors.c`. Current verification is manifest-driven via `cmd /c scripts\run_tests.bat`; pinned sample smoke uses `cmd /c scripts\test_clang.bat` or `cmd /c scripts\test_vc2026.bat`.

## 統合方針

`v38_10` をベースにする場合、以下を追加する。

1. `source/inc/nc_feature_backlog_v39.h` をインクルード対象に追加
2. `source/src/nc_feature_backlog_v39.c` をビルド対象に追加
3. 起動初期化で `NcFeatureBacklogV39_Init()` を呼び出す
4. 対象機能を有効化する場合は `Api_EnableAllNcFeatureBacklogV39Features()` または `Api_SetNcFeatureBacklogV39Feature()` を使用
5. TS側ブロック確定前に `NcFeatureBacklogV39_ApplyBlockTs()` を呼び出す
6. RT周期処理内で固定長IOを渡して `NcFeatureBacklogV39_RunRtTick()` を呼び出す
7. UI/診断側は `Api_GetNcFeatureBacklogV39Status()` で状態を取得する

## Windows確認

```bat
cd source
scripts	est_vc2026_v39_10.bat
scripts	est_clang_v39_10.bat
```

## 注意点

本モジュールはバックログ機能の実装入口であり、実機連携時はセンサビット割付、警告閾値、設備別タイムアウト、HOLD/ALARMへの最終マッピングを詳細設計側で確定する。
