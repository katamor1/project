# IBM-Bob ソースコード追加実装レポート v37_10

## 概要

`v36_5` の続きとして、未実装または入口実装止まりだった安全・保全・運転準備系の機能を 10 件追加した。
追加IDは `285〜294` とし、既存の v35/v36 と重複しない独立モジュール `nc_feature_backlog_v37` として実装した。

## 追加した機能

| ID | 機能名 | 主な処理方針 |
|---:|---|---|
| 285 | 主軸チラー流量低下段階減速 | チラーON中の流量低下または主軸温度上昇時に送りオーバライドを段階低下し、閾値超過ではHOLD側へ寄せる |
| 286 | 軸サーボファン停止HOLD | 監視対象軸のサーボREADY中にファン異常を検出し、軸マスク付きHOLD要求を記録 |
| 287 | ドアロック二重系不一致トレース | ドアロック指令中の2ch信号不一致を固定長トレースコードとして保存 |
| 288 | 工具マガジン原点復帰タイムアウトインターロック | 原点復帰要求中に原点センサが成立しない場合、インターロックとして記録 |
| 289 | 油圧昇圧READYゲート | READY要求時にポンプ/圧力条件が不足する場合、シーケンスゲートで待機 |
| 290 | ワーク着座エア確認リトライゲート | エアブローまたは着座確認が不足する場合、リトライ要求を記録 |
| 291 | 切粉洗浄圧低下HOLD | 切粉洗浄ポンプON中の圧力低下をHOLD条件として検出 |
| 292 | 操作盤イネーブルスイッチガード | 操作盤有効中にイネーブルスイッチ未成立の場合、インターロックとして扱う |
| 293 | プローブ電池低下時測定禁止 | 測定要求中のプローブ電池低下を測定禁止・ゲート条件として扱う |
| 294 | 絶対値エンコーダ電池警告トレース | 軸別電池低下マスクをトレースし、最後の軸マスクとして公開 |

## 実装ファイル

- `source/inc/nc_feature_backlog_v37.h`
- `source/src/nc_feature_backlog_v37.c`
- `source/tests/test_nc_feature_backlog_v37.c`
- `source/scripts/build_gcc_smoke_v37_10.sh`
- `source/scripts/test_gcc_smoke_v37_10.sh`
- `source/scripts/build_vc2026_v37_10.bat`
- `source/scripts/test_vc2026_v37_10.bat`
- `source/scripts/build_clang_v37_10.bat`
- `source/scripts/test_clang_v37_10.bat`

## 設計上の制約対応

- RT側処理は `NcFeatureBacklogV37_RunRtTick()` に閉じ、固定長カウンタと固定長ステータスのみ使用する。
- TS側処理は `NcFeatureBacklogV37_ApplyBlockTs()` に分離し、ブロック単位で `hold_request`、`interlock_mask`、`sequence_gate_mask`、`retry_request_mask`、`inhibit_mask`、`trace_event_code` を設定する。
- 動的メモリ確保、ファイルI/O、Windows API、非決定長ループは使用していない。
- 機能有効/無効は `enabled_mask` で管理し、API経由で個別設定できる。
- `Api_RunNcFeatureBacklogV37SelfCheck()` により、追加10機能の主要カウンタがすべて動作することを確認できる。

## 検証結果

GCC smoke build/test を実行し、PASSを確認した。
詳細は `source/docs/verification_output_v37_10.txt` を参照。
