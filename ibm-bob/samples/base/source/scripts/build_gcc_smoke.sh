#!/usr/bin/env sh
# ibm-bob/samples/base/source/scripts/build_gcc_smoke.sh
# Builds the IBM-Bob sample with GCC using one object per C file.
# This exists so large shared-memory headers do not need to be compiled as a single translation batch.
# RELEVANT FILES: ibm-bob/samples/base/source/scripts/test_gcc_smoke.sh, ibm-bob/samples/base/source/src/main.c, ibm-bob/samples/base/source/inc/system_shared.h
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR="$ROOT_DIR/build_gcc"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
for src in "$ROOT_DIR"/src/*.c; do
    obj="$BUILD_DIR/$(basename "$src" .c).o"
    gcc -std=c99 -Wall -Wextra -I"$ROOT_DIR/inc" -c "$src" -o "$obj"
done
gcc "$BUILD_DIR"/*.o -lm -o "$BUILD_DIR/ibm_bob_sample"
