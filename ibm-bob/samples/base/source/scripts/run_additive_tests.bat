@REM ibm-bob/samples/base/source/scripts/run_additive_tests.bat
@REM Runs unit tests listed in scripts/additive_tests.tsv.
@REM This exists so unit-test run scripts stay manifest-driven.
@REM RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/scripts/build_additive_tests.bat, ibm-bob/samples/base/source/scripts/run_tests.bat
@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0.."
if not "%IBM_BOB_SKIP_ADDITIVE_BUILD%"=="1" (
  call scripts\build_additive_tests.bat
  if errorlevel 1 exit /b 1
)
if not exist build mkdir build
set "BUILD_DIR=build\tests"
set "MANIFEST=scripts\additive_tests.tsv"
set "RESULT=%IBM_BOB_ADDITIVE_RESULT%"
if not defined RESULT set "RESULT=build\test_results.txt"
type nul > "%RESULT%"
set "FAIL=0"
for /f "usebackq tokens=1,2,3,* delims=|" %%A in ("%MANIFEST%") do (
  set "NAME=%%A"
  if not "!NAME!"=="" if not "!NAME:~0,1!"=="#" (
    echo ===== !NAME! =====>> "%RESULT%"
    "%BUILD_DIR%\!NAME!.exe" >> "%RESULT%" 2>&1
    if errorlevel 1 set "FAIL=1"
    echo.>> "%RESULT%"
  )
)
type "%RESULT%"
if "!FAIL!"=="1" (
  echo [run_additive_tests] FAIL. See %RESULT%.
  exit /b 1
)
echo [run_additive_tests] PASS. Results: %RESULT%
exit /b 0
