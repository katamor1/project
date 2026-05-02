#!/usr/bin/env sh
# Builds the v19 additive unit tests (40 cases / 10 executables).
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR="$ROOT_DIR/build/tests_v19"
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
build test_nc_axis_config_v19_apply_more "$ROOT_DIR/tests/test_nc_axis_config_v19_apply_more.c" "$ROOT_DIR/src/nc_axis_config.c" "$ROOT_DIR/src/nc_design_features.c" "$SYS"
build test_nc_compensation_v19_commands_more "$ROOT_DIR/tests/test_nc_compensation_v19_commands_more.c" "$ROOT_DIR/src/nc_compensation.c" "$SYS"
build test_nc_coordinate_transform_v19_modes_more "$ROOT_DIR/tests/test_nc_coordinate_transform_v19_modes_more.c" "$ROOT_DIR/src/nc_coordinate_transform.c" "$ROOT_DIR/src/nc_coordinate.c" "$SYS"
build test_nc_kinematics_v19_commands_more "$ROOT_DIR/tests/test_nc_kinematics_v19_commands_more.c" "$ROOT_DIR/src/nc_kinematics.c" "$SYS"
build test_nc_cycle_v19_variants_more "$ROOT_DIR/tests/test_nc_cycle_v19_variants_more.c" "$ROOT_DIR/src/nc_cycle.c" "$SYS"
build test_nc_feed_v19_profiles_more "$ROOT_DIR/tests/test_nc_feed_v19_profiles_more.c" "$ROOT_DIR/src/nc_feed.c" "$SYS"
build test_nc_interpolation_v19_runtime_more "$ROOT_DIR/tests/test_nc_interpolation_v19_runtime_more.c" "$ROOT_DIR/src/nc_interpolation.c" "$ROOT_DIR/src/nc_cycle.c" "$ROOT_DIR/src/nc_feed.c" "$ROOT_DIR/src/nc_motion_filter.c" "$ROOT_DIR/src/nc_interp_math.c" "$SYS"
build test_nc_reference_v19_markers_more "$ROOT_DIR/tests/test_nc_reference_v19_markers_more.c" "$ROOT_DIR/src/nc_reference.c" "$SYS"
build test_rt_control_v19_trace_more "$ROOT_DIR/tests/test_rt_control_v19_trace_more.c" "$ROOT_DIR/src/rt_control.c" "$SYS"
build test_control_api_v19_gateways_more "$ROOT_DIR/tests/test_control_api_v19_gateways_more.c" "$ROOT_DIR/src/control_api.c" "$ROOT_DIR/src/nc_program.c" "$ROOT_DIR/src/nc_axis_config.c" "$ROOT_DIR/src/nc_design_features.c" "$SYS"
echo "[build_tests_v19_gcc] Build succeeded."
