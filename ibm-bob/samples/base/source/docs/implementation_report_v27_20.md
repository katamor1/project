# IBM-Bob 追加未実装機能 実装レポート v27_20

## 概要

v26_20 をベースに、設計書上は未実装または入口実装止まりだった20件を `NC_IMPL_BACKLOG` の ID 180〜199 として追加実装した。
RT/TS分離方針に従い、TS側では `NcFeatureBacklog_ApplyBlockTs()` の固定長NCブロック補正として実装し、RT側では動的メモリ確保・ファイルI/O・Windows APIを使わず、イベント/カウンタ更新のみを行う。

## 変更ファイル

- `inc/system_config.h`
- `inc/system_shared.h`
- `src/nc_feature_backlog.c`
- `scripts/build_gcc_smoke_v27.sh`
- `scripts/test_gcc_smoke_v27.sh`
- `docs/verification_output_v27_20.txt`
- `docs/continuation_v27_20_diff.patch`

## 定数・共有メモリ変更

- `NC_IMPL_BACKLOG_FEATURE_COUNT`: 180 → 200
- `NC_IMPL_BACKLOG_WORD_COUNT`: 6 → 7
- `NC_IMPL_BACKLOG_V27_FIRST_FEATURE`: 180 を追加
- `NC_IMPL_BACKLOG_V27_ADDED_FEATURES`: 20 を追加
- `NC_IMPLEMENTATION_BACKLOG_STATUS` の説明コメントを v22-v27 / 200件に更新

## 追加した20機能

- 180. 加工中ワーク座標系変更禁止ラッチ: 運転中のG10/G54系更新をモーダルラッチとして抑制し、移動中は送りを安全側に減速
- 181. プログラム停止時補助軸減速退避: M00/M02相当の停止ブロックで補助軸を固定量退避しMFIN待ちへ接続
- 182. 主軸負荷ピークホールド減速: 主軸負荷ピークを想定して減速tickを増やし送りを段階制限
- 183. 工具交換アーム干渉領域クランプ: ATC干渉領域を固定範囲でクランプし退避フラグを付与
- 184. クランプ/アンクランプMコード順序監視: Mコード/MFIN待ちを生成し、順序監視用generationを更新
- 185. APCパレット番号整合監視: Pワードでパレット番号を固定範囲へ正規化しM60系待ちを生成
- 186. タレット割出し完了待ち監視: 工具番号とM06/MFIN待ちを補完し安全送りへ制限
- 187. ローダ/アンローダ干渉待ち合わせ: ローダ干渉領域をXYでクランプし退避・MFIN待ちへ接続
- 188. 主軸同期C軸切替ガード: C/A軸相当の最短回転補正と主軸同期切替用減速を追加
- 189. C軸輪郭制御送り正規化: C軸輪郭制御時の軸マスク/送り上限/平滑化フラグを補正
- 190. 極小円弧半径丸め補正: 小半径円弧の半径・中心値を固定範囲へ丸め、補間tickを保証
- 191. ヘリカル補間Z送り同期補正: Z軸を含むヘリカル補間として扱い、ADVANCED_INTERPと減速を付与
- 192. 工具長摩耗補正上限ガード: H番号/工具長指令/Z目標を範囲ガードし工具長補正フラグを付与
- 193. 工具径摩耗補正上限ガード: D番号/工具径指令/XY対象を範囲ガードし工具径補正フラグを付与
- 194. G10パラメータ書込み運転中禁止: G10相当の書込みブロックを運転中は禁止ラッチ＋MFIN待ちへ変換
- 195. 通信異常時NCバッファ消費停止: 通信異常相当でバッファ消費を止める安全待ちブロックを生成
- 196. 先読み異常時安全停止ブロック生成: 先読み異常時に軸移動なし・安全停止用の固定長ブロックへ変換
- 197. ドライランI/O出力抑止: ドライラン時に補助M出力を抑止し、送りだけを固定範囲に正規化
- 198. シミュレーションモード安全出力マスク: シミュレーション中の補助出力を全マスクし、軸割付/平滑化のみ残す
- 199. 保守モード軸単独送り速度制限: 保守単独送りを対象軸/速度/加減速tickの固定範囲へ制限

## 検証

以下を実行し、GCC smoke build/test が成功した。

```bash
cd source
bash scripts/test_gcc_smoke_v27.sh
```

検証結果の要点:

```text
implementation_backlog implemented=200 configured=200 ts=504 rt=258 synthetic=200 cat_mask=0x000003ff last_id=199 last_cat=4 last_policy=3 slowdown=34 cat0=100 cat9=77 last_x=1199
nc_state=4 nc_error=0
cycle_error_checks=4
```

## 補足

本実装はサンプル基盤上での固定長ブロック変換実装であり、実機投入時は設備別パラメータ、PLC信号割付、サーボ仕様、各Mコードの完了条件を詳細設計値に合わせて確定する必要がある。
