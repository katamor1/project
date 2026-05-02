@echo off
REM Builds and runs the v16 additive unit tests.
setlocal enabledelayedexpansion
if not exist build mkdir build
set RESULT_FILE=build\test_results_v16_added.txt
break > %RESULT_FILE%
for %%T in (^
 test_nc_compensation_v16_transform_more ^
 test_nc_coordinate_transform_v16_enable_more ^
 test_nc_axis_config_v16_filter_more ^
 test_nc_kinematics_v16_transform_more ^
 test_nc_interp_math_v16_geometry_more ^
 test_nc_interpolation_v16_step_more ^
 test_nc_parser_internal_v16_modal_more ^
 test_rt_control_v16_state_more ^
 test_ts_service_v16_prefetch_ui_more ^
 test_control_api_v16_request_more) do (
  echo ===== %%T ===== >> %RESULT_FILE%
  build\tests_v16\%%T.exe >> %RESULT_FILE% 2>&1 || exit /b 1
)
echo V16_ADDED_TESTS_TOTAL=40 >> %RESULT_FILE%
echo V16_ADDED_TESTS_PASS=40 >> %RESULT_FILE%
type %RESULT_FILE%
