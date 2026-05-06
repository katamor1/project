# IBM-Bob 単体テスト追加レポート v45_40

## 概要

`ibm_bob_source_implemented_additional_features_v41_10` をベースに、追加バックログ機能 `v35〜v41` を対象とする単体テストを40件追加した。

## 追加ファイル

- `source/tests/test_nc_feature_backlog_additional_v45_40.c`
- `source/scripts/build_gcc_unit_tests_v45_40.sh`
- `source/scripts/test_gcc_unit_tests_v45_40.sh`
- `source/scripts/build_vc2026_unit_tests_v45_40.bat`
- `source/scripts/test_vc2026_unit_tests_v45_40.bat`
- `source/scripts/build_clang_unit_tests_v45_40.bat`
- `source/scripts/test_clang_unit_tests_v45_40.bat`

## 追加テスト観点

今回の40件では、既存の各モジュール別スモークテストと重複しすぎないよう、以下を重点確認した。

1. 状態スナップショット取得後にAPIで状態を変更しても、取得済みスナップショットが独立していること
2. 非連続feature IDを有効化/無効化した場合の `enabled_mask` と `configured_count` の整合性
3. TS固定長ブロックを複数回適用した場合の `ts_apply_count` と `per_feature_hits[]` の累積
4. zero初期化RT入力を複数tick実行しても、tickカウントとwarningカウントがAPI契約どおりであること
5. 各モジュールの全valid feature IDが `unknown` ではない名称へ解決されること
6. feature未有効状態でTS固定長ブロックを適用しても、hitや副作用マスクを記録しないこと

## テスト件数

| 対象モジュール | 追加件数 |
|---|---:|
| `nc_feature_backlog_v35` | 6 |
| `nc_feature_backlog_v36` | 6 |
| `nc_feature_backlog_v37` | 6 |
| `nc_feature_backlog_v38` | 6 |
| `nc_feature_backlog_v39` | 6 |
| `nc_feature_backlog_v40` | 5 |
| `nc_feature_backlog_v41` | 5 |
| **合計** | **40** |

## 検証結果

GCCスモーク単体テストを実行し、40件すべてPASSした。

```text
UNIT_TEST_SUMMARY tests=40 failures=0
PASS additional_unit_tests_v45_40
```

## Windows環境での確認コマンド

```bat
cd source
scripts\test_vc2026_unit_tests_v45_40.bat
scripts\test_clang_unit_tests_v45_40.bat
```

## 補足

このLinuxサンドボックスではVC2026 / clang Windows版の実行は未実施。Windows側では上記BATを使用して確認する。
