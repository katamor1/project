#!/usr/bin/env sh
# Builds the v14 additive unit tests (40 cases / 10 executables).
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR="$ROOT_DIR/build/tests_v14"
CC=${CC:-gcc}
CFLAGS="${CFLAGS:--std=c99 -Wall -Wextra} -I$ROOT_DIR/inc -I$ROOT_DIR/stubs -I$ROOT_DIR/tests"
OBJ_DIR="${TMPDIR:-/tmp}/ibm_bob_tests_v14_objs_$$"
mkdir -p "$BUILD_DIR" "$OBJ_DIR"
trap 'rm -rf "$OBJ_DIR"' EXIT INT TERM
build() {
  out="$1"; shift
  echo "[build] $out"
  objs=""
  idx=0
  for src in "$@"; do
    obj="$OBJ_DIR/${out}_${idx}.o"
    "$CC" $CFLAGS -c "$src" -o "$obj"
    objs="$objs $obj"
    idx=$((idx + 1))
  done
  "$CC" $objs -lm -o "$OBJ_DIR/$out"
  cp "$OBJ_DIR/$out" "$BUILD_DIR/$out"
  chmod +x "$BUILD_DIR/$out"
}
build test_nc_codes_v14_catalog_more \
  "$ROOT_DIR/tests/test_nc_codes_v14_catalog_more.c" \
  "$ROOT_DIR/src/nc_codes.c"
build test_nc_feed_v14_rt_update_more \
  "$ROOT_DIR/tests/test_nc_feed_v14_rt_update_more.c" \
  "$ROOT_DIR/src/nc_feed.c" \
  "$ROOT_DIR/src/system_shared.c"
build test_nc_coordinate_v14_invalid_and_modes_more \
  "$ROOT_DIR/tests/test_nc_coordinate_v14_invalid_and_modes_more.c" \
  "$ROOT_DIR/src/nc_coordinate.c" \
  "$ROOT_DIR/src/system_shared.c"
build test_nc_axis_config_v14_masks_more \
  "$ROOT_DIR/tests/test_nc_axis_config_v14_masks_more.c" \
  "$ROOT_DIR/src/nc_axis_config.c" \
  "$ROOT_DIR/src/system_shared.c"
build test_nc_interference_v14_more \
  "$ROOT_DIR/tests/test_nc_interference_v14_more.c" \
  "$ROOT_DIR/src/nc_interference.c" \
  "$ROOT_DIR/src/system_shared.c"
build test_nc_reference_v14_more \
  "$ROOT_DIR/tests/test_nc_reference_v14_more.c" \
  "$ROOT_DIR/src/nc_reference.c" \
  "$ROOT_DIR/src/system_shared.c"
build test_nc_spindle_v14_more \
  "$ROOT_DIR/tests/test_nc_spindle_v14_more.c" \
  "$ROOT_DIR/src/nc_spindle.c" \
  "$ROOT_DIR/src/system_shared.c"
build test_nc_tool_management_v14_more \
  "$ROOT_DIR/tests/test_nc_tool_management_v14_more.c" \
  "$ROOT_DIR/src/nc_tool_management.c" \
  "$ROOT_DIR/src/system_shared.c"
build test_nc_motion_filter_v14_endpoint_more \
  "$ROOT_DIR/tests/test_nc_motion_filter_v14_endpoint_more.c" \
  "$ROOT_DIR/src/nc_motion_filter.c" \
  "$ROOT_DIR/src/system_shared.c"
build test_control_api_v14_more_getters_setters \
  "$ROOT_DIR/tests/test_control_api_v14_more_getters_setters.c" \
  "$ROOT_DIR/src/control_api.c" \
  "$ROOT_DIR/src/system_shared.c" \
  "$ROOT_DIR/src/nc_buffer.c" \
  "$ROOT_DIR/src/nc_motion_filter.c" \
  "$ROOT_DIR/stubs/stub_external_io.c" \
  "$ROOT_DIR/stubs/stub_module_deps.c"
echo "[build_tests_v14_gcc] Build succeeded."
