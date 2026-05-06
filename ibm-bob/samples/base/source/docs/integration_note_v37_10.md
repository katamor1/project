# v37_10 統合メモ

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
