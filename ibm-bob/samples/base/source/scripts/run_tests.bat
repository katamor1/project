@REM ibm-bob/samples/base/source/scripts/run_tests.bat
@REM Runs the Windows unit-test suite from scripts/additive_tests.tsv.
@REM This exists so the Windows test entry stays non-versioned and manifest-driven.
@REM RELEVANT FILES: ibm-bob/samples/base/source/scripts/build_tests.bat, ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/scripts/run_additive_tests.bat
@echo off
setlocal EnableExtensions
cd /d "%~dp0.."
call scripts\build_tests.bat
if errorlevel 1 exit /b 1
set "IBM_BOB_SKIP_ADDITIVE_BUILD=1"
set "IBM_BOB_ADDITIVE_RESULT=build\test_results.txt"
call scripts\run_additive_tests.bat
if errorlevel 1 exit /b 1
echo [run_tests] PASS. Results: build\test_results.txt
exit /b 0
