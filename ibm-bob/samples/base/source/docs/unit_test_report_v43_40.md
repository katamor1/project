# IBM-Bob 追加単体テスト v43_40 実装レポート

## 概要

`v42_10` をベースに、追加バックログ機能モジュール `v35`〜`v42` を横断する単体テストを40件追加した。

## 追加ファイル

- `source/tests/test_nc_feature_backlog_additional_40.c`
- `source/scripts/build_gcc_unit_tests_v43_40.sh`
- `source/scripts/test_gcc_unit_tests_v43_40.sh`
- `source/scripts/build_vc2026_unit_tests_v43_40.bat`
- `source/scripts/test_vc2026_unit_tests_v43_40.bat`
- `source/scripts/build_clang_unit_tests_v43_40.bat`
- `source/scripts/test_clang_unit_tests_v43_40.bat`
- `source/docs/verification_output_unit_tests_v43_40.txt`
- `source/docs/unit_test_report_v43_40.md`
- `source/docs/manifest_unit_tests_v43_40.json`

## テスト件数

合計40件。`v35`〜`v42` の8モジュールに対して、各5件ずつ追加した。

| No | 観点 | 内容 |
|---:|---|---|
| 1 | 初期化/状態既定値 | `Init` 後の実装件数、設定件数、マスク、カウンタ、初期feedを確認 |
| 2 | API異常系/名称取得 | NULL status、範囲外feature ID、未知feature名の扱いを確認 |
| 3 | enable/disable | 1機能ON/OFFと `configured_count`、全機能enableを確認 |
| 4 | NULL block | TS適用APIにNULLを渡した場合のwarning加算と副作用抑止を確認 |
| 5 | TS block効果 | 全機能enable時のTS固定長ブロック更新、カテゴリ、hit記録、feed制限を確認 |

## 検証結果

GCC smoke build/test はPASS。

```text
[TEST 01] v35 init/status defaults
[TEST 02] v35 API validation and unknown feature name
[TEST 03] v35 enable/disable configured_count
[TEST 04] v35 NULL TS block warning
[TEST 05] v35 TS block effects and per-feature hits
[TEST 06] v36 init/status defaults
[TEST 07] v36 API validation and unknown feature name
[TEST 08] v36 enable/disable configured_count
[TEST 09] v36 NULL TS block warning
[TEST 10] v36 TS block effects and per-feature hits
[TEST 11] v37 init/status defaults
[TEST 12] v37 API validation and unknown feature name
[TEST 13] v37 enable/disable configured_count
[TEST 14] v37 NULL TS block warning
[TEST 15] v37 TS block effects and per-feature hits
[TEST 16] v38 init/status defaults
[TEST 17] v38 API validation and unknown feature name
[TEST 18] v38 enable/disable configured_count
[TEST 19] v38 NULL TS block warning
[TEST 20] v38 TS block effects and per-feature hits
[TEST 21] v39 init/status defaults
[TEST 22] v39 API validation and unknown feature name
[TEST 23] v39 enable/disable configured_count
[TEST 24] v39 NULL TS block warning
[TEST 25] v39 TS block effects and per-feature hits
[TEST 26] v40 init/status defaults
[TEST 27] v40 API validation and unknown feature name
[TEST 28] v40 enable/disable configured_count
[TEST 29] v40 NULL TS block warning
[TEST 30] v40 TS block effects and per-feature hits
[TEST 31] v41 init/status defaults
[TEST 32] v41 API validation and unknown feature name
[TEST 33] v41 enable/disable configured_count
[TEST 34] v41 NULL TS block warning
[TEST 35] v41 TS block effects and per-feature hits
[TEST 36] v42 init/status defaults
[TEST 37] v42 API validation and unknown feature name
[TEST 38] v42 enable/disable configured_count
[TEST 39] v42 NULL TS block warning
[TEST 40] v42 TS block effects and per-feature hits
UNIT_TEST_SUMMARY tests=40 failures=0
PASS additional_unit_tests_v43_40
```

## Windows確認コマンド

```bat
cd source
scripts	est_vc2026_unit_tests_v43_40.bat
scripts	est_clang_unit_tests_v43_40.bat
```
