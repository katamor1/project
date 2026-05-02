@REM scripts/build_tests.bat
@echo off
setlocal EnableExtensions
cd /d "%~dp0.."
if not exist build mkdir build
if not exist build\tests mkdir build\tests
set "VS_DEV_SHELL=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1"
set "COMMON_FLAGS=/nologo /TC /W4 /D_CRT_SECURE_NO_WARNINGS /Iinc /Istubs /Itests"
where cl.exe >nul 2>nul
if not errorlevel 1 goto :compile
if not exist "%VS_DEV_SHELL%" (
  echo ERROR: cl.exe was not found in PATH.
  echo Hint: open a Visual Studio Developer Command Prompt or adjust VS_DEV_SHELL in scripts\build_tests.bat.
  exit /b 1
)
powershell -NoProfile -ExecutionPolicy Bypass -Command "& '%VS_DEV_SHELL%' -Arch amd64 -HostArch amd64; Set-Location '%CD%'; cmd /c scripts\build_tests.bat"
powershell -NoProfile -ExecutionPolicy Bypass -Command "& '%VS_DEV_SHELL%' -Arch amd64 -HostArch amd64; Set-Location '%CD%'; cmd /c scripts\build_tests_v14.bat"
powershell -NoProfile -ExecutionPolicy Bypass -Command "& '%VS_DEV_SHELL%' -Arch amd64 -HostArch amd64; Set-Location '%CD%'; cmd /c scripts\build_tests_v15.bat"
powershell -NoProfile -ExecutionPolicy Bypass -Command "& '%VS_DEV_SHELL%' -Arch amd64 -HostArch amd64; Set-Location '%CD%'; cmd /c scripts\build_tests_v16.bat"
powershell -NoProfile -ExecutionPolicy Bypass -Command "& '%VS_DEV_SHELL%' -Arch amd64 -HostArch amd64; Set-Location '%CD%'; cmd /c scripts\build_tests_v17.bat"
powershell -NoProfile -ExecutionPolicy Bypass -Command "& '%VS_DEV_SHELL%' -Arch amd64 -HostArch amd64; Set-Location '%CD%'; cmd /c scripts\build_tests_v18.bat"
powershell -NoProfile -ExecutionPolicy Bypass -Command "& '%VS_DEV_SHELL%' -Arch amd64 -HostArch amd64; Set-Location '%CD%'; cmd /c scripts\build_tests_v19.bat"
exit /b %ERRORLEVEL%
:compile
echo [build_tests] Building unit tests...
cl %COMMON_FLAGS% tests\test_control_api.c src\control_api.c src\nc_design_features.c src\nc_feature_backlog.c src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c stubs\stub_module_deps.c /Fe:build\tests\test_control_api.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_buffer.c src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_buffer.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_diagnostics.c src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c src\nc_diagnostics.c stubs\stub_external_io.c /Fe:build\tests\test_nc_diagnostics.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_motion_filter.c src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_motion_filter.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_codes.c src\nc_codes.c /Fe:build\tests\test_nc_codes.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_feed.c src\system_shared.c src\nc_feed.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_feed.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate.c src\system_shared.c src\nc_coordinate.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_coordinate.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate_transform.c src\system_shared.c src\nc_coordinate.c src\nc_coordinate_transform.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_coordinate_transform.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_axis_config.c src\system_shared.c src\nc_axis_config.c src\nc_design_features.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_axis_config.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_rotary_mcc.c src\system_shared.c src\nc_rotary_mcc.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_rotary_mcc.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_capability.c src\system_shared.c src\nc_capability.c /Fe:build\tests\test_nc_capability.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_interference.c src\system_shared.c src\nc_interference.c /Fe:build\tests\test_nc_interference.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_safety_motion.c src\system_shared.c src\nc_safety_motion.c /Fe:build\tests\test_nc_safety_motion.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_tool_management.c src\system_shared.c src\nc_tool_management.c /Fe:build\tests\test_nc_tool_management.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_spindle.c src\system_shared.c src\nc_spindle.c /Fe:build\tests\test_nc_spindle.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_precision.c src\system_shared.c src\nc_precision.c /Fe:build\tests\test_nc_precision.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_synchronization.c src\system_shared.c src\nc_synchronization.c /Fe:build\tests\test_nc_synchronization.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_compensation.c src\system_shared.c src\nc_compensation.c /Fe:build\tests\test_nc_compensation.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_cycle.c src\system_shared.c src\nc_cycle.c /Fe:build\tests\test_nc_cycle.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_reference.c src\system_shared.c src\nc_reference.c /Fe:build\tests\test_nc_reference.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_lathe_cycle.c src\system_shared.c src\nc_lathe_cycle.c /Fe:build\tests\test_nc_lathe_cycle.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_kinematics.c src\system_shared.c src\nc_kinematics.c /Fe:build\tests\test_nc_kinematics.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_interp_math.c src\system_shared.c src\nc_interp_math.c /Fe:build\tests\test_nc_interp_math.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_system_shared.c src\system_shared.c /Fe:build\tests\test_system_shared.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_ts_service.c src\system_shared.c src\ts_service.c stubs\stub_ts_service_deps.c /Fe:build\tests\test_ts_service.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_rt_control.c src\system_shared.c src\rt_control.c stubs\stub_rt_control_deps.c /Fe:build\tests\test_rt_control.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_lookahead.c src\system_shared.c src\nc_lookahead.c src\nc_interp_math.c /Fe:build\tests\test_nc_lookahead.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_interpolation.c src\system_shared.c src\nc_interpolation.c src\nc_feed.c src\nc_cycle.c src\nc_interp_math.c src\nc_motion_filter.c src\nc_buffer.c stubs\stub_external_io.c /Fe:build\tests\test_nc_interpolation.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_parser.c src\system_shared.c src\nc_parser.c src\nc_parser_modal.c src\nc_design_features.c src\nc_feature_backlog.c src\nc_parser_tokens.c src\nc_parser_feature.c src\nc_codes.c src\nc_coordinate.c src\nc_coordinate_transform.c src\nc_cycle.c src\nc_interpolation.c src\nc_feed.c src\nc_motion_filter.c src\nc_buffer.c src\nc_compensation.c src\nc_kinematics.c src\nc_axis_config.c src\nc_design_features.c src\nc_rotary_mcc.c src\nc_interp_math.c src\nc_lookahead.c src\nc_capability.c /Fe:build\tests\test_nc_parser.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_control_api_extended.c src\control_api.c src\nc_design_features.c src\nc_feature_backlog.c src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c stubs\stub_module_deps.c /Fe:build\tests\test_control_api_extended.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_parser_modal_direct.c src\system_shared.c src\nc_parser_modal.c src\nc_design_features.c src\nc_feature_backlog.c src\nc_codes.c stubs\stub_parser_deps.c /Fe:build\tests\test_nc_parser_modal_direct.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_parser_tokens_direct.c src\system_shared.c src\nc_parser_tokens.c src\nc_parser_modal.c src\nc_design_features.c src\nc_feature_backlog.c src\nc_codes.c stubs\stub_parser_deps.c /Fe:build\tests\test_nc_parser_tokens_direct.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_program_binary_flow.c src\system_shared.c src\nc_program.c src\nc_design_features.c src\nc_feature_backlog.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_nc_program_deps.c /Fe:build\tests\test_nc_program_binary_flow.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_parser_feature_direct.c src\system_shared.c src\nc_parser_feature.c /Fe:build\tests\test_nc_parser_feature_direct.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_codes_extended.c src\nc_codes.c /Fe:build\tests\test_nc_codes_extended.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_program_feature_status.c src\system_shared.c src\nc_program.c src\nc_design_features.c src\nc_feature_backlog.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_nc_program_deps.c /Fe:build\tests\test_nc_program_feature_status.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate_transform_direct.c src\system_shared.c src\nc_coordinate.c src\nc_coordinate_transform.c /Fe:build\tests\test_nc_coordinate_transform_direct.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_safety_motion_edges.c src\system_shared.c src\nc_safety_motion.c /Fe:build\tests\test_nc_safety_motion_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_reference_edges.c src\system_shared.c src\nc_reference.c /Fe:build\tests\test_nc_reference_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_tool_management_timeout.c src\system_shared.c src\nc_tool_management.c /Fe:build\tests\test_nc_tool_management_timeout.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_precision_edges.c src\system_shared.c src\nc_precision.c /Fe:build\tests\test_nc_precision_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_feed_profile_boundaries.c src\system_shared.c src\nc_feed.c /Fe:build\tests\test_nc_feed_profile_boundaries.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_cycle_segment_variants.c src\system_shared.c src\nc_cycle.c /Fe:build\tests\test_nc_cycle_segment_variants.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_capability_history_wrap.c src\system_shared.c src\nc_capability.c /Fe:build\tests\test_nc_capability_history_wrap.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_interference_warning_edges.c src\system_shared.c src\nc_interference.c /Fe:build\tests\test_nc_interference_warning_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_spindle_orientation_edges.c src\system_shared.c src\nc_spindle.c /Fe:build\tests\test_nc_spindle_orientation_edges.exe
if errorlevel 1 exit /b 1
echo [build_tests] Additional v7 build succeeded.
cl %COMMON_FLAGS% tests\test_nc_compensation_rotation_polar_edges.c src\system_shared.c src\nc_compensation.c /Fe:build\tests\test_nc_compensation_rotation_polar_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_interp_math_arc_r_edges.c src\system_shared.c src\nc_interp_math.c /Fe:build\tests\test_nc_interp_math_arc_r_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_kinematics_detach_tilt_cancel.c src\system_shared.c src\nc_kinematics.c /Fe:build\tests\test_nc_kinematics_detach_tilt_cancel.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_synchronization_cancel_double_table.c src\system_shared.c src\nc_synchronization.c /Fe:build\tests\test_nc_synchronization_cancel_double_table.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_lathe_cycle_variants.c src\system_shared.c src\nc_lathe_cycle.c /Fe:build\tests\test_nc_lathe_cycle_variants.exe
if errorlevel 1 exit /b 1
echo [build_tests] Additional v8 build succeeded.
REM Additional v10 unit tests
cl %COMMON_FLAGS% tests\test_nc_coordinate_program_machine_edges.c src\system_shared.c src\nc_coordinate.c /Fe:build\tests\test_nc_coordinate_program_machine_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_buffer_generation_validity_edges.c src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_buffer_generation_validity_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_axis_config_apply_edges.c src\system_shared.c src\nc_axis_config.c src\nc_design_features.c /Fe:build\tests\test_nc_axis_config_apply_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate_transform_apply_edges.c src\system_shared.c src\nc_coordinate.c src\nc_coordinate_transform.c /Fe:build\tests\test_nc_coordinate_transform_apply_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_system_shared_event_log_more_edges.c src\system_shared.c /Fe:build\tests\test_system_shared_event_log_more_edges.exe
if errorlevel 1 exit /b 1
echo [build_tests] Additional v10 build succeeded.
REM Additional v9 unit tests
cl %COMMON_FLAGS% tests\test_control_api_remaining_setters.c src\control_api.c src\nc_design_features.c src\nc_feature_backlog.c src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c stubs\stub_module_deps.c /Fe:build\tests\test_control_api_remaining_setters.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_program_request_edges.c src\system_shared.c src\nc_program.c src\nc_design_features.c src\nc_feature_backlog.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_nc_program_deps.c /Fe:build\tests\test_nc_program_request_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_rt_control_dispatch_edges.c src\system_shared.c src\rt_control.c stubs\stub_rt_control_deps.c /Fe:build\tests\test_rt_control_dispatch_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_ts_service_log_and_slice_edges.c src\system_shared.c src\ts_service.c stubs\stub_ts_service_deps.c /Fe:build\tests\test_ts_service_log_and_slice_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_motion_filter_multiaxis_accel_edges.c src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_motion_filter_multiaxis_accel_edges.exe
if errorlevel 1 exit /b 1
echo [build_tests] Additional v9 build succeeded.
REM Additional v11 unit tests
cl %COMMON_FLAGS% tests\test_nc_feed_per_rev_and_error_edges.c src\system_shared.c src\nc_feed.c /Fe:build\tests\test_nc_feed_per_rev_and_error_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_compensation_offset_cancel_edges.c src\system_shared.c src\nc_compensation.c /Fe:build\tests\test_nc_compensation_offset_cancel_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_tool_management_duplicate_and_mfin_edges.c src\system_shared.c src\nc_tool_management.c /Fe:build\tests\test_nc_tool_management_duplicate_and_mfin_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate_selection_clear_edges.c src\system_shared.c src\nc_coordinate.c /Fe:build\tests\test_nc_coordinate_selection_clear_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_codes_decimal_and_address_edges.c src\nc_codes.c /Fe:build\tests\test_nc_codes_decimal_and_address_edges.exe
if errorlevel 1 exit /b 1
echo [build_tests] Additional v11 build succeeded.

REM Additional v12 unit tests
cl %COMMON_FLAGS% tests\test_nc_capability_null_and_address_more.c src\system_shared.c src\nc_capability.c /Fe:build\tests\test_nc_capability_null_and_address_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_rotary_mcc_invalid_and_noop_edges.c src\system_shared.c src\nc_rotary_mcc.c src\nc_codes.c /Fe:build\tests\test_nc_rotary_mcc_invalid_and_noop_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_parser_public_line_more_edges.c src\system_shared.c src\nc_parser.c src\nc_parser_modal.c src\nc_design_features.c src\nc_feature_backlog.c src\nc_parser_tokens.c src\nc_parser_feature.c src\nc_codes.c src\nc_coordinate.c src\nc_coordinate_transform.c src\nc_cycle.c src\nc_interpolation.c src\nc_feed.c src\nc_motion_filter.c src\nc_buffer.c src\nc_compensation.c src\nc_kinematics.c src\nc_axis_config.c src\nc_design_features.c src\nc_rotary_mcc.c src\nc_interp_math.c src\nc_lookahead.c src\nc_capability.c /Fe:build\tests\test_nc_parser_public_line_more_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_interpolation_begin_cancel_more_edges.c src\system_shared.c src\nc_interpolation.c src\nc_feed.c src\nc_cycle.c src\nc_interp_math.c src\nc_motion_filter.c src\nc_buffer.c stubs\stub_external_io.c /Fe:build\tests\test_nc_interpolation_begin_cancel_more_edges.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_system_shared_defaults_reinit_edges.c src\system_shared.c /Fe:build\tests\test_system_shared_defaults_reinit_edges.exe
if errorlevel 1 exit /b 1
echo [build_tests] Additional v12 build succeeded.
REM Additional v13 unit tests - 20 files
cl %COMMON_FLAGS% tests\test_nc_axis_config_definition_more.c src\system_shared.c src\nc_axis_config.c src\nc_design_features.c /Fe:build\tests\test_nc_axis_config_definition_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_axis_config_apply_path_filter_more.c src\system_shared.c src\nc_axis_config.c src\nc_design_features.c /Fe:build\tests\test_nc_axis_config_apply_path_filter_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_axis_config_diameter_rounding_more.c src\system_shared.c src\nc_axis_config.c src\nc_design_features.c /Fe:build\tests\test_nc_axis_config_diameter_rounding_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate_work_offset_more.c src\system_shared.c src\nc_coordinate.c /Fe:build\tests\test_nc_coordinate_work_offset_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate_machine_inverse_more.c src\system_shared.c src\nc_coordinate.c /Fe:build\tests\test_nc_coordinate_machine_inverse_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate_fill_targets_more.c src\system_shared.c src\nc_coordinate.c /Fe:build\tests\test_nc_coordinate_fill_targets_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate_transform_enable_more.c src\system_shared.c src\nc_coordinate.c src\nc_coordinate_transform.c /Fe:build\tests\test_nc_coordinate_transform_enable_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate_transform_apply_combo_more.c src\system_shared.c src\nc_coordinate.c src\nc_coordinate_transform.c /Fe:build\tests\test_nc_coordinate_transform_apply_combo_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_cycle_modal_replay_more.c src\system_shared.c src\nc_cycle.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_cycle_modal_replay_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_cycle_segment_guards_more.c src\system_shared.c src\nc_cycle.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_cycle_segment_guards_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_feed_prepare_motion_zero_override_more.c src\system_shared.c src\nc_feed.c /Fe:build\tests\test_nc_feed_prepare_motion_zero_override_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_feed_prepare_dwell_bounds_more.c src\system_shared.c src\nc_feed.c /Fe:build\tests\test_nc_feed_prepare_dwell_bounds_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_interference_check_axis_limit_more.c src\system_shared.c src\nc_interference.c /Fe:build\tests\test_nc_interference_check_axis_limit_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_lookahead_null_and_g08_more.c src\system_shared.c src\nc_lookahead.c src\nc_interp_math.c /Fe:build\tests\test_nc_lookahead_null_and_g08_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_precision_setters_valid_invalid_more.c src\system_shared.c src\nc_precision.c /Fe:build\tests\test_nc_precision_setters_valid_invalid_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_spindle_css_stop_more.c src\system_shared.c src\nc_spindle.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_spindle_css_stop_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_tool_management_prepare_invalid_more.c src\system_shared.c src\nc_tool_management.c /Fe:build\tests\test_nc_tool_management_prepare_invalid_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_synchronization_overlay_bounds_more.c src\system_shared.c src\nc_synchronization.c /Fe:build\tests\test_nc_synchronization_overlay_bounds_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_reference_one_direction_more.c src\system_shared.c src\nc_reference.c /Fe:build\tests\test_nc_reference_one_direction_more.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_rotary_mcc_output_service_more.c src\system_shared.c src\nc_rotary_mcc.c /Fe:build\tests\test_nc_rotary_mcc_output_service_more.exe
if errorlevel 1 exit /b 1
echo [build_tests] Additional v13 build succeeded.
echo [build_tests] Build succeeded.
exit /b 0
