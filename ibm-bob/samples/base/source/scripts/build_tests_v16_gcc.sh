#!/usr/bin/env sh
# Builds the v16 additive unit tests (40 cases / 10 executables).
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR="$ROOT_DIR/build/tests_v16"
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
build test_nc_compensation_v16_transform_more "$ROOT_DIR/tests/test_nc_compensation_v16_transform_more.c" "$ROOT_DIR/src/nc_compensation.c" "$SYS"
build test_nc_coordinate_transform_v16_enable_more "$ROOT_DIR/tests/test_nc_coordinate_transform_v16_enable_more.c" "$ROOT_DIR/src/nc_coordinate_transform.c" "$ROOT_DIR/src/nc_coordinate.c" "$SYS"
build test_nc_axis_config_v16_filter_more "$ROOT_DIR/tests/test_nc_axis_config_v16_filter_more.c" "$ROOT_DIR/src/nc_axis_config.c" "$SYS"
build test_nc_kinematics_v16_transform_more "$ROOT_DIR/tests/test_nc_kinematics_v16_transform_more.c" "$ROOT_DIR/src/nc_kinematics.c" "$SYS"
build test_nc_interp_math_v16_geometry_more "$ROOT_DIR/tests/test_nc_interp_math_v16_geometry_more.c" "$ROOT_DIR/src/nc_interp_math.c"
build test_nc_interpolation_v16_step_more "$ROOT_DIR/tests/test_nc_interpolation_v16_step_more.c" "$ROOT_DIR/src/nc_interpolation.c" "$ROOT_DIR/src/nc_cycle.c" "$ROOT_DIR/src/nc_feed.c" "$ROOT_DIR/src/nc_motion_filter.c" "$ROOT_DIR/src/nc_interp_math.c" "$SYS"
build test_nc_parser_internal_v16_modal_more "$ROOT_DIR/tests/test_nc_parser_internal_v16_modal_more.c" "$ROOT_DIR/src/nc_parser_modal.c" "$ROOT_DIR/src/nc_parser_feature.c" "$ROOT_DIR/src/nc_parser_tokens.c" "$ROOT_DIR/src/nc_codes.c" "$ROOT_DIR/src/nc_capability.c" "$ROOT_DIR/src/nc_coordinate.c" "$ROOT_DIR/src/nc_coordinate_transform.c" "$ROOT_DIR/src/nc_cycle.c" "$ROOT_DIR/src/nc_interpolation.c" "$ROOT_DIR/src/nc_feed.c" "$ROOT_DIR/src/nc_motion_filter.c" "$ROOT_DIR/src/nc_interp_math.c" "$ROOT_DIR/src/nc_compensation.c" "$ROOT_DIR/src/nc_axis_config.c" "$ROOT_DIR/src/nc_kinematics.c" "$ROOT_DIR/src/nc_rotary_mcc.c" "$SYS"
build test_rt_control_v16_state_more "$ROOT_DIR/tests/test_rt_control_v16_state_more.c" "$ROOT_DIR/src/rt_control.c" "$SYS"
build test_ts_service_v16_prefetch_ui_more "$ROOT_DIR/tests/test_ts_service_v16_prefetch_ui_more.c" "$ROOT_DIR/src/ts_service.c" "$SYS"
build test_control_api_v16_request_more "$ROOT_DIR/tests/test_control_api_v16_request_more.c" "$ROOT_DIR/src/control_api.c" "$ROOT_DIR/src/nc_program.c" "$ROOT_DIR/src/nc_buffer.c" "$SYS"
echo "[build_tests_v16_gcc] Build succeeded."
