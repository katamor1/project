@echo off
REM Builds the v17 additive unit tests with cl.exe.
setlocal enabledelayedexpansion
set ROOT_DIR=%~dp0..
set BUILD_DIR=%ROOT_DIR%\build\tests_v17
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
set CFLAGS=/nologo /TC /W4 /D_CRT_SECURE_NO_WARNINGS /I"%ROOT_DIR%\inc" /I"%ROOT_DIR%\stubs" /I"%ROOT_DIR%\tests"
set SYS="%ROOT_DIR%\src\system_shared.c"
call :build test_nc_capability_v17_accounting_more "%ROOT_DIR%\tests\test_nc_capability_v17_accounting_more.c" "%ROOT_DIR%\src\nc_capability.c" %SYS% || exit /b 1
call :build test_nc_coordinate_v17_modes_more "%ROOT_DIR%\tests\test_nc_coordinate_v17_modes_more.c" "%ROOT_DIR%\src\nc_coordinate.c" %SYS% || exit /b 1
call :build test_nc_cycle_v17_modal_segments_more "%ROOT_DIR%\tests\test_nc_cycle_v17_modal_segments_more.c" "%ROOT_DIR%\src\nc_cycle.c" %SYS% || exit /b 1
call :build test_nc_buffer_v17_ring_more "%ROOT_DIR%\tests\test_nc_buffer_v17_ring_more.c" "%ROOT_DIR%\src\nc_buffer.c" %SYS% || exit /b 1
call :build test_nc_diagnostics_v17_thresholds_more "%ROOT_DIR%\tests\test_nc_diagnostics_v17_thresholds_more.c" "%ROOT_DIR%\src\nc_diagnostics.c" %SYS% || exit /b 1
call :build test_nc_safety_motion_v17_sensor_edges "%ROOT_DIR%\tests\test_nc_safety_motion_v17_sensor_edges.c" "%ROOT_DIR%\src\nc_safety_motion.c" %SYS% || exit /b 1
call :build test_nc_spindle_v17_css_orient_more "%ROOT_DIR%\tests\test_nc_spindle_v17_css_orient_more.c" "%ROOT_DIR%\src\nc_spindle.c" %SYS% || exit /b 1
call :build test_nc_synchronization_v17_apply_more "%ROOT_DIR%\tests\test_nc_synchronization_v17_apply_more.c" "%ROOT_DIR%\src\nc_synchronization.c" %SYS% || exit /b 1
call :build test_nc_interference_v17_warning_more "%ROOT_DIR%\tests\test_nc_interference_v17_warning_more.c" "%ROOT_DIR%\src\nc_interference.c" %SYS% || exit /b 1
call :build test_system_shared_v17_ring_more "%ROOT_DIR%\tests\test_system_shared_v17_ring_more.c" %SYS% || exit /b 1
echo [build_tests_v17] Build succeeded.
exit /b 0

:build
echo [build] %~1
cl %CFLAGS% %2 %3 %4 %5 /Fe"%BUILD_DIR%\%~1.exe"
exit /b %ERRORLEVEL%
