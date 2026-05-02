#!/usr/bin/env sh
# Runs the v19 additive unit tests (40 cases / 10 executables).
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR="$ROOT_DIR/build/tests_v19"
RESULT_FILE="$ROOT_DIR/build/test_results_v19_added.txt"
"$ROOT_DIR/scripts/build_tests_v19_gcc.sh"
: > "$RESULT_FILE"
for exe in \
  test_nc_axis_config_v19_apply_more \
  test_nc_compensation_v19_commands_more \
  test_nc_coordinate_transform_v19_modes_more \
  test_nc_kinematics_v19_commands_more \
  test_nc_cycle_v19_variants_more \
  test_nc_feed_v19_profiles_more \
  test_nc_interpolation_v19_runtime_more \
  test_nc_reference_v19_markers_more \
  test_rt_control_v19_trace_more \
  test_control_api_v19_gateways_more; do
  echo "[run] $exe" | tee -a "$RESULT_FILE"
  "$BUILD_DIR/$exe" | tee -a "$RESULT_FILE"
done
echo "[run_tests_v19_gcc] All v19 additive tests passed." | tee -a "$RESULT_FILE"
