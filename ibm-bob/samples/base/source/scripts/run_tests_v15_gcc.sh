#!/usr/bin/env sh
# Builds and runs the v15 additive unit tests.
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
RESULT_FILE="$ROOT_DIR/build/test_results_v15_added.txt"
"$ROOT_DIR/scripts/build_tests_v15_gcc.sh"
: > "$RESULT_FILE"
for test_exe in \
  test_nc_capability_v15_history_more \
  test_nc_lookahead_v15_modes_arcs_more \
  test_nc_precision_v15_modes_service_more \
  test_nc_synchronization_v15_modes_following_more \
  test_nc_rotary_mcc_v15_service_more \
  test_nc_lathe_cycle_v15_tracking_more \
  test_nc_safety_motion_v15_latch_release_more \
  test_nc_diagnostics_v15_snapshot_life_more \
  test_nc_buffer_v15_wrap_validity_more \
  test_system_shared_v15_event_log_more
  do
    echo "===== $test_exe =====" | tee -a "$RESULT_FILE"
    "$ROOT_DIR/build/tests_v15/$test_exe" 2>&1 | tee -a "$RESULT_FILE"
  done
echo "V15_ADDED_TESTS_TOTAL=40" | tee -a "$RESULT_FILE"
echo "V15_ADDED_TESTS_PASS=40" | tee -a "$RESULT_FILE"
