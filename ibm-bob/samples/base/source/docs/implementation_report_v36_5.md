# IBM-Bob 未実装機能追加実装 v36_5 実装レポート

## 1. 概要

`v35_10` の追加ID `270-279` に続き、`v36_5` として未実装/入口実装止まりだった機械保護・保全系機能を5件追加した。

本実装は、既存IBM-Bob方針に合わせ、RT/TS分離を維持する。

- TS側: 固定長ブロックへのゲート、診断、HOLD、インターロック、熱補正オフセット反映
- RT側: 固定長I/Oスナップショットに基づく周期監視、タイムアウト判定、状態カウンタ更新
- 禁止事項: 動的メモリ確保、ファイルI/O、Windows API、非固定長ループなし

## 2. 追加ファイル

| 種別 | パス | 内容 |
|---|---|---|
| ヘッダ | `source/inc/nc_feature_backlog_v36.h` | v36_5用API、共有ステータス、RT入力、TSブロック定義 |
| 実装 | `source/src/nc_feature_backlog_v36.c` | 5機能のTS/RT実装、自診断 |
| テスト | `source/tests/test_nc_feature_backlog_v36.c` | GCC smoke test |
| スクリプト | `source/scripts/build_gcc_smoke_v36_5.sh` | GCCビルド |
| スクリプト | `source/scripts/test_gcc_smoke_v36_5.sh` | GCCビルド+実行 |
| スクリプト | `source/scripts/build_vc2026_v36_5.bat` | VC2026想定ビルド |
| スクリプト | `source/scripts/test_vc2026_v36_5.bat` | VC2026想定テスト |
| スクリプト | `source/scripts/build_clang_v36_5.bat` | clang Windows想定ビルド |
| スクリプト | `source/scripts/test_clang_v36_5.bat` | clang Windows想定テスト |

## 3. 追加した5機能

| ID | 機能名 | TS側処理 | RT側処理 | 安全側動作 |
|---:|---|---|---|---|
| 280 | 潤滑圧低下段階HOLD | 診断/HOLD要求ビット設定 | ポンプON中の圧力/油面低下をタイムアウト監視 | HOLD要求、fault mask更新 |
| 281 | 軸ブレーキ解除フィードバック不一致ガード | シーケンスゲート/インターロック設定 | ブレーキ解除指令に対する解除FB不足を監視 | インターロック、対象軸mask記録 |
| 282 | チャッククランプ圧確認 | シーケンスゲート/診断設定 | クランプ指令中の圧力不足、クランプ/アンクランプ同時指令を監視 | ゲート停止、fault mask更新 |
| 283 | パレットクランプ/アンクランプタイムアウト | シーケンスゲート/インターロック設定 | クランプ/アンクランプ要求とセンサ不一致を監視 | インターロック、timeout記録 |
| 284 | 熱変位補正READYゲート | 熱補正オフセットのクランプ反映、送り3%低減、ゲート/診断設定 | 補正要求中のREADY未成立、温度差閾値超過を監視 | 段階減速、必要時HOLD、trace記録 |

## 4. API

追加APIは以下。

```c
void NcFeatureBacklogV36_Init(void);
int NcFeatureBacklogV36_IsFeatureEnabled(uint32_t feature_id);
int Api_SetNcFeatureBacklogV36Feature(uint32_t feature_id, uint32_t enabled);
void Api_EnableAllNcFeatureBacklogV36Features(void);
int Api_GetNcFeatureBacklogV36Status(NC_FEATURE_BACKLOG_V36_STATUS *out_status);
int Api_RunNcFeatureBacklogV36SelfCheck(void);
void NcFeatureBacklogV36_ApplyBlockTs(NC_FEATURE_BACKLOG_V36_BLOCK *block);
void NcFeatureBacklogV36_RunRtTick(const NC_FEATURE_BACKLOG_V36_RT_IO *io);
const char *NcFeatureBacklogV36_GetFeatureName(uint32_t feature_id);
```

## 5. RT安全性

- ループ上限は `NC_FEATURE_BACKLOG_V36_FEATURE_COUNT == 5` に固定
- 静的状態のみ使用
- `memset` 以外の標準ライブラリ依存はなし
- RT tick関数は入力ポインタNULL時に警告カウントのみ更新して復帰
- タイムアウトカウンタは静的固定長カウンタ

## 6. 検証

GCC smoke build/testを実行しPASSを確認。

```text
implementation_backlog_v36 implemented=5 configured=5 ts=1 rt=6 hold=8 interlock=7 gate=5 slowdown=4 trace=0 cat_mask=0x00000200 last_id=284 last_policy=4 trace=0x3604 fault=0x0000001f last_x=2812
PASS nc_feature_backlog_v36
```

既存v35 smoke testも再実行しPASSを確認。

```text
implementation_backlog_v35 implemented=10 configured=10 ts=1 rt=6 hold=30 interlock=11 gate=8 slowdown=7 trace=7 cat_mask=0x00000200 last_id=279 last_policy=1 trace=0x3508 last_x=2700
PASS nc_feature_backlog_v35
```

## 7. Windows環境での確認コマンド

VC2026:

```bat
cd source
scripts\test_vc2026_v36_5.bat
```

clang Windows:

```bat
cd source
scripts\test_clang_v36_5.bat
```

GCC smoke:

```bash
cd source
scripts/test_gcc_smoke_v36_5.sh
```

## 8. 統合メモ

既存の総合backlogモジュールへ統合する場合は、以下を反映する。

- `NC_IMPL_BACKLOG_FEATURE_COUNT` を `280 -> 285` に拡張
- `NC_IMPL_BACKLOG_WORD_COUNT` は `9` のまま維持
- ID `280-284` を機能定義テーブルへ追加
- 公開ヘッダへv36 APIまたは総合APIラッパーを追加
- ビルド対象へ `src/nc_feature_backlog_v36.c` を追加
- 単体/スモークテスト対象へ `tests/test_nc_feature_backlog_v36.c` を追加
