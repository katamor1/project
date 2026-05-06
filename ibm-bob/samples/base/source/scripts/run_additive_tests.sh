#!/usr/bin/env sh
# ibm-bob/samples/base/source/scripts/run_additive_tests.sh
# Runs unit tests listed in scripts/additive_tests.tsv.
# This exists so unit-test run scripts stay manifest-driven.
# RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/scripts/build_additive_tests.sh, ibm-bob/samples/base/source/scripts/run_tests_gcc.sh
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
MANIFEST="$ROOT_DIR/scripts/additive_tests.tsv"
BUILD_DIR=${IBM_BOB_ADDITIVE_BUILD_DIR:-"$ROOT_DIR/build/tests"}
RESULT=${IBM_BOB_ADDITIVE_RESULT:-"$ROOT_DIR/build/test_results.txt"}
APPEND=${IBM_BOB_ADDITIVE_APPEND:-0}
FAIL=0
if [ "${IBM_BOB_SKIP_ADDITIVE_BUILD:-0}" != "1" ]; then
  IBM_BOB_ADDITIVE_BUILD_DIR="$BUILD_DIR" "$ROOT_DIR/scripts/build_additive_tests.sh"
fi
if [ "$APPEND" != "1" ]; then
  : > "$RESULT"
fi
while IFS='|' read -r name _msvc_flags _posix_flags _sources
do
  [ -n "$name" ] || continue
  case "$name" in \#*) continue ;; esac
  echo "===== $name =====" >> "$RESULT"
  if ! "$BUILD_DIR/$name" >> "$RESULT" 2>&1; then FAIL=1; fi
  echo >> "$RESULT"
done < "$MANIFEST"
if [ "$APPEND" != "1" ]; then
  cat "$RESULT"
fi
if [ "$FAIL" -ne 0 ]; then
  echo "[run_additive_tests] FAIL. Results: $RESULT"
  exit 1
fi
echo "[run_additive_tests] PASS. Results: $RESULT"
