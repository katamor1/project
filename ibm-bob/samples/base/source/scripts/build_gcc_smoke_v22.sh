#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
mkdir -p "$ROOT_DIR/build"
gcc -std=c99 -Wall -Wextra -I"$ROOT_DIR/inc" "$ROOT_DIR"/src/*.c -lm -o "$ROOT_DIR/build/ibm_bob_sample_v22"
