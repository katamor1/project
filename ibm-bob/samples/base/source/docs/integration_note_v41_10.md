<!-- ibm-bob/samples/base/source/docs/integration_note_v41_10.md -->
<!-- Archives the pre-normalization v41_10 source evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_service_interlocks.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_service_interlocks.c -->
# v41_10 integration note

> **Archive note:** This is pre-normalization archive evidence. The current tree uses meaning-based files `inc/nc_feature_backlog_service_interlocks.h`, `src/nc_feature_backlog_service_interlocks.c`, and `tests/test_nc_feature_backlog_service_interlocks.c`. Current verification is manifest-driven via `cmd /c scripts\run_tests.bat`; pinned sample smoke uses `cmd /c scripts\test_clang.bat` or `cmd /c scripts\test_vc2026.bat`.

`v40_10` までの追加モジュール群に対して、`nc_feature_backlog_v41` を追加する。

## 統合手順

1. `source/inc/nc_feature_backlog_v41.h` を公開ヘッダ候補として追加する。
2. `source/src/nc_feature_backlog_v41.c` をビルド対象に追加する。
3. 初期化シーケンスで `NcFeatureBacklogV41_Init()` を呼ぶ。
4. 機種別に必要な機能だけを `Api_SetNcFeatureBacklogV41Feature()` または `Api_EnableAllNcFeatureBacklogV41Features()` で有効化する。
5. TS側のNC固定長ブロック生成後に `NcFeatureBacklogV41_ApplyBlockTs()` を適用する。
6. RT周期処理で `NcFeatureBacklogV41_RunRtTick()` に固定長I/Oスナップショットを渡す。

## Windows確認

```bat
cd source
scripts\test_vc2026_v41_10.bat
scripts\test_clang_v41_10.bat
```

Linux/GCC確認は `scripts/test_gcc_smoke_v41_10.sh` で実行済み。
