#!/usr/bin/env sh
# Runs the v16 additive unit tests (40 cases / 10 executables).
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR="$ROOT_DIR/build/tests_v16"
RESULT_FILE="$ROOT_DIR/build/test_results_v16_added.txt"
"$ROOT_DIR/scripts/build_tests_v16_gcc.sh"
: > "$RESULT_FILE"
for exe in \
  test_nc_compensation_v16_transform_more \
  test_nc_coordinate_transform_v16_enable_more \
  test_nc_axis_config_v16_filter_more \
  test_nc_kinematics_v16_transform_more \
  test_nc_interp_math_v16_geometry_more \
  test_nc_interpolation_v16_step_more \
  test_nc_parser_internal_v16_modal_more \
  test_rt_control_v16_state_more \
  test_ts_service_v16_prefetch_ui_more \
  test_control_api_v16_request_more; do
  echo "[run] $exe" | tee -a "$RESULT_FILE"
  "$BUILD_DIR/$exe" | tee -a "$RESULT_FILE"
done
echo "[run_tests_v16_gcc] All v16 additive tests passed." | tee -a "$RESULT_FILE"
