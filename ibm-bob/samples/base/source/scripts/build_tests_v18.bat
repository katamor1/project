@echo off
REM Builds the v18 additive unit tests (40 cases / 10 executables) with cl.exe.
setlocal
set ROOT_DIR=%~dp0..
set BUILD_DIR=%ROOT_DIR%\build\tests_v18
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
set COMMON_FLAGS=/nologo /TC /W4 /D_CRT_SECURE_NO_WARNINGS /I"%ROOT_DIR%\inc" /I"%ROOT_DIR%\stubs" /I"%ROOT_DIR%\tests"
set SYS="%ROOT_DIR%\src\system_shared.c"
cl %COMMON_FLAGS% "%ROOT_DIR%\tests\test_nc_codes_v18_lookup_more.c" "%ROOT_DIR%\src\nc_codes.c" %SYS% /Fe:"%BUILD_DIR%\test_nc_codes_v18_lookup_more.exe"
cl %COMMON_FLAGS% "%ROOT_DIR%\tests\test_nc_buffer_v18_generation_more.c" "%ROOT_DIR%\src\nc_buffer.c" %SYS% /Fe:"%BUILD_DIR%\test_nc_buffer_v18_generation_more.exe"
cl %COMMON_FLAGS% "%ROOT_DIR%\tests\test_nc_capability_v18_address_history_more.c" "%ROOT_DIR%\src\nc_capability.c" %SYS% /Fe:"%BUILD_DIR%\test_nc_capability_v18_address_history_more.exe"
cl %COMMON_FLAGS% "%ROOT_DIR%\tests\test_nc_motion_filter_v18_disable_limits_more.c" "%ROOT_DIR%\src\nc_motion_filter.c" %SYS% /Fe:"%BUILD_DIR%\test_nc_motion_filter_v18_disable_limits_more.exe"
cl %COMMON_FLAGS% "%ROOT_DIR%\tests\test_nc_precision_v18_learning_service_more.c" "%ROOT_DIR%\src\nc_precision.c" %SYS% /Fe:"%BUILD_DIR%\test_nc_precision_v18_learning_service_more.exe"
cl %COMMON_FLAGS% "%ROOT_DIR%\tests\test_nc_safety_motion_v18_priority_more.c" "%ROOT_DIR%\src\nc_safety_motion.c" %SYS% /Fe:"%BUILD_DIR%\test_nc_safety_motion_v18_priority_more.exe"
cl %COMMON_FLAGS% "%ROOT_DIR%\tests\test_nc_rotary_mcc_v18_output_more.c" "%ROOT_DIR%\src\nc_rotary_mcc.c" %SYS% /Fe:"%BUILD_DIR%\test_nc_rotary_mcc_v18_output_more.exe"
cl %COMMON_FLAGS% "%ROOT_DIR%\tests\test_nc_spindle_v18_parse_sync_more.c" "%ROOT_DIR%\src\nc_spindle.c" %SYS% /Fe:"%BUILD_DIR%\test_nc_spindle_v18_parse_sync_more.exe"
cl %COMMON_FLAGS% "%ROOT_DIR%\tests\test_nc_coordinate_transform_v18_apply_more.c" "%ROOT_DIR%\src\nc_coordinate_transform.c" "%ROOT_DIR%\src\nc_coordinate.c" %SYS% /Fe:"%BUILD_DIR%\test_nc_coordinate_transform_v18_apply_more.exe"
cl %COMMON_FLAGS% "%ROOT_DIR%\tests\test_nc_parser_v18_modal_errors_more.c" "%ROOT_DIR%\src\nc_parser.c" "%ROOT_DIR%\src\nc_parser_modal.c" "%ROOT_DIR%\src\nc_design_features.c" "%ROOT_DIR%\src\nc_feature_backlog.c" "%ROOT_DIR%\src\nc_parser_tokens.c" "%ROOT_DIR%\src\nc_parser_feature.c" "%ROOT_DIR%\src\nc_codes.c" "%ROOT_DIR%\src\nc_coordinate.c" "%ROOT_DIR%\src\nc_coordinate_transform.c" "%ROOT_DIR%\src\nc_cycle.c" "%ROOT_DIR%\src\nc_interpolation.c" "%ROOT_DIR%\src\nc_feed.c" "%ROOT_DIR%\src\nc_motion_filter.c" "%ROOT_DIR%\src\nc_buffer.c" "%ROOT_DIR%\src\nc_compensation.c" "%ROOT_DIR%\src\nc_kinematics.c" "%ROOT_DIR%\src\nc_axis_config.c" "%ROOT_DIR%\src\nc_rotary_mcc.c" "%ROOT_DIR%\src\nc_interp_math.c" "%ROOT_DIR%\src\nc_lookahead.c" "%ROOT_DIR%\src\nc_capability.c" %SYS% /Fe:"%BUILD_DIR%\test_nc_parser_v18_modal_errors_more.exe"
echo [build_tests_v18] Build succeeded.
endlocal
