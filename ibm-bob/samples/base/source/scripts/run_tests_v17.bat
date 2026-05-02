@echo off
REM Runs the v17 additive unit tests with cl.exe-built binaries.
setlocal enabledelayedexpansion
set ROOT_DIR=%~dp0..
set BUILD_DIR=%ROOT_DIR%\build\tests_v17
set RESULT_FILE=%ROOT_DIR%\build\test_results_v17_added.txt
if not exist "%ROOT_DIR%\build" mkdir "%ROOT_DIR%\build"
type nul > "%RESULT_FILE%"
for %%E in (^ 
 test_nc_capability_v17_accounting_more^ 
 test_nc_coordinate_v17_modes_more^ 
 test_nc_cycle_v17_modal_segments_more^ 
 test_nc_buffer_v17_ring_more^ 
 test_nc_diagnostics_v17_thresholds_more^ 
 test_nc_safety_motion_v17_sensor_edges^ 
 test_nc_spindle_v17_css_orient_more^ 
 test_nc_synchronization_v17_apply_more^ 
 test_nc_interference_v17_warning_more^ 
 test_system_shared_v17_ring_more) do (
  echo [run] %%E
  echo [run] %%E>>"%RESULT_FILE%"
  "%BUILD_DIR%\%%E.exe" >>"%RESULT_FILE%" || exit /b 1
)
echo [run_tests_v17] All v17 additive tests passed.>>"%RESULT_FILE%"
echo [run_tests_v17] All v17 additive tests passed.
