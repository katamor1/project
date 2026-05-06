<!-- ibm-bob/samples/base/source/scripts/README.md -->
<!-- Lists the supported build and test scripts in the IBM BOB sample workspace. -->
<!-- This exists so agents can choose the pinned command without moving or renaming scripts. -->
<!-- RELEVANT FILES: ibm-bob/samples/base/source/AGENTS.md, ibm-bob/samples/base/source/scripts/build_clang.bat, ibm-bob/samples/base/source/scripts/test_clang.bat -->
# Scripts

Run these commands from `ibm-bob/samples/base/source`.

## Preferred Smoke Commands

- `cmd /c scripts\build_clang.bat`: build the sample with `clang-cl`.
- `cmd /c scripts\test_clang.bat`: build and run the clang smoke test.
- `cmd /c scripts\build_vc2026.bat`: build with the VC2026 profile.
- `cmd /c scripts\test_vc2026.bat`: build and run the VC2026 smoke test.

## Unit-Test Suites

- `scripts/additive_tests.tsv`: consolidated unit-test manifest.
- `scripts/build_tests.bat` and `scripts/run_tests.bat`: Windows full unit-test suite.
- `scripts/build_tests_clang.sh` and `scripts/run_tests_clang.sh`: clang shell full unit-test suite.
- `scripts/build_tests_gcc.sh` and `scripts/run_tests_gcc.sh`: GCC shell full unit-test suite.
- `scripts/build_additive_tests.*` and `scripts/run_additive_tests.*`: manifest helpers used by the full suites.

## Portable Smoke Fallback

- `scripts/build_gcc_smoke.sh` and `scripts/test_gcc_smoke.sh`: portable GCC smoke build and run.

## Generated Output

Build output belongs under `ibm-bob/samples/base/source/build` or `build_gcc`.

If a `scripts/build` folder appears, it is generated noise from running a script with the wrong working directory.
