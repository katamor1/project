@REM ibm-bob/samples/base/source/scripts/build_additive_tests.bat
@REM Builds unit tests listed in scripts/additive_tests.tsv.
@REM This exists so unit-test build scripts stay manifest-driven.
@REM RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/scripts/run_additive_tests.bat, ibm-bob/samples/base/source/scripts/build_tests.bat
@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0.."
if not exist build mkdir build
if not exist build\tests mkdir build\tests
if not defined CC set "CC=cl"
set "COMPILER=%CC%"
set "MANIFEST=scripts\additive_tests.tsv"
set "COMMON_FLAGS=/nologo /TC /W4 /D_CRT_SECURE_NO_WARNINGS /Iinc /Istubs /Itests"
for /f "usebackq tokens=1,2,3,* delims=|" %%A in ("%MANIFEST%") do (
  set "NAME=%%A"
  if not "!NAME!"=="" if not "!NAME:~0,1!"=="#" (
    set "MSVC_FLAGS=%%B"
    set "SOURCES=%%D"
    if "!MSVC_FLAGS!"=="-" set "MSVC_FLAGS="
    set "SOURCE_ARGS="
    for %%S in (!SOURCES!) do set "SOURCE_ARGS=!SOURCE_ARGS! "%%S""
    echo [build_additive_tests] !NAME!
    "!COMPILER!" %COMMON_FLAGS% !MSVC_FLAGS! !SOURCE_ARGS! /Fe"build\tests\!NAME!.exe"
    if errorlevel 1 exit /b 1
  )
)
echo [build_additive_tests] Build succeeded.
exit /b 0
