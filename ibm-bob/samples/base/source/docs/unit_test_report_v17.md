# IBM-Bob 追加単体テスト v17 レポート

## 概要

v17では、v16までで薄かった別関数・別分岐を中心に、追加単体テスト40本を作成した。
対象は、NC能力記録、座標系、固定サイクル、NCバッファ、診断、安全動作、主軸、同期、干渉監視、共有リング/初期化である。

## 追加テスト一覧

| No | テストファイル | 追加ケース数 | 主な対象 |
|---:|---|---:|---|
| 1 | `test_nc_capability_v17_accounting_more.c` | 4 | `NcCapability_OnParsedBlockTs`, `NcCapability_RecordUnsupported`, `NcCapability_Reset` |
| 2 | `test_nc_coordinate_v17_modes_more.c` | 4 | `NcCoordinate_SelectWork`, `NcCoordinate_SetLocalShiftAxis`, `NcCoordinate_SetTemporaryAxis`, `NcCoordinate_FillTargets` |
| 3 | `test_nc_cycle_v17_modal_segments_more.c` | 4 | `NcCycle_ApplyCycleGCode`, `NcCycle_SetRepeatWord`, `NcCycle_FinalizeParserBlock`, `NcCycle_BuildSegmentPlanRt` |
| 4 | `test_nc_buffer_v17_ring_more.c` | 4 | `NcProgram_ClearBuffer`, `NcBuffer_CommitBlock`, `NcBuffer_PeekHead`, `NcBuffer_ConsumeHead`, `NcBuffer_IsFull` |
| 5 | `test_nc_diagnostics_v17_thresholds_more.c` | 4 | `NcDiagnostics_UpdateAxisLoadRt`, `NcDiagnostics_SetToolLifeLimit`, `NcDiagnostics_OnBlockRt`, `NcDiagnostics_BuildSnapshot` |
| 6 | `test_nc_safety_motion_v17_sensor_edges.c` | 4 | `NcSafetyMotion_UpdateRt`, `NcSafetyMotion_ApplyBlockRt` |
| 7 | `test_nc_spindle_v17_css_orient_more.c` | 4 | `NcSpindle_SetSpeedLimit`, `NcSpindle_OnBlockRt`, `NcSpindle_ServiceRt` |
| 8 | `test_nc_synchronization_v17_apply_more.c` | 4 | `NcSynchronization_SetMasterSlave`, `NcSynchronization_OnParsedBlockTs`, `NcSynchronization_ApplyBlockRt`, `NcSynchronization_SetOverlayAxis` |
| 9 | `test_nc_interference_v17_warning_more.c` | 4 | `NcInterference_SetEnabled`, `NcInterference_CheckBlockTs` |
| 10 | `test_system_shared_v17_ring_more.c` | 4 | `SystemShared_Initialize`, `LogQueue_Push`, `NcEvent_Push` |

合計: **40ケース**

## 追加・更新ファイル

- `source/tests/test_nc_capability_v17_accounting_more.c`
- `source/tests/test_nc_coordinate_v17_modes_more.c`
- `source/tests/test_nc_cycle_v17_modal_segments_more.c`
- `source/tests/test_nc_buffer_v17_ring_more.c`
- `source/tests/test_nc_diagnostics_v17_thresholds_more.c`
- `source/tests/test_nc_safety_motion_v17_sensor_edges.c`
- `source/tests/test_nc_spindle_v17_css_orient_more.c`
- `source/tests/test_nc_synchronization_v17_apply_more.c`
- `source/tests/test_nc_interference_v17_warning_more.c`
- `source/tests/test_system_shared_v17_ring_more.c`
- `source/scripts/build_tests_v17_gcc.sh`
- `source/scripts/run_tests_v17_gcc.sh`
- `source/scripts/build_tests_v17.bat`
- `source/scripts/run_tests_v17.bat`
- `source/build/test_results_v17_added.txt`
- `source/docs/manifest_v17_unit_tests.json`
- `source/docs/unit_test_report_v17.md`

## 実行結果

GCC環境でv17追加分40本をビルド・実行し、全件PASSを確認した。

```text
[run_tests_v17_gcc] All v17 additive tests passed.
```

## Windows / Visual Studio での実行方法

```bat
cd source
scripts\run_tests_v17.bat
```

## GCCでの実行方法

```sh
cd source
scripts/run_tests_v17_gcc.sh
```
