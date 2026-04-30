@REM scripts/run_tests.bat
@echo off
setlocal EnableExtensions
cd /d "%~dp0.."
call scripts\build_tests.bat
if errorlevel 1 exit /b 1
if not exist build mkdir build
set "RESULT=build\test_results.txt"
type nul > "%RESULT%"
set FAIL=0
echo ===== test_control_api ===== >> "%RESULT%"
build\tests\test_control_api.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_buffer ===== >> "%RESULT%"
build\tests\test_nc_buffer.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_diagnostics ===== >> "%RESULT%"
build\tests\test_nc_diagnostics.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_motion_filter ===== >> "%RESULT%"
build\tests\test_nc_motion_filter.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_codes ===== >> "%RESULT%"
build\tests\test_nc_codes.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_feed ===== >> "%RESULT%"
build\tests\test_nc_feed.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_coordinate ===== >> "%RESULT%"
build\tests\test_nc_coordinate.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_coordinate_transform ===== >> "%RESULT%"
build\tests\test_nc_coordinate_transform.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_axis_config ===== >> "%RESULT%"
build\tests\test_nc_axis_config.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_rotary_mcc ===== >> "%RESULT%"
build\tests\test_nc_rotary_mcc.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_capability ===== >> "%RESULT%"
build\tests\test_nc_capability.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_interference ===== >> "%RESULT%"
build\tests\test_nc_interference.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_safety_motion ===== >> "%RESULT%"
build\tests\test_nc_safety_motion.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_tool_management ===== >> "%RESULT%"
build\tests\test_nc_tool_management.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_spindle ===== >> "%RESULT%"
build\tests\test_nc_spindle.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_precision ===== >> "%RESULT%"
build\tests\test_nc_precision.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_synchronization ===== >> "%RESULT%"
build\tests\test_nc_synchronization.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_compensation ===== >> "%RESULT%"
build\tests\test_nc_compensation.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_cycle ===== >> "%RESULT%"
build\tests\test_nc_cycle.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_reference ===== >> "%RESULT%"
build\tests\test_nc_reference.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_lathe_cycle ===== >> "%RESULT%"
build\tests\test_nc_lathe_cycle.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_kinematics ===== >> "%RESULT%"
build\tests\test_nc_kinematics.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
echo ===== test_nc_interp_math ===== >> "%RESULT%"
build\tests\test_nc_interp_math.exe >> "%RESULT%" 2>&1
if errorlevel 1 set FAIL=1
echo. >> "%RESULT%"
type "%RESULT%"
if "%FAIL%"=="1" (
  echo [run_tests] FAIL. See build\test_results.txt.
  exit /b 1
)
echo [run_tests] PASS. Results: build\test_results.txt
exit /b 0
