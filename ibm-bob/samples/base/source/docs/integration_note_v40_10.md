<!-- ibm-bob/samples/base/source/docs/integration_note_v40_10.md -->
<!-- Archives the pre-normalization v40_10 source evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_process_safety.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_process_safety.c -->
# v40_10 integration note

> **Archive note:** This is pre-normalization archive evidence. The current tree uses meaning-based files `inc/nc_feature_backlog_process_safety.h`, `src/nc_feature_backlog_process_safety.c`, and `tests/test_nc_feature_backlog_process_safety.c`. Current verification is manifest-driven via `cmd /c scripts\run_tests.bat`; pinned sample smoke uses `cmd /c scripts\test_clang.bat` or `cmd /c scripts\test_vc2026.bat`.

`v39_10` までの追加モジュール群に対して、`nc_feature_backlog_v40` を追加する。

## 統合手順

1. `source/inc/nc_feature_backlog_v40.h` を公開ヘッダ候補として追加する。
2. `source/src/nc_feature_backlog_v40.c` をビルド対象に追加する。
3. 初期化シーケンスで `NcFeatureBacklogV40_Init()` を呼ぶ。
4. 機種別に必要な機能だけを `Api_SetNcFeatureBacklogV40Feature()` または `Api_EnableAllNcFeatureBacklogV40Features()` で有効化する。
5. TS側のNC固定長ブロック生成後に `NcFeatureBacklogV40_ApplyBlockTs()` を適用する。
6. RT周期処理で `NcFeatureBacklogV40_RunRtTick()` に固定長I/Oスナップショットを渡す。

## Windows確認

```bat
cd source
scripts\test_vc2026_v40_10.bat
scripts\test_clang_v40_10.bat
```

Linux/GCC確認は `scripts/test_gcc_smoke_v40_10.sh` で実行済み。
