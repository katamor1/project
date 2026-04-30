#include "test_common.h"
#include "stub_external_io.h"
#include "control_api.h"

static int test_run_mode_change_valid_and_invalid(void)
{
    Test_ResetSystem();

    TEST_ASSERT_EQ_INT(0, Api_RequestRunModeChange((uint8_t)RUN_MODE_MAINT));
    TEST_ASSERT_EQ_UINT(1U, g_uiRequest.mode_change_request);
    TEST_ASSERT_EQ_UINT((uint8_t)RUN_MODE_MAINT, g_uiRequest.requested_mode);
    TEST_ASSERT_EQ_UINT(RESPONSE_ACCEPTED, g_uiRequest.mode_response);

    TEST_ASSERT_EQ_INT(-1, Api_RequestRunModeChange((uint8_t)(RUN_MODE_MAINT + 1U)));
    TEST_ASSERT_EQ_UINT(RESPONSE_PARAM_ERROR, g_uiRequest.mode_response);
    return 0;
}

static int test_status_and_prefetch_getters_validate_null_and_copy_snapshot(void)
{
    STATUS_SNAPSHOT snapshot;
    uint8_t ready = 0U;
    uint16_t generation = 0U;

    Test_ResetSystem();
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_machineCtx.machine_state = MACHINE_RUNNING;
    g_machineCtx.alarm_code = 1234U;
    g_machineCtx.cycle_count = 55U;
    Test_SetCommandBits(0xA5A5U);
    Test_SetSensorBits(0x00F0U);
    g_uiRequest.mode_response = RESPONSE_COMPLETED;
    g_prefetchReq.ready = 1U;
    g_prefetchReq.generation = 77U;

    TEST_ASSERT_EQ_INT(-1, Api_GetStatusSnapshot(NULL));
    TEST_ASSERT_EQ_INT(0, Api_GetStatusSnapshot(&snapshot));
    TEST_ASSERT_EQ_UINT(RUN_MODE_AUTO, snapshot.run_mode);
    TEST_ASSERT_EQ_UINT(MACHINE_RUNNING, snapshot.machine_state);
    TEST_ASSERT_EQ_UINT(1234U, snapshot.alarm_code);
    TEST_ASSERT_EQ_UINT(55U, snapshot.cycle_count);
    TEST_ASSERT_EQ_UINT(0xA5A5U, snapshot.command_bits);
    TEST_ASSERT_EQ_UINT(0x00F0U, snapshot.sensor_bits);
    TEST_ASSERT_EQ_UINT(RESPONSE_COMPLETED, snapshot.mode_response);
    TEST_ASSERT_EQ_UINT(1U, snapshot.prefetch_ready);
    TEST_ASSERT_EQ_UINT(77U, snapshot.prefetch_generation);

    TEST_ASSERT_EQ_INT(-1, Api_GetPrefetchStatus(NULL, &generation));
    TEST_ASSERT_EQ_INT(-1, Api_GetPrefetchStatus(&ready, NULL));
    TEST_ASSERT_EQ_INT(0, Api_GetPrefetchStatus(&ready, &generation));
    TEST_ASSERT_EQ_UINT(1U, ready);
    TEST_ASSERT_EQ_UINT(77U, generation);
    return 0;
}

static int test_nc_program_load_rejects_null_empty_long_manual_and_busy(void)
{
    char tooLong[NC_FILE_PATH_MAX + 2U];

    Test_ResetSystem();
    TEST_ASSERT_EQ_INT(-1, Api_RequestNcProgramLoad(NULL));
    TEST_ASSERT_EQ_UINT(RESPONSE_PARAM_ERROR, g_ncProgramStatus.response_code);

    TEST_ASSERT_EQ_INT(-1, Api_RequestNcProgramLoad(""));
    TEST_ASSERT_EQ_UINT(RESPONSE_REJECTED, g_ncProgramStatus.response_code);

    TEST_ASSERT_EQ_INT(-1, Api_RequestNcProgramLoad("program.nc"));
    TEST_ASSERT_EQ_UINT(RESPONSE_REJECTED, g_ncProgramStatus.response_code);

    Test_ResetSystem();
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    memset(tooLong, 'A', sizeof(tooLong));
    tooLong[NC_FILE_PATH_MAX] = '\0';
    TEST_ASSERT_EQ_INT(-1, Api_RequestNcProgramLoad(tooLong));
    TEST_ASSERT_EQ_UINT(RESPONSE_REJECTED, g_ncProgramStatus.response_code);

    Test_ResetSystem();
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_ncProgramStatus.state = NC_RUNNING;
    TEST_ASSERT_EQ_INT(-1, Api_RequestNcProgramLoad("program.nc"));
    TEST_ASSERT_EQ_UINT(RESPONSE_REJECTED, g_ncProgramStatus.response_code);
    return 0;
}

static int test_nc_program_load_accepts_auto_idle_and_sets_request(void)
{
    Test_ResetSystem();
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_ncProgramStatus.state = NC_IDLE;

    TEST_ASSERT_EQ_INT(0, Api_RequestNcProgramLoad("sample.nc"));
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramRequest.load_request);
    TEST_ASSERT_EQ_UINT(0U, g_ncProgramRequest.start_request);
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramRequest.request_seq);
    TEST_ASSERT_EQ_UINT(RESPONSE_ACCEPTED, g_ncProgramStatus.response_code);
    TEST_ASSERT_EQ_STR("sample.nc", (const char*)g_ncProgramRequest.file_path);
    return 0;
}

static int test_nc_program_state_transition_requests(void)
{
    Test_ResetSystem();

    TEST_ASSERT_EQ_INT(-1, Api_RequestNcProgramStart());
    TEST_ASSERT_EQ_UINT(RESPONSE_REJECTED, g_ncProgramStatus.response_code);

    g_ncProgramStatus.state = NC_READY;
    TEST_ASSERT_EQ_INT(0, Api_RequestNcProgramStart());
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramRequest.start_request);
    TEST_ASSERT_EQ_UINT(RESPONSE_ACCEPTED, g_ncProgramStatus.response_code);

    Test_ResetSystem();
    TEST_ASSERT_EQ_INT(-1, Api_RequestNcProgramHold());
    g_ncProgramStatus.state = NC_RUNNING;
    TEST_ASSERT_EQ_INT(0, Api_RequestNcProgramHold());
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramRequest.hold_request);

    Test_ResetSystem();
    TEST_ASSERT_EQ_INT(-1, Api_RequestNcProgramResume());
    g_ncProgramStatus.state = NC_HOLD;
    TEST_ASSERT_EQ_INT(0, Api_RequestNcProgramResume());
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramRequest.resume_request);

    Test_ResetSystem();
    TEST_ASSERT_EQ_INT(0, Api_RequestNcProgramStop());
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramRequest.stop_request);
    TEST_ASSERT_EQ_UINT(RESPONSE_ACCEPTED, g_ncProgramStatus.response_code);
    return 0;
}

static int test_api_snapshot_getters_reject_null(void)
{
    Test_ResetSystem();
    TEST_ASSERT_EQ_INT(-1, Api_GetNcProgramStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcInterpolationStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcFeedStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcCoordinateStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcCoordinateTransformStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcFeatureStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetPrestartInterlockStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcAuxStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcCycleStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetIoTraceBuffer(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcCompensationStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcPathControlStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcKinematicsStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcAxisLoadStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcToolLifeStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcDiagnosticSnapshot(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcMotionFilterStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcEventRing(NULL));
    return 0;
}

int main(void)
{
    RUN_TEST(test_run_mode_change_valid_and_invalid);
    RUN_TEST(test_status_and_prefetch_getters_validate_null_and_copy_snapshot);
    RUN_TEST(test_nc_program_load_rejects_null_empty_long_manual_and_busy);
    RUN_TEST(test_nc_program_load_accepts_auto_idle_and_sets_request);
    RUN_TEST(test_nc_program_state_transition_requests);
    RUN_TEST(test_api_snapshot_getters_reject_null);
    printf("[  PASSED  ] test_control_api\n");
    return 0;
}
