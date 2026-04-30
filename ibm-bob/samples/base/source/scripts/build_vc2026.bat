@REM ibm-bob/samples/base/source/scripts/build_vc2026.bat
@REM Builds the IBM BOB sample with cl.exe and can bootstrap a VS Dev Shell when cl is not already on PATH.
@REM This exists so the VC2026 workspace profile can run from both an active developer shell and a normal PowerShell session.
@REM RELEVANT FILES: ibm-bob/samples/base/source/AGENTS.md, ibm-bob/samples/base/source/scripts/test_vc2026.bat, ibm-bob/samples/base/source/docs/workspace_profile_commands.md
@echo off
setlocal
cd /d "%~dp0.."

if not exist build mkdir build
if not exist build\obj mkdir build\obj

set "VS_DEV_SHELL=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1"
set "CL_ARGS=/nologo /TC /W4 /D_CRT_SECURE_NO_WARNINGS /Iinc src\system_shared.c src\control_api.c src\nc_buffer.c src\nc_capability.c src\nc_codes.c src\nc_compensation.c src\nc_coordinate.c src\nc_coordinate_transform.c src\nc_cycle.c src\nc_diagnostics.c src\nc_feed.c src\nc_interference.c src\nc_interp_math.c src\nc_interpolation.c src\nc_axis_config.c src\nc_kinematics.c src\nc_lookahead.c src\nc_lathe_cycle.c src\nc_reference.c src\nc_precision.c src\nc_spindle.c src\nc_tool_management.c src\nc_synchronization.c src\nc_rotary_mcc.c src\nc_motion_filter.c src\nc_safety_motion.c src\nc_program.c src\nc_parser.c src\nc_parser_feature.c src\nc_parser_modal.c src\nc_parser_tokens.c src\rt_control.c src\ts_service.c src\main.c /Febuild\ibm_bob_sample.exe"

echo [build_vc2026] Checking compiler...
where cl.exe >nul 2>nul
if not errorlevel 1 (
  echo [build_vc2026] Compiling sample...
  cl %CL_ARGS%
  if errorlevel 1 (
    echo ERROR: build failed.
    exit /b 1
  )
  echo [build_vc2026] Build succeeded.
  exit /b 0
)

if not exist "%VS_DEV_SHELL%" (
  echo ERROR: cl.exe was not found in PATH.
  echo Hint: open a VC2026 Developer Command Prompt first.
  echo Hint: or make sure "%VS_DEV_SHELL%" is available.
  exit /b 1
)

echo [build_vc2026] Launching VS Dev Shell...
powershell -NoProfile -ExecutionPolicy Bypass -Command "& '%VS_DEV_SHELL%' -Arch amd64 -HostArch amd64; Set-Location '%CD%'; cl %CL_ARGS%"
if errorlevel 1 (
  echo ERROR: build failed.
  exit /b 1
)

echo [build_vc2026] Build succeeded.
exit /b 0
