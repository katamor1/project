# v40_10 integration note

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
