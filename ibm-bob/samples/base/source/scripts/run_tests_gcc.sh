#!/usr/bin/env sh
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
"$ROOT_DIR/scripts/build_tests_gcc.sh"
RESULT="$ROOT_DIR/build/test_results.txt"
: > "$RESULT"
FAIL=0
echo "===== test_control_api =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_control_api" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_buffer =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_buffer" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_diagnostics =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_diagnostics" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_motion_filter =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_motion_filter" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_codes =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_codes" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_feed =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_feed" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_coordinate =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_coordinate" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_coordinate_transform =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_coordinate_transform" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_axis_config =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_axis_config" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_rotary_mcc =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_rotary_mcc" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_capability =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_capability" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_interference =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_interference" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_safety_motion =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_safety_motion" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_tool_management =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_tool_management" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_spindle =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_spindle" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_precision =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_precision" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_synchronization =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_synchronization" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_compensation =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_compensation" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_cycle =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_cycle" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_reference =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_reference" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_lathe_cycle =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_lathe_cycle" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_kinematics =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_kinematics" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_interp_math =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_interp_math" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_system_shared =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_system_shared" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_ts_service =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_ts_service" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_rt_control =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_rt_control" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_lookahead =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_lookahead" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_interpolation =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_interpolation" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_parser =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_parser" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_control_api_extended =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_control_api_extended" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_parser_modal_direct =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_parser_modal_direct" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_parser_tokens_direct =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_parser_tokens_direct" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_program_binary_flow =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_program_binary_flow" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_parser_feature_direct =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_parser_feature_direct" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_codes_extended =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_codes_extended" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_program_feature_status =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_program_feature_status" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_coordinate_transform_direct =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_coordinate_transform_direct" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_safety_motion_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_safety_motion_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_reference_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_reference_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_tool_management_timeout =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_tool_management_timeout" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_precision_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_precision_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
# Additional v7 unit tests
echo "===== test_nc_feed_profile_boundaries =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_feed_profile_boundaries" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_cycle_segment_variants =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_cycle_segment_variants" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_capability_history_wrap =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_capability_history_wrap" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_interference_warning_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_interference_warning_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_spindle_orientation_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_spindle_orientation_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
# Additional v8 unit tests
echo "===== test_nc_compensation_rotation_polar_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_compensation_rotation_polar_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_interp_math_arc_r_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_interp_math_arc_r_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_kinematics_detach_tilt_cancel =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_kinematics_detach_tilt_cancel" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_synchronization_cancel_double_table =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_synchronization_cancel_double_table" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_lathe_cycle_variants =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_lathe_cycle_variants" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
# Additional v10 unit tests
echo "===== test_nc_coordinate_program_machine_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_coordinate_program_machine_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_buffer_generation_validity_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_buffer_generation_validity_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_axis_config_apply_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_axis_config_apply_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_coordinate_transform_apply_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_coordinate_transform_apply_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_system_shared_event_log_more_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_system_shared_event_log_more_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
# Additional v9 unit tests
echo "===== test_control_api_remaining_setters =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_control_api_remaining_setters" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_program_request_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_program_request_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_rt_control_dispatch_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_rt_control_dispatch_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_ts_service_log_and_slice_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_ts_service_log_and_slice_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_motion_filter_multiaxis_accel_edges =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_gcc/test_nc_motion_filter_multiaxis_accel_edges" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"

# Additional v11 unit tests
for name in \
  test_nc_feed_per_rev_and_error_edges \
  test_nc_compensation_offset_cancel_edges \
  test_nc_tool_management_duplicate_and_mfin_edges \
  test_nc_coordinate_selection_clear_edges \
  test_nc_codes_decimal_and_address_edges
do
  echo "===== $name =====" >> "$RESULT"
  if ! "$ROOT_DIR/build/tests_gcc/$name" >> "$RESULT" 2>&1; then FAIL=1; fi
  echo >> "$RESULT"
done

# Additional v12 unit tests
for name in \
  test_nc_capability_null_and_address_more \
  test_nc_rotary_mcc_invalid_and_noop_edges \
  test_nc_parser_public_line_more_edges \
  test_nc_interpolation_begin_cancel_more_edges \
  test_system_shared_defaults_reinit_edges
do
  echo "===== $name =====" >> "$RESULT"
  if ! "$ROOT_DIR/build/tests_gcc/$name" >> "$RESULT" 2>&1; then FAIL=1; fi
  echo >> "$RESULT"
done
# Additional v13 unit tests - 20 files
for name in \
  test_nc_axis_config_definition_more \
  test_nc_axis_config_apply_path_filter_more \
  test_nc_axis_config_diameter_rounding_more \
  test_nc_coordinate_work_offset_more \
  test_nc_coordinate_machine_inverse_more \
  test_nc_coordinate_fill_targets_more \
  test_nc_coordinate_transform_enable_more \
  test_nc_coordinate_transform_apply_combo_more \
  test_nc_cycle_modal_replay_more \
  test_nc_cycle_segment_guards_more \
  test_nc_feed_prepare_motion_zero_override_more \
  test_nc_feed_prepare_dwell_bounds_more \
  test_nc_interference_check_axis_limit_more \
  test_nc_lookahead_null_and_g08_more \
  test_nc_precision_setters_valid_invalid_more \
  test_nc_spindle_css_stop_more \
  test_nc_tool_management_prepare_invalid_more \
  test_nc_synchronization_overlay_bounds_more \
  test_nc_reference_one_direction_more \
  test_nc_rotary_mcc_output_service_more
do
  echo "===== $name =====" >> "$RESULT"
  if ! "$ROOT_DIR/build/tests_gcc/$name" >> "$RESULT" 2>&1; then FAIL=1; fi
  echo >> "$RESULT"
done
cat "$RESULT"
if [ "$FAIL" -ne 0 ]; then echo "[run_tests_gcc] FAIL. Results: build/test_results.txt"; exit 1; fi
echo "[run_tests_gcc] PASS. Results: build/test_results.txt"
