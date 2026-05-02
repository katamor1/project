# IBM-Bob 追加単体テスト v15 レポート

## 概要

`ibm_bob_source_unit_tests_additional_v14.zip` をベースに、さらに他の関数向けの追加単体テストを **40本** 作成した。

- 追加テストファイル: **10件**
- 追加テストケース: **40本**
- 本体ソースの機能実装は変更せず、テスト、ビルドスクリプト、レポート、manifestのみを追加
- v14で重点化したコード表、送り、座標、軸構成、干渉、リファレンス、主軸、工具管理、モーションフィルタ、APIとは別に、能力記録、先読み、精度制御、同期、旋回MCC、旋盤サイクル、安全動作、診断、NCバッファ、共有メモリ初期化/リングを対象にした

## 追加テスト一覧

| テストファイル | 追加本数 | 主な確認対象 |
|---|---:|---|
| `test_nc_capability_v15_history_more.c` | 4 | capability reset、G/M履歴ラップ、未対応アドレス範囲外、イベント行番号 |
| `test_nc_lookahead_v15_modes_arcs_more.c` | 4 | G08不正P、HPCCキャンセル、exact stop、ヘリカル円弧検出 |
| `test_nc_precision_v15_modes_service_more.c` | 4 | learning window clamp、G08キャンセル、G05 P3、ServiceRt出力クランプ |
| `test_nc_synchronization_v15_modes_following_more.c` | 4 | mode parse計数、G51/G50切替、追従偏差警告、double table計数 |
| `test_nc_rotary_mcc_v15_service_more.c` | 4 | 半径上限、ゼロ回転差分、非motion mode、MCC出力OFF遷移 |
| `test_nc_lathe_cycle_v15_tracking_more.c` | 4 | radius初期値、G70/G94、G70.7研削、thread/circular/variable lead |
| `test_nc_safety_motion_v15_latch_release_more.c` | 4 | reset既定値、外部減速再arm、低override維持、停電時tick非縮小 |
| `test_nc_diagnostics_v15_snapshot_life_more.c` | 4 | 負analog絶対値、工具寿命limit reject、active tool使用加算、snapshot転記 |
| `test_nc_buffer_v15_wrap_validity_more.c` | 4 | Count wrap、full境界、invalid head reject、consume generation |
| `test_system_shared_v15_event_log_more.c` | 4 | 初期化既定値、LogQueue記録、NC event ring wrap、再初期化クリア |

## 実行方法

GCC系環境では以下でv15追加分のみをビルド・実行する。

```sh
cd source
scripts/run_tests_v15_gcc.sh
```

Windows Visual Studio Developer Command Prompt / Developer PowerShell では以下を使用する。

```bat
cd source
scripts\run_tests_v15.bat
```

## 検証結果

この実行環境では、v15で追加した40本についてGCCで個別ビルド・実行し、全件PASSを確認した。結果概要は `build/test_results_v15_added.txt` に保存している。

```text
V15_ADDED_TESTS_TOTAL=40
V15_ADDED_TESTS_PASS=40
```

## 補足

既存テスト一式への破壊的な変更は行っていない。v15追加テストは、既存の `tests/test_common.h`、既存共有メモリ、既存モジュールを使用する加算型の構成である。
