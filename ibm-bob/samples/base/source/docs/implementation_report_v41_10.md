<!-- ibm-bob/samples/base/source/docs/implementation_report_v41_10.md -->
<!-- Archives the pre-normalization v41_10 source evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_service_interlocks.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_service_interlocks.c -->
# IBM-Bob source implementation report v41_10

> **Archive note:** This is pre-normalization archive evidence. The current tree uses meaning-based files `inc/nc_feature_backlog_service_interlocks.h`, `src/nc_feature_backlog_service_interlocks.c`, and `tests/test_nc_feature_backlog_service_interlocks.c`. Current verification is manifest-driven via `cmd /c scripts\run_tests.bat`; pinned sample smoke uses `cmd /c scripts\test_clang.bat` or `cmd /c scripts\test_vc2026.bat`.

## 概要

v40_10 の続きとして、未実装/入口実装止まりだった安全・保全・周辺装置READY・診断系機能を ID 325〜334 の10件として追加した。RT側は固定長状態、固定長ループ、動的メモリ確保なし、ファイルI/Oなし、Windows APIなしの方針を維持する。

## 追加機能

325. サーボ位置偏差過大段階減速
326. 主軸負荷急上昇HOLD
327. クーラント流量センサ断線トレース
328. 治具クランプ圧低下インターロック
329. ロボット扉閉確認ゲート
330. 切削室排気ファンREADYゲート
331. 工具洗浄ノズル詰まりリトライ
332. 軸原点センサ二重化不一致トレース
333. NCオプションライセンス不一致実行禁止
334. 保守モードキー抜けHOLD

## 追加ファイル

- `source/inc/nc_feature_backlog_v41.h`
- `source/src/nc_feature_backlog_v41.c`
- `source/tests/test_nc_feature_backlog_v41.c`
- `source/scripts/build_gcc_smoke_v41_10.sh`
- `source/scripts/test_gcc_smoke_v41_10.sh`
- `source/scripts/build_vc2026_v41_10.bat`
- `source/scripts/test_vc2026_v41_10.bat`
- `source/scripts/build_clang_v41_10.bat`
- `source/scripts/test_clang_v41_10.bat`

## 実装方針

- TS側 `NcFeatureBacklogV41_ApplyBlockTs()` では、NC固定長ブロックに対し HOLD、INTERLOCK、GATE、INHIBIT、RETRY、TRACE、SLOWDOWN を固定長ビットマスクとして反映する。
- RT側 `NcFeatureBacklogV41_RunRtTick()` では、センサ入力を周期ごとに監視し、チャタリング/瞬断を避けるため短い固定tickカウンタでタイムアウト判定する。
- サーボ位置偏差・主軸負荷・治具クランプ圧・工具洗浄流量・ライセンス不足などを状態スナップショットで確認可能にする。
- `Api_GetNcFeatureBacklogV41Status()` により、TS/RT側での検出結果を固定長ステータスとして取得できる。

## 検証

GCC smoke build/test を実行し、`PASS nc_feature_backlog_v41` を確認した。
