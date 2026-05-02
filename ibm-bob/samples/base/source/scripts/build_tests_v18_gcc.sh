#!/usr/bin/env sh
# Builds the v18 additive unit tests (40 cases / 10 executables).
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR="$ROOT_DIR/build/tests_v18"
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
build test_nc_codes_v18_lookup_more "$ROOT_DIR/tests/test_nc_codes_v18_lookup_more.c" "$ROOT_DIR/src/nc_codes.c" "$SYS"
build test_nc_buffer_v18_generation_more "$ROOT_DIR/tests/test_nc_buffer_v18_generation_more.c" "$ROOT_DIR/src/nc_buffer.c" "$SYS"
build test_nc_capability_v18_address_history_more "$ROOT_DIR/tests/test_nc_capability_v18_address_history_more.c" "$ROOT_DIR/src/nc_capability.c" "$SYS"
build test_nc_motion_filter_v18_disable_limits_more "$ROOT_DIR/tests/test_nc_motion_filter_v18_disable_limits_more.c" "$ROOT_DIR/src/nc_motion_filter.c" "$SYS"
build test_nc_precision_v18_learning_service_more "$ROOT_DIR/tests/test_nc_precision_v18_learning_service_more.c" "$ROOT_DIR/src/nc_precision.c" "$SYS"
build test_nc_safety_motion_v18_priority_more "$ROOT_DIR/tests/test_nc_safety_motion_v18_priority_more.c" "$ROOT_DIR/src/nc_safety_motion.c" "$SYS"
build test_nc_rotary_mcc_v18_output_more "$ROOT_DIR/tests/test_nc_rotary_mcc_v18_output_more.c" "$ROOT_DIR/src/nc_rotary_mcc.c" "$SYS"
build test_nc_spindle_v18_parse_sync_more "$ROOT_DIR/tests/test_nc_spindle_v18_parse_sync_more.c" "$ROOT_DIR/src/nc_spindle.c" "$SYS"
build test_nc_coordinate_transform_v18_apply_more "$ROOT_DIR/tests/test_nc_coordinate_transform_v18_apply_more.c" "$ROOT_DIR/src/nc_coordinate_transform.c" "$ROOT_DIR/src/nc_coordinate.c" "$SYS"
build test_nc_parser_v18_modal_errors_more "$ROOT_DIR/tests/test_nc_parser_v18_modal_errors_more.c" "$ROOT_DIR/src/nc_parser.c" "$ROOT_DIR/src/nc_parser_modal.c" "$ROOT_DIR/src/nc_parser_tokens.c" "$ROOT_DIR/src/nc_parser_feature.c" "$ROOT_DIR/src/nc_codes.c" "$ROOT_DIR/src/nc_coordinate.c" "$ROOT_DIR/src/nc_coordinate_transform.c" "$ROOT_DIR/src/nc_cycle.c" "$ROOT_DIR/src/nc_interpolation.c" "$ROOT_DIR/src/nc_feed.c" "$ROOT_DIR/src/nc_motion_filter.c" "$ROOT_DIR/src/nc_buffer.c" "$ROOT_DIR/src/nc_compensation.c" "$ROOT_DIR/src/nc_kinematics.c" "$ROOT_DIR/src/nc_axis_config.c" "$ROOT_DIR/src/nc_rotary_mcc.c" "$ROOT_DIR/src/nc_interp_math.c" "$ROOT_DIR/src/nc_lookahead.c" "$ROOT_DIR/src/nc_capability.c" "$SYS"
echo "[build_tests_v18_gcc] Build succeeded."
