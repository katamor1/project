#!/usr/bin/env sh
# Runs the v17 additive unit tests (40 cases / 10 executables).
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR="$ROOT_DIR/build/tests_v17"
RESULT_FILE="$ROOT_DIR/build/test_results_v17_added.txt"
"$ROOT_DIR/scripts/build_tests_v17_gcc.sh"
: > "$RESULT_FILE"
for exe in \
  test_nc_capability_v17_accounting_more \
  test_nc_coordinate_v17_modes_more \
  test_nc_cycle_v17_modal_segments_more \
  test_nc_buffer_v17_ring_more \
  test_nc_diagnostics_v17_thresholds_more \
  test_nc_safety_motion_v17_sensor_edges \
  test_nc_spindle_v17_css_orient_more \
  test_nc_synchronization_v17_apply_more \
  test_nc_interference_v17_warning_more \
  test_system_shared_v17_ring_more; do
  echo "[run] $exe" | tee -a "$RESULT_FILE"
  "$BUILD_DIR/$exe" | tee -a "$RESULT_FILE"
done
echo "[run_tests_v17_gcc] All v17 additive tests passed." | tee -a "$RESULT_FILE"
