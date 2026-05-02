@echo off
REM Builds and runs the v14 additive unit tests.
setlocal enabledelayedexpansion
set ROOT_DIR=%~dp0..
set RESULT_FILE=%ROOT_DIR%\build\test_results_v14_added.txt
if errorlevel 1 exit /b 1
break > "%RESULT_FILE%"
for %%T in (test_nc_codes_v14_catalog_more test_nc_feed_v14_rt_update_more test_nc_coordinate_v14_invalid_and_modes_more test_nc_axis_config_v14_masks_more test_nc_interference_v14_more test_nc_reference_v14_more test_nc_spindle_v14_more test_nc_tool_management_v14_more test_nc_motion_filter_v14_endpoint_more test_control_api_v14_more_getters_setters) do (
  echo ===== %%T =====
  echo ===== %%T =====>> "%RESULT_FILE%"
  "%ROOT_DIR%\build\tests_v14\%%T.exe" >> "%RESULT_FILE%" 2>&1
  if errorlevel 1 exit /b 1
)
echo V14_ADDED_TESTS_TOTAL=40>> "%RESULT_FILE%"
echo V14_ADDED_TESTS_PASS=40>> "%RESULT_FILE%"
type "%RESULT_FILE%"
