<!-- ibm-bob/samples/base/source/docs/implementation_report_v38_10.md -->
<!-- Archives the pre-normalization v38_10 source evidence. -->
<!-- This exists so current verification can distinguish archive evidence from runnable commands. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_pallet_fixture_safety.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_pallet_fixture_safety.c -->
# IBM-Bob 未実装機能追加 実装レポート v38_10

> **Archive note:** This is pre-normalization archive evidence. The current tree uses meaning-based files `inc/nc_feature_backlog_pallet_fixture_safety.h`, `src/nc_feature_backlog_pallet_fixture_safety.c`, and `tests/test_nc_feature_backlog_pallet_fixture_safety.c`. Current verification is manifest-driven via `cmd /c scripts\run_tests.bat`; pinned sample smoke uses `cmd /c scripts\test_clang.bat` or `cmd /c scripts\test_vc2026.bat`.

## 概要

`v37_10` の続きとして、未実装/入口実装止まりだった周辺装置READY監視・安全保全系の機能を10件追加した。
追加IDは `295〜304` とし、既存の `v35〜v37` と同じく独立モジュール方式で実装した。

## 追加ファイル

- `source/inc/nc_feature_backlog_v38.h`
- `source/src/nc_feature_backlog_v38.c`
- `source/tests/test_nc_feature_backlog_v38.c`
- `source/scripts/build_gcc_smoke_v38_10.sh`
- `source/scripts/test_gcc_smoke_v38_10.sh`
- `source/scripts/build_vc2026_v38_10.bat`
- `source/scripts/test_vc2026_v38_10.bat`
- `source/scripts/build_clang_v38_10.bat`
- `source/scripts/test_clang_v38_10.bat`

## 追加した10機能

| ID | 機能名 | 処理方針 | 主な動作 |
|---:|---|---|---|
| 295 | 機械熱安定READY禁止 | INHIBIT | 熱安定待ち中、温度未READYまたは熱変位量過大時に運転/測定系を抑止する。 |
| 296 | 油圧フィルタ目詰まりトレース | TRACE | 油圧フィルタ目詰まり入力が継続した場合に診断トレースへ記録する。 |
| 297 | ATCポット割出確認インターロック | INTERLOCK | ATCポット割出要求中に位置確認が成立しない場合、起動/シーケンスを禁止する。 |
| 298 | パレットID不一致ゲート | GATE | 期待パレットIDと実パレットIDの不一致時に加工開始をゲートする。 |
| 299 | Z軸カウンタバランス圧低下HOLD | HOLD | Z軸有効時のカウンタバランス圧低下を監視し、HOLD要求と軸マスクを公開する。 |
| 300 | サーボ絶対位置不一致トレース | TRACE | 絶対位置照合対象軸で不一致が継続した場合に軸マスク付きでトレースする。 |
| 301 | 主軸工具アンクランプ確認リトライ | RETRY | 工具アンクランプ要求後に確認入力が入らない場合、リトライ要求を発行する。 |
| 302 | クーラントタンク液面低下ゲート | GATE | クーラント要求中の液面低下でシーケンスゲートとHOLD側の保護を行う。 |
| 303 | ミストコレクタREADYゲート | GATE | ミストコレクタ必須条件でREADY未成立の場合、加工開始/補助機能をゲートする。 |
| 304 | 安全マット侵入HOLD | HOLD | 安全マット侵入入力を即時HOLD/インターロック/トレース対象として扱う。 |

## RT/TS分担

- TS側: `NcFeatureBacklogV38_ApplyBlockTs()` で固定長ブロックに対し、ゲート、HOLD、インターロック、リトライ、抑止、診断トレース、送りオーバライド低減を設定する。
- RT側: `NcFeatureBacklogV38_RunRtTick()` で周期入力を固定長カウンタにより監視し、タイムアウト成立時に状態公開する。
- API側: `Api_SetNcFeatureBacklogV38Feature()`、`Api_EnableAllNcFeatureBacklogV38Features()`、`Api_GetNcFeatureBacklogV38Status()`、`Api_RunNcFeatureBacklogV38SelfCheck()` を追加した。

## RT安全性

- 動的メモリ確保なし。
- ファイルI/Oなし。
- Windows API呼び出しなし。
- ループ上限は `NC_FEATURE_BACKLOG_V38_FEATURE_COUNT = 10` の固定長。
- 状態は `g_ncFeatureBacklogV38Status` と静的カウンタのみ。

## 検証

GCC smoke build/test を実行し、PASSを確認した。

```text
implementation_backlog_v38 implemented=10 configured=10 ts=1 rt=6 hold=15 interlock=4 gate=16 trace=12 retry=5 inhibit=4 cat_mask=0x00000200 last_id=304 last_policy=2 trace=0x3809 fault=0x000003ff axis=0x00000005 pallet=17/23 thermal=190 last_x=1800
PASS nc_feature_backlog_v38
```

## Windows確認方法

```bat
cd source
scripts\test_vc2026_v38_10.bat
scripts\test_clang_v38_10.bat
```

