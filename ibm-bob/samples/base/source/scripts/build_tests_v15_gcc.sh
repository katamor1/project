#!/usr/bin/env sh
# Builds the v15 additive unit tests (40 cases / 10 executables).
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR="$ROOT_DIR/build/tests_v15"
CC=${CC:-gcc}
CFLAGS="${CFLAGS:--std=c99 -Wall -Wextra} -I$ROOT_DIR/inc -I$ROOT_DIR/stubs -I$ROOT_DIR/tests"
mkdir -p "$BUILD_DIR"
build() {
  out="$1"; shift
  echo "[build] $out"
  "$CC" $CFLAGS "$@" -lm -o "$BUILD_DIR/$out"
  chmod +x "$BUILD_DIR/$out"
}
build test_nc_capability_v15_history_more "$ROOT_DIR/tests/test_nc_capability_v15_history_more.c" "$ROOT_DIR/src/nc_capability.c" "$ROOT_DIR/src/system_shared.c"
build test_nc_lookahead_v15_modes_arcs_more "$ROOT_DIR/tests/test_nc_lookahead_v15_modes_arcs_more.c" "$ROOT_DIR/src/nc_lookahead.c" "$ROOT_DIR/src/nc_interp_math.c" "$ROOT_DIR/src/system_shared.c"
build test_nc_precision_v15_modes_service_more "$ROOT_DIR/tests/test_nc_precision_v15_modes_service_more.c" "$ROOT_DIR/src/nc_precision.c" "$ROOT_DIR/src/system_shared.c"
build test_nc_synchronization_v15_modes_following_more "$ROOT_DIR/tests/test_nc_synchronization_v15_modes_following_more.c" "$ROOT_DIR/src/nc_synchronization.c" "$ROOT_DIR/src/system_shared.c"
build test_nc_rotary_mcc_v15_service_more "$ROOT_DIR/tests/test_nc_rotary_mcc_v15_service_more.c" "$ROOT_DIR/src/nc_rotary_mcc.c" "$ROOT_DIR/src/nc_codes.c" "$ROOT_DIR/src/system_shared.c"
build test_nc_lathe_cycle_v15_tracking_more "$ROOT_DIR/tests/test_nc_lathe_cycle_v15_tracking_more.c" "$ROOT_DIR/src/nc_lathe_cycle.c" "$ROOT_DIR/src/system_shared.c"
build test_nc_safety_motion_v15_latch_release_more "$ROOT_DIR/tests/test_nc_safety_motion_v15_latch_release_more.c" "$ROOT_DIR/src/nc_safety_motion.c" "$ROOT_DIR/src/system_shared.c"
build test_nc_diagnostics_v15_snapshot_life_more "$ROOT_DIR/tests/test_nc_diagnostics_v15_snapshot_life_more.c" "$ROOT_DIR/src/nc_diagnostics.c" "$ROOT_DIR/src/system_shared.c"
build test_nc_buffer_v15_wrap_validity_more "$ROOT_DIR/tests/test_nc_buffer_v15_wrap_validity_more.c" "$ROOT_DIR/src/nc_buffer.c" "$ROOT_DIR/src/system_shared.c"
build test_system_shared_v15_event_log_more "$ROOT_DIR/tests/test_system_shared_v15_event_log_more.c" "$ROOT_DIR/src/system_shared.c"
echo "[build_tests_v15_gcc] Build succeeded."
