# Integration Note v42_10

`v42_10` は `v41_10` の後続追加モジュールである。既存本体へ統合する場合は、以下を追加する。

1. `source/inc/nc_feature_backlog_v42.h` を公開ヘッダ群へ追加する。
2. `source/src/nc_feature_backlog_v42.c` をビルド対象へ追加する。
3. システム初期化時に `NcFeatureBacklogV42_Init()` を呼ぶ。
4. 設備構成または評価用設定で `Api_EnableAllNcFeatureBacklogV42Features()` または個別 `Api_SetNcFeatureBacklogV42Feature()` を呼ぶ。
5. TS側の固定ブロック確定処理から `NcFeatureBacklogV42_ApplyBlockTs()` を呼ぶ。
6. RT周期処理から `NcFeatureBacklogV42_RunRtTick()` を呼ぶ。
7. 状態表示・診断ログから `Api_GetNcFeatureBacklogV42Status()` を参照する。

RT側実装は固定長カウンタと整数演算のみで、ファイルI/O、動的メモリ、Windows API呼び出しは行わない。
