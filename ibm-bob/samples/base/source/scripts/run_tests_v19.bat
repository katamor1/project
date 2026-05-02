@echo off
REM Runs the v19 additive unit tests (40 cases / 10 executables) with cl.exe outputs.
setlocal
set ROOT_DIR=%~dp0..
set BUILD_DIR=%ROOT_DIR%\build\tests_v19
set RESULT_FILE=%ROOT_DIR%\build\test_results_v19_added.txt
if exist "%RESULT_FILE%" del "%RESULT_FILE%"
for %%E in (
  test_nc_axis_config_v19_apply_more.exe
  test_nc_compensation_v19_commands_more.exe
  test_nc_coordinate_transform_v19_modes_more.exe
  test_nc_kinematics_v19_commands_more.exe
  test_nc_cycle_v19_variants_more.exe
  test_nc_feed_v19_profiles_more.exe
  test_nc_interpolation_v19_runtime_more.exe
  test_nc_reference_v19_markers_more.exe
  test_rt_control_v19_trace_more.exe
  test_control_api_v19_gateways_more.exe
) do (
  echo [run] %%E
  echo [run] %%E >> "%RESULT_FILE%"
  "%BUILD_DIR%\%%E" >> "%RESULT_FILE%" 2>&1 || exit /b 1
)
echo [run_tests_v19] All v19 additive tests passed.>> "%RESULT_FILE%"
echo [run_tests_v19] All v19 additive tests passed.
endlocal
