#!/usr/bin/env sh
# Builds the v17 additive unit tests (40 cases / 10 executables).
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR="$ROOT_DIR/build/tests_v17"
CC=${CC:-gcc}
CFLAGS="${CFLAGS:--std=c99 -Wall -Wextra -ffunction-sections -fdata-sections} -I$ROOT_DIR/inc -I$ROOT_DIR/stubs -I$ROOT_DIR/tests"
LDFLAGS="${LDFLAGS:--Wl,--gc-sections}"
mkdir -p "$BUILD_DIR"
build() {
  out="$1"; shift
  echo "[build] $out"
  "$CC" $CFLAGS "$@" -lm $LDFLAGS -o "$BUILD_DIR/$out"
  chmod +x "$BUILD_DIR/$out"
}
SYS="$ROOT_DIR/src/system_shared.c"
build test_nc_capability_v17_accounting_more "$ROOT_DIR/tests/test_nc_capability_v17_accounting_more.c" "$ROOT_DIR/src/nc_capability.c" "$SYS"
build test_nc_coordinate_v17_modes_more "$ROOT_DIR/tests/test_nc_coordinate_v17_modes_more.c" "$ROOT_DIR/src/nc_coordinate.c" "$SYS"
build test_nc_cycle_v17_modal_segments_more "$ROOT_DIR/tests/test_nc_cycle_v17_modal_segments_more.c" "$ROOT_DIR/src/nc_cycle.c" "$SYS"
build test_nc_buffer_v17_ring_more "$ROOT_DIR/tests/test_nc_buffer_v17_ring_more.c" "$ROOT_DIR/src/nc_buffer.c" "$SYS"
build test_nc_diagnostics_v17_thresholds_more "$ROOT_DIR/tests/test_nc_diagnostics_v17_thresholds_more.c" "$ROOT_DIR/src/nc_diagnostics.c" "$SYS"
build test_nc_safety_motion_v17_sensor_edges "$ROOT_DIR/tests/test_nc_safety_motion_v17_sensor_edges.c" "$ROOT_DIR/src/nc_safety_motion.c" "$SYS"
build test_nc_spindle_v17_css_orient_more "$ROOT_DIR/tests/test_nc_spindle_v17_css_orient_more.c" "$ROOT_DIR/src/nc_spindle.c" "$SYS"
build test_nc_synchronization_v17_apply_more "$ROOT_DIR/tests/test_nc_synchronization_v17_apply_more.c" "$ROOT_DIR/src/nc_synchronization.c" "$SYS"
build test_nc_interference_v17_warning_more "$ROOT_DIR/tests/test_nc_interference_v17_warning_more.c" "$ROOT_DIR/src/nc_interference.c" "$SYS"
build test_system_shared_v17_ring_more "$ROOT_DIR/tests/test_system_shared_v17_ring_more.c" "$SYS"
echo "[build_tests_v17_gcc] Build succeeded."
