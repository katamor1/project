@REM ibm-bob/samples/base/source/scripts/test_clang.bat
@REM Builds and runs the IBM BOB sample smoke test with clang-cl.
@REM This exists so baseline and NC v4 coordinate/unit behavior are checked together.
@REM RELEVANT FILES: ibm-bob/samples/base/source/scripts/build_clang.bat, ibm-bob/samples/base/source/src/main.c, ibm-bob/samples/base/source/src/nc_program.c
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

findstr /c:"mode=1 state=3 alarm=9001 cycle=16" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"nc_state=6 nc_exec=15 nc_x=1200 nc_y=2300 nc_z=3400 nc_m=130" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected NC output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"interp_segments=5 feed_profile=1 feed_override=100 dwell_ticks=1" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected feed/interpolation output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"coord_unit=0 coord_distance=0 coord_work=0 coord_local=0 coord_work_x=1000" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected coordinate output was not found.
  type build\test_output.txt
  exit /b 1
)

findstr /c:"coord_error_checks=5" build\test_output.txt >nul
if errorlevel 1 (
  echo ERROR: expected coordinate error checks were not found.
  type build\test_output.txt
  exit /b 1
)

if not exist runtime_log.csv (
  echo ERROR: runtime_log.csv was not generated.
  exit /b 1
)

echo [test_clang] PASS
exit /b 0
