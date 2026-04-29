#!/usr/bin/env sh
# Builds and runs the IBM-Bob GCC smoke simulation.
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
"$ROOT_DIR/scripts/build_gcc_smoke.sh"
"$ROOT_DIR/build_gcc/ibm_bob_sample"
