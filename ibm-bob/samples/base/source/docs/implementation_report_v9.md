# IBM-Bob 追加実装 v9 レポート

## 目的

v8で追加したモーション指令平滑化フィルターに対し、実機適用時に問題になりやすい以下を補強した。

- フィルター後速度の軸別上限制限
- フィルター後速度変化量、すなわち加速度相当量の軸別上限制限
- フィルター遅れ量と最大遅れ量の状態公開
- 終点近傍での残差補正と終点スナップ記録
- 制限発生/終点補正の固定長イベントリング記録

RT側は引き続き固定長配列のみを使用し、動的メモリ確保、ファイルI/O、Windows API呼び出しは行わない。

## 追加・変更ファイル

- `inc/system_config.h`
  - モーションフィルター軸別制限、終点補正窓、終点許容残差の定数を追加。
- `inc/system_shared.h`
  - `NC_MOTION_FILTER_STATUS` に軸別速度/加速度上限、遅れ量、終点残差、制限イベント数を追加。
  - `NC_EVENT_CODE_MOTION_FILTER_SATURATED`、`NC_EVENT_CODE_MOTION_FILTER_ENDPOINT_CORRECT` を追加。
- `inc/nc_motion_filter.h`
  - `NcMotionFilter_SetAxisLimitRt()` を追加。
  - `NcMotionFilter_ApplyRt()` に残りtick数を渡す引数を追加。
- `inc/control_api.h` / `src/control_api.c`
  - `Api_SetNcMotionFilterAxisLimit()` を追加。
- `src/nc_motion_filter.c`
  - 速度リミット、加速度リミット、終点残差補正、フィルター遅れ計測を実装。
- `src/nc_interpolation.c`
  - 補間残りtickをモーションフィルターへ渡すように変更。
- `src/system_shared.c`
  - モーションフィルター状態の初期値に軸別制限値を追加。
- `src/main.c`
  - 軸別制限設定、状態表示、回帰確認を追加。

## 実装方針

補間器は従来どおり軸位置目標を生成する。モーションフィルターは、その位置目標から1周期ごとの速度成分を算出し、次の順に処理する。

1. 第1段: 速度移動平均
2. 第2段: 移動平均またはFIRによる加速度平滑化
3. 軸別加速度上限制限
4. 軸別速度上限制限
5. 終点近傍の残差補正
6. 最終tickでの終点一致保証

最終tickでは目標終点との一致を優先する。ただし補正量は `endpoint_corrections` とイベントリングに記録し、フィルター設計の妥当性確認に使えるようにした。

## 確認結果

コンテナ内で以下を実行した。

```text
cd source
./scripts/test_gcc_smoke.sh
```

実行結果の要点:

```text
nc_state=6 nc_error=0
motion_filter_samples=83 mode=2 axis_mask=0x00000009 raw_vx=3661 smooth_vx=3661 out_x=3400
motion_filter_limits vel_events=2 accel_events=12 endpoint_corrections=14 residual_samples=23 max_lag_x=1757 residual_x=-20
tool_life_hold_state=5 tool_life_hold_error=15 tool_life_hold_line=4 events=34 lost=0 last_event=8201
coord_error_checks=5
```

## 残課題

- 最終tickの終点一致では、必要に応じて大きめの補正速度が発生するため、実機向けには残差補正窓をより長くし、終点前に完全吸収するS字プロファイル化が必要。
- 軸別上限値は現在サンプル値。実機ではサーボ仕様、軸慣性、機械構成、単位系に基づく機種別パラメータ化が必要。
- 現在のFIR係数は固定 `{1,2,3,2,1}`。実機では周波数特性と遅れ量を評価し、係数テーブルを機種別/加工モード別に切り替える必要がある。
