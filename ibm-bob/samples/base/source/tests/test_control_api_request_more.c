#include "test_common.h"
#include "system_shared.h"
#include "control_api.h"
#include "nc_program.h"

static void setup(void)
{
    SystemShared_Initialize();
}

static NC_EXEC_BLOCK make_block(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_target[0] = 100;
    b.feed_override_percent = 100U;
    return b;
}

static int test_invalid_run_mode_request_sets_parameter_error_response(void)
{
    setup();
    TEST_ASSERT_EQ_INT(-1, Api_RequestRunModeChange(99U));
    TEST_ASSERT_EQ_UINT(RESPONSE_PARAM_ERROR, g_uiRequest.mode_response);
    TEST_ASSERT_EQ_UINT(0U, g_uiRequest.mode_change_request);
    return 0;
}

static int test_program_load_rejects_null_and_manual_mode_path(void)
{
    setup();
    TEST_ASSERT_EQ_INT(-1, Api_RequestNcProgramLoad(NULL));
    TEST_ASSERT_EQ_UINT(RESPONSE_PARAM_ERROR, g_ncProgramStatus.response_code);
    TEST_ASSERT_EQ_INT(-1, Api_RequestNcProgramLoad("sample.nc"));
    TEST_ASSERT_EQ_UINT(RESPONSE_REJECTED, g_ncProgramStatus.response_code);
    return 0;
}

static int test_binary_load_accepts_auto_idle_and_updates_binary_status(void)
{
    NC_EXEC_BLOCK blocks[2];
    setup();
    blocks[0] = make_block(1661U);
    blocks[1] = make_block(1662U);
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    TEST_ASSERT_EQ_INT(0, Api_RequestNcBinaryProgramLoad(blocks, 2U));
    TEST_ASSERT_EQ_UINT(RESPONSE_ACCEPTED, g_ncProgramStatus.response_code);
    TEST_ASSERT_EQ_UINT(2U, g_ncBinaryProgramRequest.accepted_count);
    TEST_ASSERT_EQ_UINT(1U, g_ncBinaryProgramRequest.load_request);
    return 0;
}

static int test_status_getters_reject_null_pointers_without_mutating_source_state(void)
{
    uint8_t ready = 99U;
    uint16_t generation = 99U;
    setup();
    g_prefetchReq.ready = 1U;
    g_prefetchReq.generation = 7U;
    TEST_ASSERT_EQ_INT(-1, Api_GetStatusSnapshot(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcProgramStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetPrefetchStatus(NULL, &generation));
    TEST_ASSERT_EQ_INT(-1, Api_GetPrefetchStatus(&ready, NULL));
    TEST_ASSERT_EQ_INT(0, Api_GetPrefetchStatus(&ready, &generation));
    TEST_ASSERT_EQ_UINT(1U, ready);
    TEST_ASSERT_EQ_UINT(7U, generation);
    return 0;
}

int main(void)
{
    RUN_TEST(test_invalid_run_mode_request_sets_parameter_error_response);
    RUN_TEST(test_program_load_rejects_null_and_manual_mode_path);
    RUN_TEST(test_binary_load_accepts_auto_idle_and_updates_binary_status);
    RUN_TEST(test_status_getters_reject_null_pointers_without_mutating_source_state);
    printf("test_control_api_request_more: PASS\n");
    return 0;
}
