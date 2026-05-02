@echo off
REM Builds the v15 additive unit tests (40 cases / 10 executables) with cl.exe.
setlocal enabledelayedexpansion
if not exist build\tests_v15 mkdir build\tests_v15
set COMMON_FLAGS=/nologo /TC /W4 /D_CRT_SECURE_NO_WARNINGS /Iinc /Istubs /Itests
cl %COMMON_FLAGS% tests\test_nc_capability_v15_history_more.c src\nc_capability.c src\system_shared.c /Fe:build\tests_v15\test_nc_capability_v15_history_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_lookahead_v15_modes_arcs_more.c src\nc_lookahead.c src\nc_interp_math.c src\system_shared.c /Fe:build\tests_v15\test_nc_lookahead_v15_modes_arcs_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_precision_v15_modes_service_more.c src\nc_precision.c src\system_shared.c /Fe:build\tests_v15\test_nc_precision_v15_modes_service_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_synchronization_v15_modes_following_more.c src\nc_synchronization.c src\system_shared.c /Fe:build\tests_v15\test_nc_synchronization_v15_modes_following_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_rotary_mcc_v15_service_more.c src\nc_rotary_mcc.c src\nc_codes.c src\system_shared.c /Fe:build\tests_v15\test_nc_rotary_mcc_v15_service_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_lathe_cycle_v15_tracking_more.c src\nc_lathe_cycle.c src\system_shared.c /Fe:build\tests_v15\test_nc_lathe_cycle_v15_tracking_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_safety_motion_v15_latch_release_more.c src\nc_safety_motion.c src\system_shared.c /Fe:build\tests_v15\test_nc_safety_motion_v15_latch_release_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_diagnostics_v15_snapshot_life_more.c src\nc_diagnostics.c src\system_shared.c /Fe:build\tests_v15\test_nc_diagnostics_v15_snapshot_life_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_nc_buffer_v15_wrap_validity_more.c src\nc_buffer.c src\system_shared.c /Fe:build\tests_v15\test_nc_buffer_v15_wrap_validity_more.exe || exit /b 1
cl %COMMON_FLAGS% tests\test_system_shared_v15_event_log_more.c src\system_shared.c /Fe:build\tests_v15\test_system_shared_v15_event_log_more.exe || exit /b 1
echo [build_tests_v15] Build succeeded.
