#include "test_common.h"
#include "system_shared.h"
#include "control_api.h"

static NC_EXEC_BLOCK make_block(uint32_t line)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_target[0] = (int32_t)line;
    return b;
}

static int test_common_getters_reject_null_outputs(void)
{
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(-1, Api_GetStatusSnapshot(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetPrefetchStatus(NULL, NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcProgramStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcAxisConfigStatus(NULL));
    return 0;
}

static int test_program_load_rejects_non_auto_and_accepts_auto_idle_path(void)
{
    SystemShared_Initialize();
    g_machineCtx.run_mode = RUN_MODE_MANUAL;
    TEST_ASSERT_EQ_INT(-1, Api_RequestNcProgramLoad("demo.nc"));
    TEST_ASSERT_EQ_UINT(RESPONSE_REJECTED, g_ncProgramStatus.response_code);
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_ncProgramStatus.state = NC_IDLE;
    TEST_ASSERT_EQ_INT(0, Api_RequestNcProgramLoad("demo.nc"));
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramRequest.load_request);
    TEST_ASSERT_EQ_STR("demo.nc", (const char*)g_ncProgramRequest.file_path);
    TEST_ASSERT_EQ_UINT(RESPONSE_ACCEPTED, g_ncProgramStatus.response_code);
    return 0;
}

static int test_start_hold_resume_requests_are_state_gated(void)
{
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(-1, Api_RequestNcProgramStart());
    TEST_ASSERT_EQ_UINT(RESPONSE_REJECTED, g_ncProgramStatus.response_code);
    g_ncProgramStatus.state = NC_READY;
    TEST_ASSERT_EQ_INT(0, Api_RequestNcProgramStart());
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramRequest.start_request);
    g_ncProgramStatus.state = NC_RUNNING;
    TEST_ASSERT_EQ_INT(0, Api_RequestNcProgramHold());
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramRequest.hold_request);
    g_ncProgramStatus.state = NC_HOLD;
    TEST_ASSERT_EQ_INT(0, Api_RequestNcProgramResume());
    TEST_ASSERT_EQ_UINT(1U, g_ncProgramRequest.resume_request);
    return 0;
}

static int test_binary_program_and_axis_config_api_wrappers_update_shared_state(void)
{
    NC_EXEC_BLOCK blocks[2];
    NC_AXIS_CONFIG_STATUS st;
    SystemShared_Initialize();
    blocks[0] = make_block(1981U);
    blocks[1] = make_block(1982U);
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_ncProgramStatus.state = NC_IDLE;
    TEST_ASSERT_EQ_INT(0, Api_RequestNcBinaryProgramLoad(blocks, 2U));
    TEST_ASSERT_EQ_UINT(1U, g_ncBinaryProgramRequest.load_request);
    TEST_ASSERT_EQ_UINT(2U, g_ncBinaryProgramRequest.requested_count);
    TEST_ASSERT_EQ_INT(0, Api_SetNcAxisDefinition(3U, (uint8_t)'b', NC_AXIS_TYPE_ROTARY, 1U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcAxisDetachedMask(0x08U));
    TEST_ASSERT_EQ_INT(0, Api_GetNcAxisConfigStatus(&st));
    TEST_ASSERT_EQ_UINT((uint8_t)'B', st.axis_name[3]);
    TEST_ASSERT_EQ_UINT(0x08U, st.detached_axis_mask);
    return 0;
}

int main(void)
{
    RUN_TEST(test_common_getters_reject_null_outputs);
    RUN_TEST(test_program_load_rejects_non_auto_and_accepts_auto_idle_path);
    RUN_TEST(test_start_hold_resume_requests_are_state_gated);
    RUN_TEST(test_binary_program_and_axis_config_api_wrappers_update_shared_state);
    printf("test_control_api_v19_gateways_more: PASS\n");
    return 0;
}
