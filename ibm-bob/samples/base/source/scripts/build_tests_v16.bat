@echo off
REM Builds the v16 additive unit tests (40 cases / 10 executables) with cl.exe.
setlocal enabledelayedexpansion
if not exist build\tests_v16 mkdir build\tests_v16
set COMMON_FLAGS=/nologo /TC /W4 /D_CRT_SECURE_NO_WARNINGS /Iinc /Istubs /Itests
set ALL_SRC=src\control_api.c src\nc_design_features.c src\nc_feature_backlog.c src\nc_axis_config.c src\nc_buffer.c src\nc_capability.c src\nc_codes.c src\nc_compensation.c src\nc_coordinate.c src\nc_coordinate_transform.c src\nc_cycle.c src\nc_diagnostics.c src\nc_feed.c src\nc_interference.c src\nc_interp_math.c src\nc_interpolation.c src\nc_kinematics.c src\nc_lathe_cycle.c src\nc_lookahead.c src\nc_motion_filter.c src\nc_parser.c src\nc_parser_feature.c src\nc_parser_modal.c src\nc_design_features.c src\nc_feature_backlog.c src\nc_parser_tokens.c src\nc_precision.c src\nc_program.c src\nc_reference.c src\nc_rotary_mcc.c src\nc_safety_motion.c src\nc_spindle.c src\nc_synchronization.c src\nc_tool_management.c src\rt_control.c src\system_shared.c src\ts_service.c
cl %COMMON_FLAGS% tests\test_nc_compensation_v16_transform_more.c %ALL_SRC% /Fe:build\tests_v16\test_nc_compensation_v16_transform_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate_transform_v16_enable_more.c %ALL_SRC% /Fe:build\tests_v16\test_nc_coordinate_transform_v16_enable_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_axis_config_v16_filter_more.c %ALL_SRC% /Fe:build\tests_v16\test_nc_axis_config_v16_filter_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_kinematics_v16_transform_more.c %ALL_SRC% /Fe:build\tests_v16\test_nc_kinematics_v16_transform_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_interp_math_v16_geometry_more.c %ALL_SRC% /Fe:build\tests_v16\test_nc_interp_math_v16_geometry_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_interpolation_v16_step_more.c %ALL_SRC% /Fe:build\tests_v16\test_nc_interpolation_v16_step_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_parser_internal_v16_modal_more.c %ALL_SRC% /Fe:build\tests_v16\test_nc_parser_internal_v16_modal_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_rt_control_v16_state_more.c %ALL_SRC% /Fe:build\tests_v16\test_rt_control_v16_state_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_ts_service_v16_prefetch_ui_more.c %ALL_SRC% /Fe:build\tests_v16\test_ts_service_v16_prefetch_ui_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_control_api_v16_request_more.c %ALL_SRC% /Fe:build\tests_v16\test_control_api_v16_request_more.exe || exit /b 1
echo [build_tests_v16] Build succeeded.
