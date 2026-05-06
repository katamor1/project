# IBM-Bob 追加単体テスト v46_40 実装レポート

## 概要

`v45_40` に続く追加単体テストとして、バックログ実装モジュール `v35〜v41` を対象に、重複しない観点の単体テストを40件追加した。

今回の主眼は、通常系のTS/RT適用確認ではなく、設定APIと状態管理の堅牢性である。

## 追加ファイル

- `source/tests/test_nc_feature_backlog_additional_v46_40.c`
- `source/scripts/build_gcc_unit_tests_v46_40.sh`
- `source/scripts/test_gcc_unit_tests_v46_40.sh`
- `source/scripts/build_vc2026_unit_tests_v46_40.bat`
- `source/scripts/test_vc2026_unit_tests_v46_40.bat`
- `source/scripts/build_clang_unit_tests_v46_40.bat`
- `source/scripts/test_clang_unit_tests_v46_40.bat`

## テスト件数

合計40件。

- `v35`: 6件
- `v36`: 6件
- `v37`: 6件
- `v38`: 6件
- `v39`: 6件
- `v40`: 5件
- `v41`: 5件

## 追加観点

1. `Api_Set...Feature()` の二重enable/disableが `configured_count` を壊さないこと
2. `Api_Get...Status(NULL)` が `ERR_NULL` を返すこと
3. TS/RT実行後に `Init()` するとカウンタ・マスク・ヒット数が初期化されること
4. 全機能enable後、逆順disableしても `enabled_mask` と `configured_count` が0へ戻ること
5. feature ID の有効範囲外、下限未満/上限超過で `unknown` を返すこと
6. 部分disable後のTS適用で、無効化された先頭/末尾featureのhitが記録されないこと

## GCC確認結果

```text
UNIT_TEST_SUMMARY tests=40 failures=0
PASS additional_unit_tests_v46_40
```

## Windows確認方法

Visual Studio Developer Command Prompt または clang が通ったPowerShellで以下を実行する。

```bat
cd source
scripts\test_vc2026_unit_tests_v46_40.bat
scripts\test_clang_unit_tests_v46_40.bat
```

## 補足

このLinuxサンドボックスでは GCC によるビルド/実行まで確認済み。VC2026 / clang Windows の実行確認はWindows環境側で行う。
