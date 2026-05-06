<!-- ibm-bob/samples/base/source/docs/integration_note_v37_10.md -->
<!-- Archives the pre-normalization v37_10 source evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_readiness_sensors.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_readiness_sensors.c -->
# v37_10 統合メモ

> **Archive note:** This is pre-normalization archive evidence. The current tree uses meaning-based files `inc/nc_feature_backlog_readiness_sensors.h`, `src/nc_feature_backlog_readiness_sensors.c`, and `tests/test_nc_feature_backlog_readiness_sensors.c`. Current verification is manifest-driven via `cmd /c scripts\run_tests.bat`; pinned sample smoke uses `cmd /c scripts\test_clang.bat` or `cmd /c scripts\test_vc2026.bat`.

## 統合方針

この v37_10 は `v36_5` の続きとして追加する独立モジュールである。
既存の v35/v36 モジュールと同じ方式で、ヘッダ・Cファイル・テストをプロジェクトへ追加する。

## 追加手順

1. `source/inc/nc_feature_backlog_v37.h` を include 配下へ追加する。
2. `source/src/nc_feature_backlog_v37.c` を src 配下へ追加する。
3. ビルド対象に `src\nc_feature_backlog_v37.c` を追加する。
4. 公開API一覧へ以下を追加する。
   - `Api_SetNcFeatureBacklogV37Feature()`
   - `Api_EnableAllNcFeatureBacklogV37Features()`
   - `Api_GetNcFeatureBacklogV37Status()`
   - `Api_RunNcFeatureBacklogV37SelfCheck()`
5. 単体/スモーク確認として `source/tests/test_nc_feature_backlog_v37.c` を追加する。

## Windowsでの確認コマンド

```bat
cd source
scripts\test_vc2026_v37_10.bat
scripts\test_clang_v37_10.bat
```

## Linux/GCCでの確認コマンド

```sh
cd source
scripts/test_gcc_smoke_v37_10.sh
```

## ID管理

- v35_10: `270〜279`
- v36_5: `280〜284`
- v37_10: `285〜294`

次回追加する場合は `295` 以降を使用する。
