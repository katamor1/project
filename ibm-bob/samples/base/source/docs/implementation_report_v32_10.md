# IBM-Bob source implementation report v32_10

## 概要

直近の `v31_10` ソースをベースに、未実装/入口実装止まりだった設計機能を10件追加した。
今回の追加では、`nc_feature_backlog` の固定長バックログ定義を 240 件から 250 件へ拡張し、追加ID `240`〜`249` を実装対象とした。

## 変更ファイル

- `source/inc/system_config.h`
  - `NC_IMPL_BACKLOG_FEATURE_COUNT` を `240U` から `250U` に変更
  - v29〜v32 の追加バッチ定義を追加
- `source/inc/system_shared.h`
  - バックログ状態コメントを v32 / 250件に更新
- `source/src/nc_feature_backlog.c`
  - `s_backlog[]` に ID `240`〜`249` の10件を追加
  - `ApplyV23Policy()` に追加10件のTS側固定長ブロック補正を追加
  - コメントを v32 対応に更新
- `source/scripts/build_gcc_smoke_v32_10.sh`
  - v32_10 用GCCスモークビルドスクリプトを追加
- `source/scripts/test_gcc_smoke_v32_10.sh`
  - v32_10 用GCCスモークテストスクリプトを追加

## 追加実装した10機能

| ID | 機能名 | TS側補正概要 | RT側扱い |
|---:|---|---|---|
| 240 | 工具クーラント遅延停止監視 | MFIN待ち、M78相当、遅延監視用Pワード、状態ラッチを設定 | イベント/カウントのみ |
| 241 | 主軸負荷積分しきい値減速 | 主軸速度フォールバック、送り低下、減速tick増加 | イベント/カウントのみ |
| 242 | APC扉閉完了二重確認 | MFIN待ち、M79相当、扉閉確認ラッチ、送り低下 | イベント/カウントのみ |
| 243 | 治具着座センサ不一致ガード | 経路軸マスク固定、退避フラグ、HOLD寄り減速 | イベント/カウントのみ |
| 244 | 回転軸クランプ解除待ちMコード監視 | A/B軸有効化、M80相当、回転軸最短経路正規化 | イベント/カウントのみ |
| 245 | バックグラウンド編集中NC開始禁止ラッチ | 開始禁止ラッチ、再開識別Pワード、送り低下 | イベント/カウントのみ |
| 246 | プログラム番号/サブ番号範囲ガード | Pワードを1〜9999へクランプ、行番号フォールバック | イベント/カウントのみ |
| 247 | 先読み中工具補正未登録ブロック停止 | H/D未設定時の安全フォールバック、工具長/径補正フラグ | イベント/カウントのみ |
| 248 | 送り軸ブレーキ解除確認 | MFIN待ち、M81相当、XYZ軸対象、加速tick増加 | イベント/カウントのみ |
| 249 | 非常停止入力チャタリング診断 | 非常停止診断ラッチ、強制最小送り、減速tick増加 | イベント/カウントのみ |

## RT/TS分担

- TS側: `NcFeatureBacklog_ApplyBlockTs()` から `ApplyV23Policy()` を通じて、固定長 `NC_EXEC_BLOCK` のみを補正する。
- RT側: `NcFeatureBacklog_OnBlockRt()` では動的メモリ確保、ファイルI/O、Windows API呼び出しを行わず、実行カウントと最終状態の更新に限定する。

## 検証

GCCスモークビルド/テストを実行し、以下を確認した。

- `implemented=250`
- `configured=250`
- `synthetic=250`
- `last_id=249`
- `nc_error=0`
- 既存の `coord_error_checks` / `g08_error_checks` / `hpcc_error_checks` / `cycle_error_checks` が出力されること

詳細は `source/docs/verification_output_v32_10.txt` を参照。
