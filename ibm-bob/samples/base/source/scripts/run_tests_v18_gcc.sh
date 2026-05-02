#!/usr/bin/env sh
# Runs the v18 additive unit tests (40 cases / 10 executables).
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR="$ROOT_DIR/build/tests_v18"
RESULT_FILE="$ROOT_DIR/build/test_results_v18_added.txt"
"$ROOT_DIR/scripts/build_tests_v18_gcc.sh"
: > "$RESULT_FILE"
for exe in \
  test_nc_codes_v18_lookup_more \
  test_nc_buffer_v18_generation_more \
  test_nc_capability_v18_address_history_more \
  test_nc_motion_filter_v18_disable_limits_more \
  test_nc_precision_v18_learning_service_more \
  test_nc_safety_motion_v18_priority_more \
  test_nc_rotary_mcc_v18_output_more \
  test_nc_spindle_v18_parse_sync_more \
  test_nc_coordinate_transform_v18_apply_more \
  test_nc_parser_v18_modal_errors_more; do
  echo "[run] $exe" | tee -a "$RESULT_FILE"
  "$BUILD_DIR/$exe" | tee -a "$RESULT_FILE"
done
echo "[run_tests_v18_gcc] All v18 additive tests passed." | tee -a "$RESULT_FILE"
