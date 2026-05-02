#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
"$ROOT_DIR/scripts/build_gcc_smoke_v22.sh"
"$ROOT_DIR/build/ibm_bob_sample_v22" | tee "$ROOT_DIR/docs/verification_output_v22.txt"
