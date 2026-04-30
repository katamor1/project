#!/usr/bin/env sh
# ibm-bob/samples/base/source/scripts/test_gcc_smoke.sh
# Builds and runs the IBM-Bob GCC smoke simulation.
# This exists as a portable fallback smoke path outside the Windows compiler profiles.
# RELEVANT FILES: ibm-bob/samples/base/source/scripts/build_gcc_smoke.sh, ibm-bob/samples/base/source/src/main.c, ibm-bob/samples/base/source/AGENTS.md
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
"$ROOT_DIR/scripts/build_gcc_smoke.sh"
"$ROOT_DIR/build_gcc/ibm_bob_sample"
