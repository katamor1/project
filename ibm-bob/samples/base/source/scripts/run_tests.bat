@REM scripts/run_tests.bat
@echo off
setlocal EnableExtensions
cd /d "%~dp0.."
call scripts\build_tests.bat
if errorlevel 1 exit /b 1
if not exist build mkdir build
set "RESULT=build\test_results.txt"
type nul > "%RESULT%"
set FAIL=0
echo ===== test_control_api ===== >> "%RESULT%"
build\tests\test_control_api.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_buffer ===== >> "%RESULT%"
build\tests\test_nc_buffer.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_diagnostics ===== >> "%RESULT%"
build\tests\test_nc_diagnostics.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_motion_filter ===== >> "%RESULT%"
build\tests\test_nc_motion_filter.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_codes ===== >> "%RESULT%"
build\tests\test_nc_codes.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_feed ===== >> "%RESULT%"
build\tests\test_nc_feed.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_coordinate ===== >> "%RESULT%"
build\tests\test_nc_coordinate.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_coordinate_transform ===== >> "%RESULT%"
build\tests\test_nc_coordinate_transform.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_axis_config ===== >> "%RESULT%"
build\tests\test_nc_axis_config.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_rotary_mcc ===== >> "%RESULT%"
build\tests\test_nc_rotary_mcc.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_capability ===== >> "%RESULT%"
build\tests\test_nc_capability.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_interference ===== >> "%RESULT%"
build\tests\test_nc_interference.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_safety_motion ===== >> "%RESULT%"
build\tests\test_nc_safety_motion.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_tool_management ===== >> "%RESULT%"
build\tests\test_nc_tool_management.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_spindle ===== >> "%RESULT%"
build\tests\test_nc_spindle.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_precision ===== >> "%RESULT%"
build\tests\test_nc_precision.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_synchronization ===== >> "%RESULT%"
build\tests\test_nc_synchronization.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_compensation ===== >> "%RESULT%"
build\tests\test_nc_compensation.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_cycle ===== >> "%RESULT%"
build\tests\test_nc_cycle.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_reference ===== >> "%RESULT%"
build\tests\test_nc_reference.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_lathe_cycle ===== >> "%RESULT%"
build\tests\test_nc_lathe_cycle.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_kinematics ===== >> "%RESULT%"
build\tests\test_nc_kinematics.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_interp_math ===== >> "%RESULT%"
build\tests\test_nc_interp_math.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_system_shared ===== >> "%RESULT%"
build\tests\test_system_shared.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_ts_service ===== >> "%RESULT%"
build\tests\test_ts_service.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_rt_control ===== >> "%RESULT%"
build\tests\test_rt_control.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_lookahead ===== >> "%RESULT%"
build\tests\test_nc_lookahead.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_interpolation ===== >> "%RESULT%"
build\tests\test_nc_interpolation.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_parser ===== >> "%RESULT%"
build\tests\test_nc_parser.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_control_api_extended ===== >> "%RESULT%"
build\tests\test_control_api_extended.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_parser_modal_direct ===== >> "%RESULT%"
build\tests\test_nc_parser_modal_direct.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_parser_tokens_direct ===== >> "%RESULT%"
build\tests\test_nc_parser_tokens_direct.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_program_binary_flow ===== >> "%RESULT%"
build\tests\test_nc_program_binary_flow.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_parser_feature_direct ===== >> "%RESULT%"
build\tests\test_nc_parser_feature_direct.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_codes_extended ===== >> "%RESULT%"
build\tests\test_nc_codes_extended.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_program_feature_status ===== >> "%RESULT%"
build\tests\test_nc_program_feature_status.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_coordinate_transform_direct ===== >> "%RESULT%"
build\tests\test_nc_coordinate_transform_direct.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_safety_motion_edges ===== >> "%RESULT%"
build\tests\test_nc_safety_motion_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_reference_edges ===== >> "%RESULT%"
build\tests\test_nc_reference_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_tool_management_timeout ===== >> "%RESULT%"
build\tests\test_nc_tool_management_timeout.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_precision_edges ===== >> "%RESULT%"
build\tests\test_nc_precision_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo. >> "%RESULT%"
echo ===== test_nc_feed_profile_boundaries ===== >> "%RESULT%"
build\tests\test_nc_feed_profile_boundaries.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_cycle_segment_variants ===== >> "%RESULT%"
build\tests\test_nc_cycle_segment_variants.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_capability_history_wrap ===== >> "%RESULT%"
build\tests\test_nc_capability_history_wrap.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_interference_warning_edges ===== >> "%RESULT%"
build\tests\test_nc_interference_warning_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_spindle_orientation_edges ===== >> "%RESULT%"
build\tests\test_nc_spindle_orientation_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo ===== test_nc_compensation_rotation_polar_edges ===== >> "%RESULT%"
build\tests\test_nc_compensation_rotation_polar_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_interp_math_arc_r_edges ===== >> "%RESULT%"
build\tests\test_nc_interp_math_arc_r_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_kinematics_detach_tilt_cancel ===== >> "%RESULT%"
build\tests\test_nc_kinematics_detach_tilt_cancel.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_synchronization_cancel_double_table ===== >> "%RESULT%"
build\tests\test_nc_synchronization_cancel_double_table.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_lathe_cycle_variants ===== >> "%RESULT%"
build\tests\test_nc_lathe_cycle_variants.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
REM Additional v10 unit tests
echo ===== test_nc_coordinate_program_machine_edges ===== >> "%RESULT%"
build\tests\test_nc_coordinate_program_machine_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_buffer_generation_validity_edges ===== >> "%RESULT%"
build\tests\test_nc_buffer_generation_validity_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_axis_config_apply_edges ===== >> "%RESULT%"
build\tests\test_nc_axis_config_apply_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_coordinate_transform_apply_edges ===== >> "%RESULT%"
build\tests\test_nc_coordinate_transform_apply_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_system_shared_event_log_more_edges ===== >> "%RESULT%"
build\tests\test_system_shared_event_log_more_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
REM Additional v9 unit tests
echo ===== test_control_api_remaining_setters ===== >> "%RESULT%"
build\tests\test_control_api_remaining_setters.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_program_request_edges ===== >> "%RESULT%"
build\tests\test_nc_program_request_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_rt_control_dispatch_edges ===== >> "%RESULT%"
build\tests\test_rt_control_dispatch_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_ts_service_log_and_slice_edges ===== >> "%RESULT%"
build\tests\test_ts_service_log_and_slice_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_motion_filter_multiaxis_accel_edges ===== >> "%RESULT%"
build\tests\test_nc_motion_filter_multiaxis_accel_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
REM Additional v11 unit tests
echo ===== test_nc_feed_per_rev_and_error_edges ===== >> "%RESULT%"
build\tests\test_nc_feed_per_rev_and_error_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_compensation_offset_cancel_edges ===== >> "%RESULT%"
build\tests\test_nc_compensation_offset_cancel_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_tool_management_duplicate_and_mfin_edges ===== >> "%RESULT%"
build\tests\test_nc_tool_management_duplicate_and_mfin_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_coordinate_selection_clear_edges ===== >> "%RESULT%"
build\tests\test_nc_coordinate_selection_clear_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_codes_decimal_and_address_edges ===== >> "%RESULT%"
build\tests\test_nc_codes_decimal_and_address_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"

REM Additional v12 unit tests
echo ===== test_nc_capability_null_and_address_more ===== >> "%RESULT%"
build\tests\test_nc_capability_null_and_address_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_rotary_mcc_invalid_and_noop_edges ===== >> "%RESULT%"
build\tests\test_nc_rotary_mcc_invalid_and_noop_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_parser_public_line_more_edges ===== >> "%RESULT%"
build\tests\test_nc_parser_public_line_more_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_interpolation_begin_cancel_more_edges ===== >> "%RESULT%"
build\tests\test_nc_interpolation_begin_cancel_more_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_system_shared_defaults_reinit_edges ===== >> "%RESULT%"
build\tests\test_system_shared_defaults_reinit_edges.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
REM Additional v13 unit tests - 20 files
echo ===== test_nc_axis_config_definition_more ===== >> "%RESULT%"
build\tests\test_nc_axis_config_definition_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_axis_config_apply_path_filter_more ===== >> "%RESULT%"
build\tests\test_nc_axis_config_apply_path_filter_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_axis_config_diameter_rounding_more ===== >> "%RESULT%"
build\tests\test_nc_axis_config_diameter_rounding_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_coordinate_work_offset_more ===== >> "%RESULT%"
build\tests\test_nc_coordinate_work_offset_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_coordinate_machine_inverse_more ===== >> "%RESULT%"
build\tests\test_nc_coordinate_machine_inverse_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_coordinate_fill_targets_more ===== >> "%RESULT%"
build\tests\test_nc_coordinate_fill_targets_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_coordinate_transform_enable_more ===== >> "%RESULT%"
build\tests\test_nc_coordinate_transform_enable_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_coordinate_transform_apply_combo_more ===== >> "%RESULT%"
build\tests\test_nc_coordinate_transform_apply_combo_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_cycle_modal_replay_more ===== >> "%RESULT%"
build\tests\test_nc_cycle_modal_replay_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_cycle_segment_guards_more ===== >> "%RESULT%"
build\tests\test_nc_cycle_segment_guards_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_feed_prepare_motion_zero_override_more ===== >> "%RESULT%"
build\tests\test_nc_feed_prepare_motion_zero_override_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_feed_prepare_dwell_bounds_more ===== >> "%RESULT%"
build\tests\test_nc_feed_prepare_dwell_bounds_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_interference_check_axis_limit_more ===== >> "%RESULT%"
build\tests\test_nc_interference_check_axis_limit_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_lookahead_null_and_g08_more ===== >> "%RESULT%"
build\tests\test_nc_lookahead_null_and_g08_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_precision_setters_valid_invalid_more ===== >> "%RESULT%"
build\tests\test_nc_precision_setters_valid_invalid_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_spindle_css_stop_more ===== >> "%RESULT%"
build\tests\test_nc_spindle_css_stop_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_tool_management_prepare_invalid_more ===== >> "%RESULT%"
build\tests\test_nc_tool_management_prepare_invalid_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_synchronization_overlay_bounds_more ===== >> "%RESULT%"
build\tests\test_nc_synchronization_overlay_bounds_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_reference_one_direction_more ===== >> "%RESULT%"
build\tests\test_nc_reference_one_direction_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_rotary_mcc_output_service_more ===== >> "%RESULT%"
build\tests\test_nc_rotary_mcc_output_service_more.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
type "%RESULT%"
if "%FAIL%"=="1" (
  echo [run_tests] FAIL. See build\test_results.txt.
  exit /b 1
)
echo [run_tests] PASS. Results: build\test_results.txt
call scripts\run_tests_v14.bat
call scripts\run_tests_v15.bat
call scripts\run_tests_v16.bat
call scripts\run_tests_v17.bat
call scripts\run_tests_v18.bat
call scripts\run_tests_v19.bat
exit /b 0
