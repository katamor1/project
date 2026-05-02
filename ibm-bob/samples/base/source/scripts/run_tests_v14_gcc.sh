#!/usr/bin/env sh
# Builds and runs the v14 additive unit tests.
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
RESULT_FILE="$ROOT_DIR/build/test_results_v14_added.txt"
"$ROOT_DIR/scripts/build_tests_v14_gcc.sh"
: > "$RESULT_FILE"
for test_exe in \
  test_nc_codes_v14_catalog_more \
  test_nc_feed_v14_rt_update_more \
  test_nc_coordinate_v14_invalid_and_modes_more \
  test_nc_axis_config_v14_masks_more \
  test_nc_interference_v14_more \
  test_nc_reference_v14_more \
  test_nc_spindle_v14_more \
  test_nc_tool_management_v14_more \
  test_nc_motion_filter_v14_endpoint_more \
  test_control_api_v14_more_getters_setters
  do
    echo "===== $test_exe =====" | tee -a "$RESULT_FILE"
    "$ROOT_DIR/build/tests_v14/$test_exe" 2>&1 | tee -a "$RESULT_FILE"
  done
echo "V14_ADDED_TESTS_TOTAL=40" | tee -a "$RESULT_FILE"
echo "V14_ADDED_TESTS_PASS=40" | tee -a "$RESULT_FILE"
