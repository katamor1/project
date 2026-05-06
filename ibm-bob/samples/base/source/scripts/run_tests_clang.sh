#!/usr/bin/env sh
# ibm-bob/samples/base/source/scripts/run_tests_clang.sh
# Runs the clang unit-test suite from scripts/additive_tests.tsv.
# This exists so the clang test entry stays non-versioned and manifest-driven.
# RELEVANT FILES: ibm-bob/samples/base/source/scripts/build_tests_clang.sh, ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/scripts/run_additive_tests.sh
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
"$ROOT_DIR/scripts/build_tests_clang.sh"
IBM_BOB_SKIP_ADDITIVE_BUILD=1 \
IBM_BOB_ADDITIVE_RESULT="$ROOT_DIR/build/test_results.txt" \
IBM_BOB_ADDITIVE_BUILD_DIR="$ROOT_DIR/build/tests_clang" \
  "$ROOT_DIR/scripts/run_additive_tests.sh"
echo "[run_tests_clang] PASS. Results: build/test_results.txt"
