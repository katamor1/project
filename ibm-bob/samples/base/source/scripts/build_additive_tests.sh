#!/usr/bin/env sh
# ibm-bob/samples/base/source/scripts/build_additive_tests.sh
# Builds unit tests listed in scripts/additive_tests.tsv.
# This exists so unit-test build scripts stay manifest-driven.
# RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/scripts/run_additive_tests.sh, ibm-bob/samples/base/source/scripts/build_tests_gcc.sh
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
MANIFEST="$ROOT_DIR/scripts/additive_tests.tsv"
BUILD_DIR=${IBM_BOB_ADDITIVE_BUILD_DIR:-"$ROOT_DIR/build/tests"}
CC=${CC:-gcc}
LDFLAGS=${LDFLAGS:-}
CFLAGS=${CFLAGS:-"-std=c99 -Wall -Wextra -I$ROOT_DIR/inc -I$ROOT_DIR/stubs -I$ROOT_DIR/tests"}
mkdir -p "$BUILD_DIR"
while IFS='|' read -r name _msvc_flags posix_flags sources
do
  [ -n "$name" ] || continue
  case "$name" in \#*) continue ;; esac
  [ "$posix_flags" != "-" ] || posix_flags=
  set --
  for src in $sources
  do
    set -- "$@" "$ROOT_DIR/$src"
  done
  echo "[build_additive_tests] $name"
  "$CC" $LDFLAGS $CFLAGS $posix_flags "$@" -lm -o "$BUILD_DIR/$name"
done < "$MANIFEST"
echo "[build_additive_tests] Build succeeded."
