@echo off
REM Builds the v14 additive unit tests with cl.exe-compatible arguments.
setlocal
set ROOT_DIR=%~dp0..
set BUILD_DIR=%ROOT_DIR%\build\tests_v14
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if "%CC%"=="" set CC=cl
set INC=/I"%ROOT_DIR%\inc" /I"%ROOT_DIR%\stubs" /I"%ROOT_DIR%\tests"
set CFLAGS=/nologo /TC /W4 /D_CRT_SECURE_NO_WARNINGS %INC%
%CC% %CFLAGS% "%ROOT_DIR%\tests\test_nc_codes_v14_catalog_more.c" "%ROOT_DIR%\src\nc_codes.c" /Fe"%BUILD_DIR%\test_nc_codes_v14_catalog_more.exe" || exit /b 1
%CC% %CFLAGS% "%ROOT_DIR%\tests\test_nc_feed_v14_rt_update_more.c" "%ROOT_DIR%\src\nc_feed.c" "%ROOT_DIR%\src\system_shared.c" /Fe"%BUILD_DIR%\test_nc_feed_v14_rt_update_more.exe" || exit /b 1
%CC% %CFLAGS% "%ROOT_DIR%\tests\test_nc_coordinate_v14_invalid_and_modes_more.c" "%ROOT_DIR%\src\nc_coordinate.c" "%ROOT_DIR%\src\system_shared.c" /Fe"%BUILD_DIR%\test_nc_coordinate_v14_invalid_and_modes_more.exe" || exit /b 1
%CC% %CFLAGS% "%ROOT_DIR%\tests\test_nc_axis_config_v14_masks_more.c" "%ROOT_DIR%\src\nc_axis_config.c" "%ROOT_DIR%\src\system_shared.c" /Fe"%BUILD_DIR%\test_nc_axis_config_v14_masks_more.exe" || exit /b 1
%CC% %CFLAGS% "%ROOT_DIR%\tests\test_nc_interference_v14_more.c" "%ROOT_DIR%\src\nc_interference.c" "%ROOT_DIR%\src\system_shared.c" /Fe"%BUILD_DIR%\test_nc_interference_v14_more.exe" || exit /b 1
%CC% %CFLAGS% "%ROOT_DIR%\tests\test_nc_reference_v14_more.c" "%ROOT_DIR%\src\nc_reference.c" "%ROOT_DIR%\src\system_shared.c" /Fe"%BUILD_DIR%\test_nc_reference_v14_more.exe" || exit /b 1
%CC% %CFLAGS% "%ROOT_DIR%\tests\test_nc_spindle_v14_more.c" "%ROOT_DIR%\src\nc_spindle.c" "%ROOT_DIR%\src\system_shared.c" /Fe"%BUILD_DIR%\test_nc_spindle_v14_more.exe" || exit /b 1
%CC% %CFLAGS% "%ROOT_DIR%\tests\test_nc_tool_management_v14_more.c" "%ROOT_DIR%\src\nc_tool_management.c" "%ROOT_DIR%\src\system_shared.c" /Fe"%BUILD_DIR%\test_nc_tool_management_v14_more.exe" || exit /b 1
%CC% %CFLAGS% "%ROOT_DIR%\tests\test_nc_motion_filter_v14_endpoint_more.c" "%ROOT_DIR%\src\nc_motion_filter.c" "%ROOT_DIR%\src\system_shared.c" /Fe"%BUILD_DIR%\test_nc_motion_filter_v14_endpoint_more.exe" || exit /b 1
%CC% %CFLAGS% "%ROOT_DIR%\tests\test_control_api_v14_more_getters_setters.c" "%ROOT_DIR%\src\control_api.c" "%ROOT_DIR%\src\nc_design_features.c" "%ROOT_DIR%\src\nc_feature_backlog.c" "%ROOT_DIR%\src\system_shared.c" "%ROOT_DIR%\src\nc_buffer.c" "%ROOT_DIR%\src\nc_motion_filter.c" "%ROOT_DIR%\stubs\stub_external_io.c" "%ROOT_DIR%\stubs\stub_module_deps.c" /Fe"%BUILD_DIR%\test_control_api_v14_more_getters_setters.exe" || exit /b 1
echo [build_tests_v14] Build succeeded.
