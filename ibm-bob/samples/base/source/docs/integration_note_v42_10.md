<!-- ibm-bob/samples/base/source/docs/integration_note_v42_10.md -->
<!-- Archives the pre-normalization v42_10 source evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_loader_environment.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_loader_environment.c -->
# Integration Note v42_10

> **Archive note:** This is pre-normalization archive evidence. The current tree uses meaning-based files `inc/nc_feature_backlog_loader_environment.h`, `src/nc_feature_backlog_loader_environment.c`, and `tests/test_nc_feature_backlog_loader_environment.c`. Current verification is manifest-driven via `cmd /c scripts\run_tests.bat`; pinned sample smoke uses `cmd /c scripts\test_clang.bat` or `cmd /c scripts\test_vc2026.bat`.

`v42_10` は `v41_10` の後続追加モジュールである。既存本体へ統合する場合は、以下を追加する。

1. `source/inc/nc_feature_backlog_v42.h` を公開ヘッダ群へ追加する。
2. `source/src/nc_feature_backlog_v42.c` をビルド対象へ追加する。
3. システム初期化時に `NcFeatureBacklogV42_Init()` を呼ぶ。
4. 設備構成または評価用設定で `Api_EnableAllNcFeatureBacklogV42Features()` または個別 `Api_SetNcFeatureBacklogV42Feature()` を呼ぶ。
5. TS側の固定ブロック確定処理から `NcFeatureBacklogV42_ApplyBlockTs()` を呼ぶ。
6. RT周期処理から `NcFeatureBacklogV42_RunRtTick()` を呼ぶ。
7. 状態表示・診断ログから `Api_GetNcFeatureBacklogV42Status()` を参照する。

RT側実装は固定長カウンタと整数演算のみで、ファイルI/O、動的メモリ、Windows API呼び出しは行わない。
