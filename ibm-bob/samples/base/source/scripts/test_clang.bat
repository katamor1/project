@REM ibm-bob/samples/base/source/scripts/test_clang.bat
@REM Builds and runs the IBM BOB sample smoke test.
@REM This exists so NC v7 diagnostics, compensation, kinematics, path, and cycle behavior are checked together.
@REM RELEVANT FILES: ibm-bob/samples/base/source/scripts/build_clang.bat, ibm-bob/samples/base/source/src/main.c, ibm-bob/samples/base/source/src/nc_diagnostics.c
@echo off
setlocal
cd /d "%~dp0.."

call scripts\build_clang.bat
if errorlevel 1 exit /b 1

if exist runtime_log.csv del /q runtime_log.csv
if exist build\test_output.txt del /q build\test_output.txt

echo [test_clang] Running smoke test...
build\ibm_bob_sample.exe > build\test_output.txt
if errorlevel 1 (
  echo ERROR: executable returned failure.
  exit /b 1
)

findstr /c:"nc_state=6 nc_error=0" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected NC completion output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"path_arc=3 path_helical=1 path_arc_segments=12" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected path-control output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"kin_transformed=43 kin_mirrored=23 kin_retracts=1" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected kinematics output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"diag_axis_warn=0x00000003 diag_axis_limit=0x00000001 diag_axis_critical=0x00000000 decel_level=2" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected axis-load diagnostic output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"tool_active=2 tool2_uses=31 tool_warn=0x00000000 tool_expired=0x00000000" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected tool-life diagnostic output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"g08_standard_seen=1 g08_high_precision_seen=1 g08_cancel_seen=1 g08_final_smoothing=0 g08_final_high_precision=0" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected G08 smoothing status output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"hpcc_seen=1 hpcc_cancel_seen=1 hpcc_final=0 g05_p3_seen=1" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected HPCC status output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"motion_filter_samples=1092 mode=2 axis_mask=0x0000000f" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected motion-filter status output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"motion_filter_limits vel_events=18 accel_events=64 endpoint_corrections=58" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected motion-filter limit output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"tool_life_hold_state=5 tool_life_hold_error=15 tool_life_hold_line=4" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected tool-life hold output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"coord_error_checks=5" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected coordinate error checks were not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"g08_error_checks=3" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected G08 error checks were not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"hpcc_error_checks=4" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected HPCC error checks were not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"cycle_detail last=890 repeat=1 repeat_done=15 peck_blocks=4 tap_blocks=2 boring_blocks=5 spindle_sync=2 spindle_reverse=2 manual_return=1" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected fixed-cycle detail output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"cycle_error_checks=4" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected fixed-cycle error checks were not found.
  type build\test_output.txt
  exit /b 1
)

if not exist runtime_log.csv (
  echo ERROR: runtime_log.csv was not generated.
  exit /b 1
)

echo [test_clang] PASS
exit /b 0
