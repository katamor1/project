# IBM-Bob 単体テスト追加レポート v44_40

## 概要

`v43_40` の追加単体テストに続き、`nc_feature_backlog_v35` 〜 `nc_feature_backlog_v42` を対象に、別観点の単体テストを40件追加した。

## 追加テストファイル

- `source/tests/test_nc_feature_backlog_additional_v44_40.c`

## 追加テスト件数

| 対象モジュール | 件数 |
|---|---:|
| `nc_feature_backlog_v35` | 5 |
| `nc_feature_backlog_v36` | 5 |
| `nc_feature_backlog_v37` | 5 |
| `nc_feature_backlog_v38` | 5 |
| `nc_feature_backlog_v39` | 5 |
| `nc_feature_backlog_v40` | 5 |
| `nc_feature_backlog_v41` | 5 |
| `nc_feature_backlog_v42` | 5 |
| 合計 | 40 |

## テスト観点

各モジュールに対し、以下の5観点を追加した。

1. `Api_RunNcFeatureBacklogVxxSelfCheck()` の呼び出し可能性と戻り値契約
2. feature ID 境界値: `base - 1`、`last + 1`、末尾ID enable
3. `NcFeatureBacklogVxx_RunRtTick(NULL)` の警告加算と `rt_tick_count` 非加算
4. zero初期化済みRT入力に対する安全パス: `rt_tick_count` のみ加算、hitなし
5. TS適用時の disabled feature 除外と、re-enable後の再適用hit反映

## 実行結果

GCC smoke unit test はPASS。

```text
UNIT_TEST_SUMMARY tests=40 failures=0
PASS additional_unit_tests_v44_40
```

## 補足

- v43_40では初期化、API異常系、enable/disable、NULL TS block、TS block適用の基本観点を確認した。
- v44_40ではRT側NULL/zero入力、境界ID、disabled feature除外と再有効化後の再適用を追加し、状態遷移・境界値・繰返し適用のカバレッジを補強した。
- Windows VC2026 / clang はこの実行環境では未実行。ZIP展開後、同梱BATで確認する。
