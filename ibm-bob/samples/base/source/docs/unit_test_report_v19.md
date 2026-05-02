# IBM-Bob 追加単体テスト v19 レポート

## 概要

v18までの単体テストに対して、さらに別関数・別分岐を対象にした追加単体テストを40本作成した。
今回のv19では、TS側のブロック変換、RT側の補間実行、APIゲート、参照点復帰、RT制御トレースなど、既存テストで相対的に薄かった経路を重点的に確認する。

## 追加テスト件数

- 追加テストファイル: 10
- 追加テストケース: 40
- GCCビルド・実行結果: PASS

## 追加対象一覧

| No | テストファイル | 主な対象 | ケース数 |
|---:|---|---|---:|
| 1 | `test_nc_axis_config_v19_apply_more.c` | 軸定義、経路軸マスク、制御軸取り外し、径指令変換 | 4 |
| 2 | `test_nc_compensation_v19_commands_more.c` | 工具長補正、工具径補正、極座標変換、範囲エラー | 4 |
| 3 | `test_nc_coordinate_transform_v19_modes_more.c` | 動的治具補正、取付誤差、回転テーブル補正、API拒否 | 4 |
| 4 | `test_nc_kinematics_v19_commands_more.c` | 軸割付、制御軸取り外し、ミラー、傾斜/工具軸特異点警告 | 4 |
| 5 | `test_nc_cycle_v19_variants_more.c` | G74/G84/G83/G82固定サイクル分岐 | 4 |
| 6 | `test_nc_feed_v19_profiles_more.c` | 早送り、回転送り、ドウェル、RT更新/取消 | 4 |
| 7 | `test_nc_interpolation_v19_runtime_more.c` | 非モーションno-op、開始拒否、直線完了、G31スキップ早期完了 | 4 |
| 8 | `test_nc_reference_v19_markers_more.c` | G28/G30、距離コードマーカ、参照点設定エラー | 4 |
| 9 | `test_rt_control_v19_trace_more.c` | 起動前インターロック、I/Oトレースfreeze、先読み反映、出力ランプ | 4 |
| 10 | `test_control_api_v19_gateways_more.c` | 公開API null拒否、NCロード状態ゲート、開始/保持/再開、軸設定API | 4 |

## 実行方法

### Linux/GCC系

```sh
cd source
scripts/run_tests_v19_gcc.sh
```

### Windows / Visual Studio Developer Shell

```bat
cd source
scripts\run_tests_v19.bat
```

## 確認結果

`source/build/test_results_v19_added.txt` にGCCでの実行ログを保存した。追加40本はすべてPASS。

## 補足

v19は既存テストの置き換えではなく追加テストである。既存のv13〜v18テスト群はZIP内に残しており、必要に応じて個別または一括スクリプトへ統合できる。
