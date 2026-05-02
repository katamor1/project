# IBM-Bob 単体テスト追加 v18 レポート

## 概要

v17に続き、他の関数・別分岐を対象に単体テストを40本追加した。
既存テストの焼き直しを避けるため、NCコード表、NCバッファ、能力記録、モーションフィルタ、精度制御、安全動作、旋回MCC、主軸、座標変換、NCパーサの境界条件と副作用確認を中心にした。

## 追加テスト一覧

| No | テストファイル | 対象 | 追加ケース数 |
|---:|---|---|---:|
| 1 | `test_nc_codes_v18_lookup_more.c` | G/M/アドレスコード表の小数Gコード、lowercase正規化、Mコード境界 | 4 |
| 2 | `test_nc_buffer_v18_generation_more.c` | NC実行バッファのcommit/peek/consume、full、clear、generation | 4 |
| 3 | `test_nc_capability_v18_address_history_more.c` | capabilityのアドレスmask、G/M履歴wrap、未対応記録 | 4 |
| 4 | `test_nc_motion_filter_v18_disable_limits_more.c` | モーションフィルタのdisable、limit正規化、window clamp、endpoint補正 | 4 |
| 5 | `test_nc_precision_v18_learning_service_more.c` | 精度制御のsetter範囲、G08 P2、learning補正、RT service | 4 |
| 6 | `test_nc_safety_motion_v18_priority_more.c` | 外部減速、停電減速優先、位置スイッチラッチ、block適用 | 4 |
| 7 | `test_nc_rotary_mcc_v18_output_more.c` | 旋回MCCのradius設定、mode ON/OFF、仮想移動量、出力条件 | 4 |
| 8 | `test_nc_spindle_v18_parse_sync_more.c` | 主軸のTS解析、M03/M04方向変更、ねじ/タップ同期、orient完了 | 4 |
| 9 | `test_nc_coordinate_transform_v18_apply_more.c` | 座標変換の有効/無効、API拒否、補正合成、null/no-op | 4 |
| 10 | `test_nc_parser_v18_modal_errors_more.c` | パーサのinch換算、modal conflict、G53座標エラー、token/unsupported | 4 |

## 実行方法

### GCC / clang系

```sh
cd source
scripts/run_tests_v18_gcc.sh
```

### Visual Studio cl.exe

```bat
cd source
scriptsun_tests_v18.bat
```

## 確認結果

`source/scripts/run_tests_v18_gcc.sh` により、追加分10実行ファイル・40テストケースをビルド・実行し、全件PASSを確認した。

結果ログ: `source/build/test_results_v18_added.txt`

## 補足

v18では、テスト済み関数そのものを単純に再確認するのではなく、既存テストでは薄かった入力正規化、generation更新、リング境界、状態ラッチ、RT service副作用、モード競合、null/no-op経路を追加で確認している。
