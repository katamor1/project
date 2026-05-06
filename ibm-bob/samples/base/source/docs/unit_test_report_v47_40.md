# IBM-Bob 追加単体テスト v47_40 レポート

## 概要

`v46_40` に続く追加単体テストとして、`nc_feature_backlog_v35` 〜 `nc_feature_backlog_v41` を対象に、さらに40件の単体テストを追加した。

今回のテストは、既存の v43〜v46 で確認済みの初期化、NULL、range、enable/disable基本遷移とは重複しないよう、以下の観点を中心にしている。

- sparse enable mask によるTS適用対象の分離
- 最終featureのみ有効化した場合の `last_feature_id` 契約
- TS適用後の `feed_override_percent` 下限/上限維持
- 範囲外ID指定時に状態が変化しないこと
- 全valid feature IDの名称解決
- self-check API横断確認
- feature未有効時のTS副作用なし確認
- first/last feature enable状態の独立性
- trace base順序と不正ID名解決
- `implemented_count` の定数整合性

## 追加ファイル

- `source/tests/test_nc_feature_backlog_additional_v47_40.c`
- `source/scripts/build_gcc_unit_tests_v47_40.sh`
- `source/scripts/test_gcc_unit_tests_v47_40.sh`
- `source/scripts/build_vc2026_unit_tests_v47_40.bat`
- `source/scripts/test_vc2026_unit_tests_v47_40.bat`
- `source/scripts/build_clang_unit_tests_v47_40.bat`
- `source/scripts/test_clang_unit_tests_v47_40.bat`

## テスト件数

| 区分 | 件数 |
|---|---:|
| v35 モジュール個別 | 5 |
| v36 モジュール個別 | 5 |
| v37 モジュール個別 | 5 |
| v38 モジュール個別 | 5 |
| v39 モジュール個別 | 5 |
| v40 モジュール個別 | 5 |
| v41 モジュール個別 | 5 |
| 横断テスト | 5 |
| 合計 | 40 |

## GCC確認結果

```text
UNIT_TEST_SUMMARY tests=40 failures=0
PASS additional_unit_tests_v47_40
```

## Windows確認コマンド

```bat
cd source
scripts\test_vc2026_unit_tests_v47_40.bat
scripts\test_clang_unit_tests_v47_40.bat
```

## 補足

このサンドボックスではGCCでのビルド・実行まで確認済み。VC2026 / clang Windows版は、ZIP展開後に上記BATをWindows環境で実行する。
