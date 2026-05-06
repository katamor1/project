<!-- ibm-bob/samples/base/source/docs/integration_note_v38_10.md -->
<!-- Archives the pre-normalization v38_10 source evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_pallet_fixture_safety.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_pallet_fixture_safety.c -->
# v38_10 統合メモ

> **Archive note:** This is pre-normalization archive evidence. The current tree uses meaning-based files `inc/nc_feature_backlog_pallet_fixture_safety.h`, `src/nc_feature_backlog_pallet_fixture_safety.c`, and `tests/test_nc_feature_backlog_pallet_fixture_safety.c`. Current verification is manifest-driven via `cmd /c scripts\run_tests.bat`; pinned sample smoke uses `cmd /c scripts\test_clang.bat` or `cmd /c scripts\test_vc2026.bat`.

## 統合方針

この版は `v37_10` からの差分追加として、`nc_feature_backlog_v38` モジュールを追加する。
既存モジュールに直接依存しないため、まずは単独スモークテストで検証し、その後に統合ビルド対象へ追加する。

## 追加するビルド対象

- `src/nc_feature_backlog_v38.c`
- `tests/test_nc_feature_backlog_v38.c`

## 公開ヘッダ

- `inc/nc_feature_backlog_v38.h`

## 既存ソースへ統合する場合の候補

1. `system_shared.h` に `NC_FEATURE_BACKLOG_V38_STATUS` の共有メモリ公開方針を追加する。
2. `control_api.h` に v38 API の公開宣言を追加する。
3. `rt_control.c` のRT周期処理末尾で `NcFeatureBacklogV38_RunRtTick()` を呼ぶ。
4. `ts_service.c` のNC固定ブロック生成後に `NcFeatureBacklogV38_ApplyBlockTs()` を呼ぶ。
5. `build_vc2026.bat` / `build_clang.bat` のコンパイル対象へ `src\nc_feature_backlog_v38.c` を追加する。

## 注意事項

- 本モジュールはRT安全性を優先し、設備パラメータの永続化やログファイル出力は含めていない。
- 実機適用時は、各入力信号のチャタリング除去、二重化信号の診断周期、HOLDとALARMの最終遷移先を機械仕様に合わせて確定する。
