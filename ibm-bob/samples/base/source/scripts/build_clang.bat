@REM ibm-bob/samples/base/source/scripts/build_clang.bat
@REM Builds the IBM BOB sample with clang-cl using the pinned sample workspace rules.
@REM This exists so the clang workspace profile compiles the sample without inventing alternate commands.
@REM RELEVANT FILES: ibm-bob/samples/base/source/AGENTS.md, ibm-bob/samples/base/source/scripts/test_clang.bat, ibm-bob/samples/base/source/scripts/build_vc2026.bat
@echo off
setlocal
cd /d "%~dp0.."

if not exist build mkdir build
if not exist build\obj mkdir build\obj

set "VS_DEV_SHELL=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1"
set "BACKLOG_MODULES=src\nc_feature_backlog_aux_safety.c src\nc_feature_backlog_lube_brake_thermal.c src\nc_feature_backlog_readiness_sensors.c src\nc_feature_backlog_pallet_fixture_safety.c src\nc_feature_backlog_tool_utility_monitors.c src\nc_feature_backlog_process_safety.c src\nc_feature_backlog_service_interlocks.c src\nc_feature_backlog_loader_environment.c"
set "CLANG_ARGS=/nologo /TC /W4 /D_CRT_SECURE_NO_WARNINGS /Iinc src\system_shared.c src\control_api.c src\nc_buffer.c src\nc_capability.c src\nc_codes.c src\nc_compensation.c src\nc_coordinate.c src\nc_coordinate_transform.c src\nc_cycle.c src\nc_diagnostics.c src\nc_feed.c src\nc_interference.c src\nc_interp_math.c src\nc_interpolation.c src\nc_axis_config.c src\nc_design_features.c src\nc_feature_backlog.c %BACKLOG_MODULES% src\nc_kinematics.c src\nc_lookahead.c src\nc_lathe_cycle.c src\nc_reference.c src\nc_precision.c src\nc_spindle.c src\nc_tool_management.c src\nc_synchronization.c src\nc_rotary_mcc.c src\nc_motion_filter.c src\nc_safety_motion.c src\nc_program.c src\nc_parser.c src\nc_parser_feature.c src\nc_parser_modal.c src\nc_parser_tokens.c src\rt_control.c src\ts_service.c src\main.c /Febuild\ibm_bob_sample.exe"
set "CLANG_CL="

echo [build_clang] Checking compiler...
where clang-cl.exe >nul 2>nul
if not errorlevel 1 set "CLANG_CL=clang-cl.exe"
if not defined CLANG_CL if exist "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\bin\clang-cl.exe" set "CLANG_CL=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\bin\clang-cl.exe"
if not defined CLANG_CL if exist "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\x64\bin\clang-cl.exe" set "CLANG_CL=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\x64\bin\clang-cl.exe"
if not defined CLANG_CL if exist "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\ARM64\bin\clang-cl.exe" set "CLANG_CL=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\ARM64\bin\clang-cl.exe"
if defined CLANG_CL goto compile_local
if not exist "%VS_DEV_SHELL%" (
  echo ERROR: clang-cl.exe was not found on PATH or in the Visual Studio LLVM toolchain.
  exit /b 1
)

echo [build_clang] Launching VS Dev Shell...
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$clang = $null; " ^
  "& '%VS_DEV_SHELL%' -Arch amd64 -HostArch amd64; " ^
  "Set-Location '%CD%'; " ^
  "$command = Get-Command clang-cl.exe -ErrorAction SilentlyContinue; " ^
  "if ($command) { $clang = $command.Source }; " ^
  "if (-not $clang) { " ^
  "  $candidates = @(" ^
  "    'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\bin\clang-cl.exe'," ^
  "    'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\x64\bin\clang-cl.exe'," ^
  "    'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\ARM64\bin\clang-cl.exe'" ^
  "  ); " ^
  "  $clang = $candidates | Where-Object { Test-Path $_ } | Select-Object -First 1 " ^
  "}; " ^
  "if (-not $clang) { Write-Error 'clang-cl.exe was not found after launching VS Dev Shell.'; exit 1 }; " ^
  "& $clang %CLANG_ARGS%"
if errorlevel 1 (
  echo ERROR: build failed.
  exit /b 1
)

echo [build_clang] Build succeeded.
exit /b 0

:compile_local
echo [build_clang] Compiling sample...
"%CLANG_CL%" %CLANG_ARGS%
if errorlevel 1 (
  echo ERROR: build failed.
  exit /b 1
)

echo [build_clang] Build succeeded.
exit /b 0
