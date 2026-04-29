# IBM-Bob 追加機能実装レポート v5

## 目的

v4 で追加した工具長補正、工具径補正、座標回転、極座標モードの上に、NC実行層をさらに実機制御寄りに近づけるため、円弧/ヘリカル補間、簡易先読み、コーナー減速のサンプル実装を追加した。

## 追加実装

### 1. 円弧/ヘリカル補間の固定セグメント化

- `G02/G03` の円弧補間を、RT周期ごとの三角関数計算ではなく、ブロック開始時に固定長セグメント列へ展開する方式に変更した。
- Z等の平面外軸を同時に変化させる場合は、円弧平面の弦分割と平面外軸の線形補間を組み合わせ、ヘリカル補間として扱う。
- RT周期中は `OutputSegmentAtTick()` による線形セグメント補間のみで軸目標を生成する。

### 2. TS側簡易先読み/コーナー減速

- `nc_lookahead.h` / `nc_lookahead.c` を追加した。
- TS側で連続するモーションブロックのベクトル角度を評価し、角度がしきい値以上の場合に当該ブロックの `interp_ticks` を増やして保守的に減速する。
- RT側には減速済みの固定ブロックのみを渡すため、RT周期中に角度評価や高負荷演算を行わない。

### 3. 経路制御状態の共有メモリ/API

- `NC_PATH_CONTROL_STATUS` を追加した。
- `g_ncPathControlStatus` を追加した。
- `Api_GetNcPathControlStatus()` を追加した。
- 以下の状態を公開する。
  - 円弧ブロック数
  - ヘリカルブロック数
  - 生成済み円弧セグメント数
  - 先読み評価ブロック数
  - コーナー減速イベント数
  - 直近コーナー角度
  - 直近減速オーバライド率

### 4. スモークテスト拡張

- `G02` ヘリカル円弧
- `G03` 円弧
- 鋭角コーナーを含む連続直線
- 既存の固定サイクル、MFIN、工具補正、座標回転、G31スキップ、I/Oトレース

を同一NCプログラム内で確認するようにした。

## 変更ファイル

- `inc/system_config.h`
- `inc/system_shared.h`
- `inc/control_api.h`
- `inc/nc_lookahead.h`
- `src/system_shared.c`
- `src/control_api.c`
- `src/nc_lookahead.c`
- `src/nc_program.c`
- `src/nc_interpolation.c`
- `src/main.c`

## 確認結果

`docs/verification_output_v5.txt` を参照。

主な確認値:

- `nc_state=6`
- `nc_error=0`
- `path_arc=2`
- `path_helical=1`
- `path_arc_segments=8`
- `path_lookahead=14`
- `path_slowdowns=11`
- `aux_completed=1`
- `trace_frozen=1`
- `coord_error_checks=5`

## 注意事項

今回の円弧/ヘリカル補間は固定セグメント化のサンプル実装である。実機向けには、許容弦誤差、加減速連続性、工具径補正との厳密な接続、象限またぎ、同一点全周円、微小円弧、機械別速度制限の詳細化が必要である。
