@REM scripts/build_tests.bat
@echo off
setlocal EnableExtensions
cd /d "%~dp0.."
if not exist build mkdir build
if not exist build\tests mkdir build\tests
set "VS_DEV_SHELL=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\Launch-VsDevShell.ps1"
set "COMMON_FLAGS=/nologo /TC /W4 /D_CRT_SECURE_NO_WARNINGS /Iinc /Istubs /Itests"
where cl.exe >nul 2>nul
if not errorlevel 1 goto :compile
if not exist "%VS_DEV_SHELL%" (
  echo ERROR: cl.exe was not found in PATH.
  echo Hint: open a Visual Studio Developer Command Prompt or adjust VS_DEV_SHELL in scripts\build_tests.bat.
  exit /b 1
)
powershell -NoProfile -ExecutionPolicy Bypass -Command "& '%VS_DEV_SHELL%' -Arch amd64 -HostArch amd64; Set-Location '%CD%'; cmd /c scripts\build_tests.bat"
exit /b %ERRORLEVEL%
:compile
echo [build_tests] Building unit tests...
cl %COMMON_FLAGS% tests\test_control_api.c src\control_api.c src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c stubs\stub_module_deps.c /Fe:build\tests\test_control_api.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_buffer.c src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_buffer.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_diagnostics.c src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c src\nc_diagnostics.c stubs\stub_external_io.c /Fe:build\tests\test_nc_diagnostics.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_motion_filter.c src\system_shared.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_motion_filter.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_codes.c src\nc_codes.c /Fe:build\tests\test_nc_codes.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_feed.c src\system_shared.c src\nc_feed.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_feed.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate.c src\system_shared.c src\nc_coordinate.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_coordinate.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_coordinate_transform.c src\system_shared.c src\nc_coordinate.c src\nc_coordinate_transform.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_coordinate_transform.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_axis_config.c src\system_shared.c src\nc_axis_config.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_axis_config.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_rotary_mcc.c src\system_shared.c src\nc_rotary_mcc.c src\nc_buffer.c src\nc_motion_filter.c stubs\stub_external_io.c /Fe:build\tests\test_nc_rotary_mcc.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_capability.c src\system_shared.c src\nc_capability.c /Fe:build\tests\test_nc_capability.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_interference.c src\system_shared.c src\nc_interference.c /Fe:build\tests\test_nc_interference.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_safety_motion.c src\system_shared.c src\nc_safety_motion.c /Fe:build\tests\test_nc_safety_motion.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_tool_management.c src\system_shared.c src\nc_tool_management.c /Fe:build\tests\test_nc_tool_management.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_spindle.c src\system_shared.c src\nc_spindle.c /Fe:build\tests\test_nc_spindle.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_precision.c src\system_shared.c src\nc_precision.c /Fe:build\tests\test_nc_precision.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_synchronization.c src\system_shared.c src\nc_synchronization.c /Fe:build\tests\test_nc_synchronization.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_compensation.c src\system_shared.c src\nc_compensation.c /Fe:build\tests\test_nc_compensation.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_cycle.c src\system_shared.c src\nc_cycle.c /Fe:build\tests\test_nc_cycle.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_reference.c src\system_shared.c src\nc_reference.c /Fe:build\tests\test_nc_reference.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_lathe_cycle.c src\system_shared.c src\nc_lathe_cycle.c /Fe:build\tests\test_nc_lathe_cycle.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_kinematics.c src\system_shared.c src\nc_kinematics.c /Fe:build\tests\test_nc_kinematics.exe
if errorlevel 1 exit /b 1
cl %COMMON_FLAGS% tests\test_nc_interp_math.c src\system_shared.c src\nc_interp_math.c /Fe:build\tests\test_nc_interp_math.exe
if errorlevel 1 exit /b 1
echo [build_tests] Build succeeded.
exit /b 0
