@REM ibm-bob/samples/base/source/scripts/build_tests.bat
@REM Builds the Windows unit-test suite from scripts/additive_tests.tsv.
@REM This exists so the Windows unit-test entry stays non-versioned and manifest-driven.
@REM RELEVANT FILES: ibm-bob/samples/base/source/scripts/run_tests.bat, ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/scripts/build_additive_tests.bat
@echo off
setlocal EnableExtensions
cd /d "%~dp0.."
set "VS_DEV_SHELL=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1"
if defined CC goto :compile
where cl.exe >nul 2>nul
if not errorlevel 1 goto :compile
if not exist "%VS_DEV_SHELL%" (
  echo ERROR: cl.exe was not found in PATH.
  echo Hint: open a Visual Studio Developer Command Prompt or adjust VS_DEV_SHELL in scripts\build_tests.bat.
  exit /b 1
)
if defined IBM_BOB_VS_BOOTSTRAPPED (
  echo ERROR: cl.exe was still not found after launching the Visual Studio developer shell.
  echo Hint: verify VS_DEV_SHELL or run from a Developer Command Prompt.
  exit /b 1
)
set "IBM_BOB_VS_BOOTSTRAPPED=1"
powershell -NoProfile -ExecutionPolicy Bypass -Command "& '%VS_DEV_SHELL%' -Arch amd64 -HostArch amd64; Set-Location '%CD%'; cmd /c scripts\build_tests.bat"
exit /b %ERRORLEVEL%
:compile
echo [build_tests] Building manifest-listed unit tests...
call scripts\build_additive_tests.bat
if errorlevel 1 exit /b 1
echo [build_tests] Build succeeded.
exit /b 0
