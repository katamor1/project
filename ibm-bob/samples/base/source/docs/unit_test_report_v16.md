# IBM-Bob 追加単体テスト v16 レポート

## 概要

- 目的: v15までで未確認または薄かった別関数・別分岐に対し、追加で40本の単体テストを作成する。
- 追加テスト: 10ファイル / 40ケース
- 確認環境: GCC / C99 / Linuxサンドボックス
- 結果: v16追加分40本すべてPASS

## 追加対象

| No | 分類 | テストファイル | ケース数 | 主な確認観点 |
|---:|---|---|---:|---|
| 1 | NC補正 | `test_nc_compensation_v16_transform_more.c` | 4 | 負方向工具長補正 / 極座標変換 / 座標回転キャンセル / ゼロ長工具径補正no-op |
| 2 | 座標変換 | `test_nc_coordinate_transform_v16_enable_more.c` | 4 | 有効フラグ別補正適用 / 不正軸API拒否 / 円弧中心補正 / 非モーションno-op |
| 3 | 軸構成 | `test_nc_axis_config_v16_filter_more.c` | 4 | 軸名正規化 / 不正経路軸マスク拒否 / 径/半径変換 / 制御軸取り外し・経路軸フィルタ |
| 4 | キネマティクス | `test_nc_kinematics_v16_transform_more.c` | 4 | 論理/物理軸割付 / ミラー中心反転 / 傾斜特異点警告 / 軸退避/復帰マスク |
| 5 | 補間数学 | `test_nc_interp_math_v16_geometry_more.c` | 4 | 平面軸選択 / CW/CCW掃引方向 / IJK円弧長 / IJK/R競合拒否 |
| 6 | 補間RT | `test_nc_interpolation_v16_step_more.c` | 4 | Begin拒否 / 直線補間完了 / Cancel / Dwell時コマンド非出力 |
| 7 | パーサ内部 | `test_nc_parser_internal_v16_modal_more.c` | 4 | 初期ブロック / F/S設定 / 同一グループ重複拒否 / G53+G91拒否 |
| 8 | RT制御 | `test_rt_control_v16_state_more.c` | 4 | 起動前インターロックOK / STOPPED/READY/RUNNING遷移 / AUTO要求拒否/許可 / 異常時安全出力 |
| 9 | TSサービス | `test_ts_service_v16_prefetch_ui_more.c` | 4 | prefetch完了 / busy中no-op / UIスナップショット / ログflush要求 |
| 10 | Control API | `test_control_api_v16_request_more.c` | 4 | 不正運転モード拒否 / NCロード拒否 / バイナリロード受付 / NULL getter拒否 |

## 実行方法

### GCC

```sh
cd source
scripts/run_tests_v16_gcc.sh
```

### Visual Studio / cl.exe

```bat
cd source
scripts\run_tests_v16.bat
```

## 確認結果

`source/build/test_results_v16_added.txt` にGCCでの実行ログを保存済み。

```text
V16_ADDED_TESTS_TOTAL=40
V16_ADDED_TESTS_PASS=40
[run_tests_v16_gcc] All v16 additive tests passed.
```
