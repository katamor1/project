# IBM-Bob source implementation report v21

## Scope

v20をベースに、設計書側でまだ入口実装または未接続だったNC制御機能を10件追加した。
RT周期内では重い幾何計算・ファイルI/O・動的メモリ確保を行わず、TS側の固定長ブロック変換、状態公開、RT実行時イベント記録として実装した。

## Implemented items

1. **インボリュート補間 G02.2/G03.2**
   - `nc_codes.c` にGコードを追加。
   - `nc_parser_feature.c` で高度補間ブロックとして認識。
   - `nc_design_features.c` で最小補間tickと速度抑制を適用。

2. **高精度輪郭/スムージング実行準備 G05/G05.1/G05.2/G08**
   - 既存のG05/G08認識結果をv21ステータスへ接続。
   - 運動ブロックでは補間tick、加減速tick、速度抑制を付与。

3. **完全停止/ねじモード停止 G09/G61/G63**
   - `G63` を停止制御グループとして扱うように修正。
   - exact-stop状態をTS/RT双方のステータスへ記録。

4. **リジッドタップ準備 G84/G74**
   - タップサイクルで主軸同期用の最小tickを保証。
   - 主軸回転数未指定時は安全側の最小RPMを補完。
   - feed/rev相当のピッチ診断値を状態公開。

5. **特殊ねじ切り/面取り・退避入口 G76.7/G78/G63**
   - thread motionを検出し、減速tickを安全側に補強。
   - 面取り/退避対象ブロック数をステータス化。

6. **第2レファレンス点復帰 G30**
   - `G30` を第2レファレンス復帰対象として状態公開。
   - RT側イベント記録へ接続。

7. **機械座標直接指令 G53**
   - G53モーダルフラグ付きブロックを機械座標直接指令としてカウント。

8. **座標シフト/プリセット G10/G52/G92**
   - 既存の座標変換処理に加えて、v21設計機能ステータスへ連携。

9. **周速一定/固定回転切替 G96/G97**
   - G96/G97切替ブロックを設計機能ステータスへ公開。
   - CSS診断用の代表径を保持。

10. **回転送り G95 + motion**
    - feed-per-rev状態で実行される運動ブロックを検出。
    - 主軸回転数と実効送りをステータスへ保存。

## Changed files

- `inc/system_config.h`
- `inc/system_shared.h`
- `src/nc_codes.c`
- `src/nc_parser_feature.c`
- `src/nc_parser_modal.c`
- `src/nc_design_features.c`
- `src/main.c`

## Public shared-memory extension

`NC_DESIGN_FEATURE_STATUS` に以下のカウンタを追加した。

- `involute_blocks`
- `high_precision_contour_blocks`
- `exact_stop_blocks`
- `rigid_tap_blocks`
- `thread_chamfer_blocks`
- `second_reference_blocks`
- `machine_direct_blocks`
- `coordinate_shift_blocks`
- `constant_surface_speed_blocks`
- `feed_per_rev_blocks`

診断値として以下も追加した。

- `last_effective_feed`
- `last_spindle_speed`
- `last_rigid_tap_pitch`
- `last_css_diameter`

## Verification

The GCC smoke executable was built from all `src/*.c` files and run successfully in the sandbox. The portable script compilation is slow in this environment, so verification was performed with object-by-object GCC compilation followed by manual smoke execution.

Key output:

```text
nc_state=6 nc_error=0
feature_blocks=84 feature_flags=57346 canned=27 skip=1 advanced=11
design_features mode=0x000ffdff nurbs=1 exp=1 cyl=3 polar=3 helical_b=1 auto_corner=2 grinding=15 egb_skip=1 interference3d=26 followup=0 ts=49 rt=50 slowdown=50 override=70 last_x=1220 last_y=2320
design_features_v21 involute=2 hp_contour=7 exact_stop=6 rigid_tap=2 thread_chamfer=1 second_ref=1 machine_direct=1 coord_shift=3 css=2 feed_per_rev=3
```

Full output: `docs/verification_output_v21.txt`.
