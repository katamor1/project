# v41_10 integration note

`v40_10` までの追加モジュール群に対して、`nc_feature_backlog_v41` を追加する。

## 統合手順

1. `source/inc/nc_feature_backlog_v41.h` を公開ヘッダ候補として追加する。
2. `source/src/nc_feature_backlog_v41.c` をビルド対象に追加する。
3. 初期化シーケンスで `NcFeatureBacklogV41_Init()` を呼ぶ。
4. 機種別に必要な機能だけを `Api_SetNcFeatureBacklogV41Feature()` または `Api_EnableAllNcFeatureBacklogV41Features()` で有効化する。
5. TS側のNC固定長ブロック生成後に `NcFeatureBacklogV41_ApplyBlockTs()` を適用する。
6. RT周期処理で `NcFeatureBacklogV41_RunRtTick()` に固定長I/Oスナップショットを渡す。

## Windows確認

```bat
cd source
scripts\test_vc2026_v41_10.bat
scripts\test_clang_v41_10.bat
```

Linux/GCC確認は `scripts/test_gcc_smoke_v41_10.sh` で実行済み。
