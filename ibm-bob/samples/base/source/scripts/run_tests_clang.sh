#!/usr/bin/env sh
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
"$ROOT_DIR/scripts/build_tests_clang.sh"
RESULT="$ROOT_DIR/build/test_results.txt"
: > "$RESULT"
FAIL=0
echo "===== test_control_api =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_control_api" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_buffer =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_buffer" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_diagnostics =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_diagnostics" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_motion_filter =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_motion_filter" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_codes =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_codes" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_feed =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_feed" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_coordinate =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_coordinate" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_coordinate_transform =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_coordinate_transform" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_axis_config =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_axis_config" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_rotary_mcc =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_rotary_mcc" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_capability =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_capability" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_interference =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_interference" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_safety_motion =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_safety_motion" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_tool_management =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_tool_management" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_spindle =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_spindle" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_precision =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_precision" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_synchronization =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_synchronization" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_compensation =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_compensation" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_cycle =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_cycle" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_reference =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_reference" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_lathe_cycle =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_lathe_cycle" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_kinematics =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_kinematics" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
echo "===== test_nc_interp_math =====" >> "$RESULT"
if ! "$ROOT_DIR/build/tests_clang/test_nc_interp_math" >> "$RESULT" 2>&1; then FAIL=1; fi
echo >> "$RESULT"
cat "$RESULT"
if [ "$FAIL" -ne 0 ]; then echo "[run_tests_clang] FAIL. Results: build/test_results.txt"; exit 1; fi
echo "[run_tests_clang] PASS. Results: build/test_results.txt"
