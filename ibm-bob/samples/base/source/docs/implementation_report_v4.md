# IBM-Bob 追加機能実装 v4 レポート

## 目的
v3で実装した固定サイクル、G31スキップ、MFIN、I/Oトレースに続き、v4では高度加工系で必要になるTS側前処理を強化した。
RT側に可変長解析、工具表参照、三角関数計算を持ち込まず、TS側で補正後の固定長 `NC_EXEC_BLOCK` に変換する方針を維持した。

## 追加実装

### 1. 工具長補正 G43/G44/G49
- `G43 Hn` / `G44 Hn` / `G49` を解析。
- `Api_SetNcToolLengthOffset()` で工具長補正量を登録。
- TS側でZ軸目標値に補正量を反映。
- `NC_FEATURE_FLAG_TOOL_LENGTH` を追加。

### 2. 工具径補正 G41/G42/G40
- `G41 Dn` / `G42 Dn` / `G40` を解析。
- `Api_SetNcCutterRadiusOffset()` で工具径補正量を登録。
- TS側でXY移動方向の法線方向へ終点を補正。
- `NC_FEATURE_FLAG_CUTTER_RADIUS` を追加。

### 3. 座標回転 G68/G69
- `G68 X... Y... R...` / `G69` を解析。
- G68中の `X/Y` は移動指令ではなく回転中心として扱う。
- G68中の `R` は角度として扱い、`NC_ANGLE_SCALE` で固定小数点化。
- TS側でXY目標値を回転変換。
- `NC_FEATURE_FLAG_COORD_ROTATION` を追加。

### 4. 極座標モード G16/G15
- `G16` / `G15` を解析。
- 極座標モード状態を `NC_COMPENSATION_STATUS` に公開。
- TS側固定ブロック化の入口を実装。

### 5. 補正状態API
- `NC_COMPENSATION_STATUS`
- `g_ncCompensationStatus`
- `Api_GetNcCompensationStatus()`
- `Api_SetNcToolLengthOffset()`
- `Api_SetNcCutterRadiusOffset()`

### 6. スモークテスト拡張
- `G43 H1`
- `G41 D1`
- `G68 X0 Y0 R15`
- `G16/G15`
- `G69/G40/G49`
- MFIN待ち時間が非ゼロになるよう入力タイミングを調整。

## 検証結果

ビルドコマンド:

```sh
gcc -std=c99 -Wall -Wextra -Iinc src/*.c -lm -o build_v4_test
```

実行結果:

```text
nc_state=6 nc_error=0 nc_error_line=0 nc_exec=35 nc_read=35 nc_buf=0 nc_x=1200 nc_y=2300 nc_z=3400 nc_m=130
interp_segments=11 feed_profile=1 feed_override=100 dwell_ticks=1
coord_unit=0 coord_distance=0 coord_work=0 coord_local=0 coord_work_x=1000
feature_blocks=30 feature_flags=97 canned=2 skip=1 advanced=3
comp_applied=9 comp_mask=0x00000007 comp_tool=0 comp_cutter=0 comp_rotation=0
cycle_completed=2 cycle_phase_count=1 skip_completed=1 skip_x=533
aux_completed=1 aux_wait_cycles=91 prestart_ok=0 prestart_mask=0x00000014
trace_frozen=1 trace_captured=32 trace_trigger_cycle=451
mode=1 state=3 alarm=9001 cycle=520
coord_error_checks=5
```

## v4時点の制限
- 工具径補正は終点補正のサンプル実装であり、コーナー接続、干渉回避、アプローチ/キャンセル動作の完全な幾何処理は未実装。
- 座標回転と極座標はTS側固定ブロック変換の入口実装であり、全平面・全モーダル組合せの実機完全互換までは未実装。
- NURBS、自由曲面、高精度輪郭、3D工具補正は、まだ固定ブロック化・状態公開レイヤ中心。
