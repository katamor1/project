# v35_10 統合メモ

このZIPは、v34_10の実体ZIPが実行環境に存在しない状態でも確認できるよう、追加10機能を独立モジュールとして同梱している。
既存v34_10ツリーへ統合する場合は、以下を行う。

1. `source/inc/nc_feature_backlog_v35.h` を include 配下へ追加
2. `source/src/nc_feature_backlog_v35.c` を src 配下へ追加
3. 既存の公開APIヘッダ、または `control_api.h` から必要に応じて `nc_feature_backlog_v35.h` をinclude
4. `rt_control.c` のRT周期診断処理で `NcFeatureBacklogV35_RunRtTick()` を呼び出す
5. `ts_service.c` の固定長NCブロック生成後に `NcFeatureBacklogV35_ApplyBlockTs()` を呼び出す
6. 既存ビルドスクリプトのコンパイル対象に `src\nc_feature_backlog_v35.c` を追加
7. v34_10側の一元管理定数がある場合は、`NC_IMPL_BACKLOG_FEATURE_COUNT` を `280` に更新する

`NC_IMPL_BACKLOG_WORD_COUNT` は、280件時点でも `ceil(280 / 32) = 9` のため変更不要。
