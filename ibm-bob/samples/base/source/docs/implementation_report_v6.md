# IBM-Bob 追加機能実装レポート v6

## 目的

v5 までで実装した NC 解析、RT 固定ブロック実行、固定サイクル、G31 スキップ、MFIN、工具補正、座標回転、円弧/ヘリカル固定セグメント、簡易先読みを前提に、v6 では以下の設計書テーマを追加実装した。

- 任意角度・傾斜・ピボット軸制御の入口実装
- 軸名称/経路軸割付けの API 実装
- 制御軸取り外し/ミラー設定入力の入口実装
- 軸退避復帰の状態管理
- 5軸/工具軸方向の診断状態公開

## 追加ファイル

- `inc/nc_kinematics.h`
- `src/nc_kinematics.c`

## 主な追加内容

### TS側キネマティクス前処理

`NcKinematics_ApplyBlockTs()` を追加し、NCテキスト解析後、RTバッファ投入前に以下を処理する。

- `G68.2` による傾斜作業平面の有効化
- `G69.1` による傾斜作業平面の解除
- `G53.1` による工具軸方向ベクトルの状態化
- `G51.1` / `G50.1` によるミラー有効/解除
- `G10.6` / `G11.6` による軸退避/復帰状態管理
- 論理軸から物理軸への割付け、符号反転、軸取り外し

RT側には変換済みの `NC_EXEC_BLOCK` だけを渡すため、RT周期内にNC文字列解析、工具表検索、座標系モード判定、三角関数を持ち込まない方針を維持している。

### 追加共有メモリ/API

`NC_KINEMATICS_STATUS` と `g_ncKinematicsStatus` を追加した。

公開API:

- `Api_SetNcAxisAssignment()`
- `Api_SetNcMirrorMask()`
- `Api_GetNcKinematicsStatus()`

状態として以下を公開する。

- 傾斜平面有効状態
- 工具軸方向有効状態
- 論理軸/物理軸マスク
- 取り外し軸マスク
- ミラー軸マスク
- 退避軸マスク
- 軸割付けテーブル
- 工具軸方向ベクトル
- 変換済みブロック数
- ミラー適用ブロック数
- 退避/復帰イベント数
- 特異姿勢警告数
- 最終変換済み軸目標

### Gコード追加

以下を追加した。

- `G50.1` ミラー解除
- `G51.1` ミラー有効
- `G53.1` 工具軸方向指令
- `G68.2` 傾斜作業平面有効
- `G69.1` 傾斜作業平面解除
- `G11.6` 軸復帰サンプル

既存の `G10.6` は軸退避指令としても状態管理するように拡張した。

## スモークテスト

`main.c` のサンプルNCプログラムに以下を追加した。

- `G68.2 X0 Y0 Z0 A5 B10 C0`
- `G53.1 B10`
- `G51.1 X1`
- 5軸変換対象の `G01 ... A...`
- `G10.6 Z0.100`
- `G50.1`
- `G69.1`

実行結果は `docs/verification_output_v6.txt` を参照。

## 実行結果要点

```text
nc_state=6 nc_error=0 nc_error_line=0 nc_exec=45 nc_read=45 nc_buf=0 nc_x=3400 nc_y=2300 nc_z=3400 nc_m=130
interp_segments=16 feed_profile=1 feed_override=100 dwell_ticks=1
coord_unit=0 coord_distance=0 coord_work=0 coord_local=0 coord_work_x=1000
feature_blocks=40 feature_flags=97 canned=2 skip=1 advanced=3
comp_applied=11 comp_mask=0x00000007 comp_tool=0 comp_cutter=0 comp_rotation=0
path_arc=2 path_helical=1 path_arc_segments=8 path_lookahead=16 path_slowdowns=11 path_corner_deg=171 path_override=60
kin_transformed=16 kin_mirrored=11 kin_retracts=1 kin_singularity_warn=0 kin_mirror_mask=0x00000000 kin_retract_mask=0x00000004 kin_tool_z=9848 kin_last_x=3400 kin_last_a=-100
cycle_completed=2 cycle_phase_count=1 skip_completed=1 skip_x=3670
aux_completed=1 aux_wait_cycles=80 prestart_ok=0 prestart_mask=0x00000014
trace_frozen=1 trace_captured=32 trace_trigger_cycle=451
mode=1 state=3 alarm=9001 cycle=520
coord_error_checks=5
```

## 未実装/注意事項

- 傾斜作業平面はTS側サンプル変換であり、実機レベルの5軸TCP、ピボット長補正、工具先端点制御、回転軸機械構成別ポスト変換までは未実装。
- ミラー処理は座標反転の入口実装であり、工具径補正、円弧方向反転、ねじ切り方向、C軸/極座標との厳密な複合規則は未実装。
- 軸割付けは固定長テーブルによるサンプル実装であり、機種別パラメータ読込、重複割付け禁止、サーボ軸存在診断は今後拡張対象。
- 軸退避/復帰は状態管理と固定ブロック化の入口であり、干渉回避経路生成や実機安全エリア監視は未実装。
