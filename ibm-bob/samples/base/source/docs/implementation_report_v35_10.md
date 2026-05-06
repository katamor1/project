<!-- ibm-bob/samples/base/source/docs/implementation_report_v35_10.md -->
<!-- Archives the pre-normalization v35_10 source evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_aux_safety.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_aux_safety.c -->
# IBM-Bob 未実装機能追加 v35_10 実装レポート

> **Archive note:** This is pre-normalization archive evidence. The current tree uses meaning-based files `inc/nc_feature_backlog_aux_safety.h`, `src/nc_feature_backlog_aux_safety.c`, and `tests/test_nc_feature_backlog_aux_safety.c`. Current verification is manifest-driven via `cmd /c scripts\run_tests.bat`; pinned sample smoke uses `cmd /c scripts\test_clang.bat` or `cmd /c scripts\test_vc2026.bat`.

## 目的

直近の v34_10 で `NC_IMPL_BACKLOG_FEATURE_COUNT` が 270、追加IDが `260〜269` まで到達していたため、今回は重複しない `270〜279` の10機能を追加した。

## 追加モジュール

- `source/inc/nc_feature_backlog_v35.h`
- `source/src/nc_feature_backlog_v35.c`
- `source/tests/test_nc_feature_backlog_v35.c`

## 追加API

- `NcFeatureBacklogV35_Init()`
- `NcFeatureBacklogV35_IsFeatureEnabled()`
- `Api_SetNcFeatureBacklogV35Feature()`
- `Api_EnableAllNcFeatureBacklogV35Features()`
- `Api_GetNcFeatureBacklogV35Status()`
- `Api_RunNcFeatureBacklogV35SelfCheck()`
- `NcFeatureBacklogV35_ApplyBlockTs()`
- `NcFeatureBacklogV35_RunRtTick()`
- `NcFeatureBacklogV35_GetFeatureName()`

## 追加共有メモリ相当ステータス

- `NC_FEATURE_BACKLOG_V35_STATUS`
- `g_ncFeatureBacklogV35Status`

固定長構造体のみを使用し、RT側で動的メモリ確保、ファイルI/O、Windows API呼び出しを行わない設計としている。

## 追加した10機能

| ID | 機能名 | 主な処理配置 | 安全方針 |
|---:|---|---|---|
| 270 | 主軸オリエンテーション完了監視 | RT監視/TS診断 | タイムアウト時HOLD |
| 271 | ATCアーム原点不一致ガード | RT監視/TSゲート | 不一致時インターロック |
| 272 | クランプ/アンクランプ二重確認 | RT監視/TSゲート | 二重指令・確認不一致時インターロック |
| 273 | ワーク着座エアブロー確認 | RT監視/TSゲート | 着座未確認時シーケンスゲート |
| 274 | 切粉コンベア過負荷HOLD | RT監視/TS診断 | 過負荷時HOLD |
| 275 | クーラントポンプ空運転保護 | RT監視/TS診断 | 流量未確認時HOLD |
| 276 | 主軸冷却水温上昇段階減速 | RT監視/TS補正 | 警告温度で減速、異常温度でHOLD |
| 277 | サーボアンプREADY喪失即時HOLD | RT監視/TS診断 | READY喪失時HOLD |
| 278 | 非常停止系統不一致トレース | RT監視/TS証跡 | 入力/安全リレー不一致をイベント化 |
| 279 | 保護カバーLOCKタイムアウト監視 | RT監視/TSゲート | LOCK未完了時ゲート/HOLD候補 |

## 実装方針

- v34_10 の次段として、概念上の総機能数を `270 → 280` に拡張する前提で `NC_IMPL_BACKLOG_FEATURE_COUNT_V35` を定義した。
- `NC_IMPL_BACKLOG_WORD_COUNT_V35` は 280件でも 32bitワード換算で9ワードに収まるため、9のままとした。
- RT側処理は `NcFeatureBacklogV35_RunRtTick()` に集約し、入力信号からHOLD/INTERLOCK/GATE/SLOWDOWN/TRACEのカウンタを更新する。
- TS側処理は `NcFeatureBacklogV35_ApplyBlockTs()` に集約し、固定長ブロックに補助要求、ゲート、インターロック、HOLD、診断、トレースコードを反映する。
- 既存基盤に統合する場合は、`rt_control.c` の周期処理末尾または診断処理部から `NcFeatureBacklogV35_RunRtTick()` を呼び、`ts_service.c` の固定ブロック生成後に `NcFeatureBacklogV35_ApplyBlockTs()` を呼び出す。

## 検証

GCC smoke build/test を実行済み。

```text
implementation_backlog_v35 implemented=10 configured=10 ts=1 rt=6 hold=30 interlock=11 gate=8 slowdown=7 trace=7 cat_mask=0x00000200 last_id=279 last_policy=1 trace=0x3508 last_x=2700
PASS nc_feature_backlog_v35
```

## Windows側確認コマンド

VC2026:

```bat
cd source
scripts\test_vc2026_v35_10.bat
```

clang Windows:

```bat
cd source
scripts\test_clang_v35_10.bat
```

GCC smoke:

```bash
cd source
scripts/test_gcc_smoke_v35_10.sh
```
