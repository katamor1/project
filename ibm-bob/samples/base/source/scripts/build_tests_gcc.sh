#!/usr/bin/env sh
# ibm-bob/samples/base/source/scripts/build_tests_gcc.sh
# Builds the GCC unit-test suite from scripts/additive_tests.tsv.
# This exists so the portable GCC unit-test entry stays non-versioned and manifest-driven.
# RELEVANT FILES: ibm-bob/samples/base/source/scripts/run_tests_gcc.sh, ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/scripts/build_additive_tests.sh
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR="$ROOT_DIR/build/tests_gcc"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
CC=${CC:-gcc}
CFLAGS=${CFLAGS:-"-std=c99 -Wall -Wextra -I$ROOT_DIR/inc -I$ROOT_DIR/stubs -I$ROOT_DIR/tests"}
IBM_BOB_ADDITIVE_BUILD_DIR="$BUILD_DIR" CC="$CC" CFLAGS="$CFLAGS" "$ROOT_DIR/scripts/build_additive_tests.sh"
echo "[build_tests_gcc] Build succeeded."
