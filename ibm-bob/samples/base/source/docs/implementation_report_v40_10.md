<!-- ibm-bob/samples/base/source/docs/implementation_report_v40_10.md -->
<!-- Archives the pre-normalization v40_10 source evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_process_safety.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_process_safety.c -->
# IBM-Bob source implementation report v40_10

> **Archive note:** This is pre-normalization archive evidence. The current tree uses meaning-based files `inc/nc_feature_backlog_process_safety.h`, `src/nc_feature_backlog_process_safety.c`, and `tests/test_nc_feature_backlog_process_safety.c`. Current verification is manifest-driven via `cmd /c scripts\run_tests.bat`; pinned sample smoke uses `cmd /c scripts\test_clang.bat` or `cmd /c scripts\test_vc2026.bat`.

## 概要

v39_10 の続きとして、未実装/入口実装止まりだった安全・保全・周辺装置READY系の機能を ID 315〜324 の10件として追加した。RT側は固定長状態、固定長ループ、動的メモリ確保なし、ファイルI/Oなし、Windows APIなしの方針を維持する。

## 追加機能

315. レーザエリアスキャナ侵入HOLD
316. 主軸振動上昇トレース
317. 送り軸潤滑ポンプREADYゲート
318. NCプログラム署名不一致実行禁止
319. チャック開閉センサ二重化不一致インターロック
320. 工具折損検出後自動退避要求
321. 工具長測定値急変HOLD
322. クーラント温度高段階減速
323. パレット搬送ロボットREADYタイムアウト
324. 安全PLCハートビート喪失即時HOLD

## 追加ファイル

- `source/inc/nc_feature_backlog_v40.h`
- `source/src/nc_feature_backlog_v40.c`
- `source/tests/test_nc_feature_backlog_v40.c`
- `source/scripts/build_gcc_smoke_v40_10.sh`
- `source/scripts/test_gcc_smoke_v40_10.sh`
- `source/scripts/build_vc2026_v40_10.bat`
- `source/scripts/test_vc2026_v40_10.bat`
- `source/scripts/build_clang_v40_10.bat`
- `source/scripts/test_clang_v40_10.bat`

## 実装方針

- TS側 `NcFeatureBacklogV40_ApplyBlockTs()` では、NC固定長ブロックに対し HOLD、INTERLOCK、GATE、INHIBIT、RETRACT、RETRY、TRACE、SLOWDOWN を固定長ビットマスクとして反映する。
- RT側 `NcFeatureBacklogV40_RunRtTick()` では、センサ入力を周期ごとに監視し、チャタリング/瞬断を避けるため短い固定tickカウンタでタイムアウト判定する。
- 安全PLCハートビートはカウンタ停止を検出し、一定tick継続時にHOLDとして状態公開する。
- 工具長測定値急変、主軸振動、クーラント温度は閾値超過を診断値として保持し、状態スナップショットで確認可能にする。

## 検証

GCC smoke build/test を実行し、`PASS nc_feature_backlog_v40` を確認した。
