# IBM-Bob 追加単体テスト v14 レポート

## 概要

`ibm_bob_source_unit_tests_additional_v13.zip` をベースに、他の関数向けの追加単体テストを **40本** 作成しました。

- 追加テストファイル: **10件**
- 追加テストケース: **40本**
- 追加対象: NCコード表、送り制御、座標系、軸構成、干渉監視、リファレンス復帰、主軸、工具管理、モーションフィルタ、Control API
- 本体ソースの機能実装は変更せず、テスト・ビルドスクリプト・レポートのみを追加

## 追加テスト一覧

| テストファイル | 追加本数 | 主な確認対象 |
|---|---:|---|
| `test_nc_codes_v14_catalog_more.c` | 4 | G28/G10.6/G73.7、G90/G91/G98/G99、D/I/Wアドレス、M06/M999/M1000境界 |
| `test_nc_feed_v14_rt_update_more.c` | 4 | Begin後Cancel、NULL更新、ドウェル完了tick、加減速プロファイル比率 |
| `test_nc_coordinate_v14_invalid_and_modes_more.c` | 4 | 不正軸、NULLブロック、単位/距離モード、ローカルシフト解除 |
| `test_nc_axis_config_v14_masks_more.c` | 4 | 不正マスク、制御軸取り外し、経路軸マスク、非移動ブロックno-op |
| `test_nc_interference_v14_more.c` | 4 | enable/disable、disabled no-op、負クリアランス、微小円弧警告 |
| `test_nc_reference_v14_more.c` | 4 | distance-coded marker、G28既定軸、G30、リファレンスマーク立上り検出 |
| `test_nc_spindle_v14_more.c` | 4 | 回転数制限、min/maxクランプ、CCW/停止、オリエント完了/タイムアウト |
| `test_nc_tool_management_v14_more.c` | 4 | pocket解除、T指令prepare、M06即時完了、MFIN待ち完了 |
| `test_nc_motion_filter_v14_endpoint_more.c` | 4 | 不正config、軸制限既定値、disabled apply、終点force snap |
| `test_control_api_v14_more_getters_setters.c` | 4 | 軸構成API、モーションフィルタAPI、不正引数、NULL getter |

## 実行方法

GCC系環境では以下でv14追加分のみをビルド・実行できます。

```sh
cd source
scripts/run_tests_v14_gcc.sh
```

Windows Visual Studio Developer Command Prompt / Developer PowerShell では以下を使用できます。

```bat
cd source
scripts\run_tests_v14.bat
```

## 検証結果

この実行環境では、v14で追加した40本についてGCCで個別ビルド・実行し、全件PASSを確認しました。結果は `build/test_results_v14_added.txt` に保存しています。

```text
V14_ADDED_TESTS_TOTAL=40
V14_ADDED_TESTS_PASS=40
```

## 補足

既存テスト一式への破壊的な変更は行っていません。v14追加テストは、既存の `tests/test_common.h` と既存モジュールを使用する加算型の構成です。
