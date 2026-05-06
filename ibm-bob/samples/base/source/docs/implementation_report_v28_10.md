# IBM-Bob ソースコード追加実装レポート v28_10

## 概要

`ibm_bob_source_implemented_additional_features_v27_20.zip` をベースに、未実装/入口実装止まりだった設計機能を10件追加した。

## 変更サマリ

- `NC_IMPL_BACKLOG_FEATURE_COUNT` を `200U` から `210U` に拡張。
- `NC_IMPL_BACKLOG_WORD_COUNT` は `7U` のまま維持。
  - 210件は 32bit * 7 word = 224 bit 内に収まるため。
- `NC_IMPL_BACKLOG_V28_FIRST_FEATURE` / `NC_IMPL_BACKLOG_V28_ADDED_FEATURES` を追加。
- `nc_feature_backlog.c` のバックログ記述子へ ID `200U`〜`209U` を追加。
- `ApplyV23Policy()` に ID `200U`〜`209U` のTS側固定長ブロック補正を追加。
- GCC smoke build/test 用スクリプト `build_gcc_smoke_v28_10.sh` / `test_gcc_smoke_v28_10.sh` を追加。

## 追加した10機能

| ID | 機能名 | 実装概要 |
|---:|---|---|
| 200 | 輪郭先読み急峻角ブレーキ | XY移動量から急峻角相当を検出し、送り低下と減速tickを付与 |
| 201 | 微小円弧チョード誤差補正 | 円弧半径の下限ガード、終点微調整、高精度フラグ付与 |
| 202 | 高速加工G05.1有効範囲監視 | 高速加工/高度補間フラグを付与し、送り上限と加減速tickを制御 |
| 203 | ナノ補間丸め誤差蓄積リセット | 有効軸の指令値を微小ステップ境界へ丸め、累積誤差を抑制 |
| 204 | 工具先端点姿勢変化率制限 | 工具姿勢角をパラメータ範囲でクランプし、姿勢変化時の送りを制限 |
| 205 | 自動潤滑ポンプREADY監視 | 潤滑READY確認用Mコード/MFIN待ちを固定長ブロックへ付与 |
| 206 | クランプエア圧低下HOLDラッチ | エア圧低下相当時に退避フラグ、MFIN待ち、強制減速を付与 |
| 207 | 切削油温度上昇フィード制限 | 切削油温上昇相当時に送り上限・加減速tick・平滑化を付与 |
| 208 | チャック回転中ドアロック二重確認 | 主軸回転中のドアロック確認用補助Mコードと安全減速を付与 |
| 209 | 主軸冷却ファン異常段階減速 | 主軸冷却異常相当時に主軸速度フォールバック、減速tick、平滑化を付与 |

## RT/TS分担

- TS側: `NcFeatureBacklog_ApplyBlockTs()` 経由で固定長 `NC_EXEC_BLOCK` の補正を行う。
- RT側: `NcFeatureBacklog_OnBlockRt()` 経由で状態カウントとイベント記録のみ行う。
- RT側では動的メモリ確保、ファイルI/O、Windows API呼び出しは追加していない。

## 検証

以下を実行し、GCC smoke build/test を通過した。

```bash
cd source
scripts/test_gcc_smoke_v28_10.sh
```

確認結果の要点:

```text
implementation_backlog implemented=210 configured=210 ts=525 rt=268 synthetic=210 cat_mask=0x000003ff last_id=209 last_cat=4 last_policy=5 slowdown=43 cat0=108 cat9=80 last_x=1209
coord_error_checks=5
g08_error_checks=3
hpcc_error_checks=4
cycle_error_checks=4
```

## 変更ファイル

- `source/inc/system_config.h`
- `source/src/nc_feature_backlog.c`
- `source/scripts/build_gcc_smoke_v28_10.sh`
- `source/scripts/test_gcc_smoke_v28_10.sh`
- `source/docs/implementation_report_v28_10.md`
- `source/docs/verification_output_v28_10.txt`
- `source/docs/manifest_v28_10.json`
- `source/docs/continuation_v28_10_diff.patch`
