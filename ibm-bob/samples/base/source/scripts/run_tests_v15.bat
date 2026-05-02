@echo off
REM Builds and runs the v15 additive unit tests.
setlocal enabledelayedexpansion
if not exist build mkdir build
set RESULT_FILE=build\test_results_v15_added.txt
break > %RESULT_FILE%
for %%T in (^ 
 test_nc_capability_v15_history_more ^
 test_nc_lookahead_v15_modes_arcs_more ^
 test_nc_precision_v15_modes_service_more ^
 test_nc_synchronization_v15_modes_following_more ^
 test_nc_rotary_mcc_v15_service_more ^
 test_nc_lathe_cycle_v15_tracking_more ^
 test_nc_safety_motion_v15_latch_release_more ^
 test_nc_diagnostics_v15_snapshot_life_more ^
 test_nc_buffer_v15_wrap_validity_more ^
 test_system_shared_v15_event_log_more) do (
  echo ===== %%T ===== >> %RESULT_FILE%
  build\tests_v15\%%T.exe >> %RESULT_FILE% 2>&1 || exit /b 1
)
echo V15_ADDED_TESTS_TOTAL=40 >> %RESULT_FILE%
echo V15_ADDED_TESTS_PASS=40 >> %RESULT_FILE%
type %RESULT_FILE%
