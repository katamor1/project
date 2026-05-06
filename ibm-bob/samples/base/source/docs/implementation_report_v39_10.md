<!-- ibm-bob/samples/base/source/docs/implementation_report_v39_10.md -->
<!-- Archives the pre-normalization v39_10 source evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_tool_utility_monitors.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_tool_utility_monitors.c -->
# IBM-Bob 未実装機能追加 実装レポート v39_10

> **Archive note:** This is pre-normalization archive evidence. The current tree uses meaning-based files `inc/nc_feature_backlog_tool_utility_monitors.h`, `src/nc_feature_backlog_tool_utility_monitors.c`, and `tests/test_nc_feature_backlog_tool_utility_monitors.c`. Current verification is manifest-driven via `cmd /c scripts\run_tests.bat`; pinned sample smoke uses `cmd /c scripts\test_clang.bat` or `cmd /c scripts\test_vc2026.bat`.

## 概要

`v38_10` の続きとして、未実装/入口実装止まりだった機械安全・周辺装置READY・保全診断系の機能を10件追加した。
追加IDは `305〜314` とし、既存の `v35〜v38` と同じ固定長・RT安全な独立バックログモジュール方式で実装した。

## 追加機能

| ID | 機能 | 主処理方針 |
|---:|---|---|
| 305 | 主軸ベアリング温度上昇段階減速 | 温度警告をRT周期で監視し、段階減速/高温時HOLDを記録 |
| 306 | リニアスケール汚れ検出トレース | 軸別dirty maskを固定長ステータスへ記録 |
| 307 | 工具クランプ引込確認インターロック | ドローバ確認不成立時に起動禁止/インターロック |
| 308 | ロータリテーブルクランプ確認ゲート | 回転テーブル動作前のクランプ確認ゲート |
| 309 | 切削液フィルタ差圧高HOLD | 差圧高継続時にHOLD要求 |
| 310 | 集塵装置風量低下ゲート | 集塵READY/風量OKをシーケンスゲート化 |
| 311 | 主軸オーバライド変更禁止ゾーン | 危険領域でのoverride変更抑止 |
| 312 | ATC工具番号照合不一致HOLD | expected/actual工具番号不一致でHOLD |
| 313 | ワーク測定プローブREADYタイムアウト | READY待ちタイムアウト時にretry要求 |
| 314 | UPS電圧低下減速停止要求 | UPS低電圧時に減速停止/HOLD相当の安全側要求 |

## 追加ファイル

- `source/inc/nc_feature_backlog_v39.h`
- `source/src/nc_feature_backlog_v39.c`
- `source/tests/test_nc_feature_backlog_v39.c`
- `source/scripts/build_gcc_smoke_v39_10.sh`
- `source/scripts/test_gcc_smoke_v39_10.sh`
- `source/scripts/build_vc2026_v39_10.bat`
- `source/scripts/test_vc2026_v39_10.bat`
- `source/scripts/build_clang_v39_10.bat`
- `source/scripts/test_clang_v39_10.bat`

## RT/TS分離

- TS側: `NcFeatureBacklogV39_ApplyBlockTs()` で固定長ブロックへ hold/interlock/gate/retry/inhibit/trace/slowdown を反映する。
- RT側: `NcFeatureBacklogV39_RunRtTick()` で固定長IOを監視し、閾値/タイムアウト成立時のみ固定長ステータスを更新する。
- RT側ではファイルI/O、Windows API、動的メモリ確保、非固定長ループを使用しない。

## 検証

GCC smoke build/test を実行し、`PASS nc_feature_backlog_v39` を確認した。
VC2026 / clang Windows は実行環境の都合で未実行のため、同梱BATでWindows側確認を行う。
