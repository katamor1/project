#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_motion_filter.h"

#define TEST_CMD_ENABLE_SERVO  (0x00000001UL)
#define TEST_CMD_START_MOTION  (0x00000002UL)

static void setup(void)
{
    SystemShared_Initialize();
    NcProgram_ClearBuffer();
    NcMotionFilter_ResetRt();
}

static NC_EXEC_BLOCK make_aux_block(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_AUX_ONLY;
    b.axis_mask = 0x01U;
    b.axis_target[0] = 12345;
    b.spindle_speed = 1500U;
    b.tool_no = 4U;
    b.aux_m_code = 130U;
    b.aux_flags = NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_SPINDLE | NC_AUX_FLAG_TOOL;
    return b;
}

static int test_binary_load_validation_and_ts_commit(void)
{
    NC_EXEC_BLOCK blocks[2];

    setup();
    blocks[0] = make_aux_block(10U);
    memset(&blocks[1], 0, sizeof(blocks[1]));
    blocks[1].valid = 1U;
    blocks[1].line_no = 11U;
    blocks[1].motion_type = NC_MOTION_NONE;

    TEST_ASSERT_EQ_INT(-1, NcProgram_RequestBinaryLoad(NULL, 1U));
    TEST_ASSERT_EQ_INT(-1, NcProgram_RequestBinaryLoad(blocks, 0U));
    TEST_ASSERT_EQ_INT(-1, NcProgram_RequestBinaryLoad(blocks, NC_BINARY_PROGRAM_MAX_BLOCKS + 1U));

    TEST_ASSERT_EQ_INT(0, NcProgram_RequestBinaryLoad(blocks, 2U));
    TEST_ASSERT_EQ_UINT(1U, g_ncBinaryProgramRequest.load_request);
    TEST_ASSERT_EQ_UINT(2U, g_ncBinaryProgramRequest.requested_count);

    TsNcProgram_ExecuteSlice();
    TEST_ASSERT_EQ_UINT(0U, g_ncBinaryProgramRequest.load_request);
    TEST_ASSERT_EQ_UINT(1U, g_ncBinaryProgramRequest.ready);
    TEST_ASSERT_EQ_UINT(2U, g_ncBinaryProgramStatus.loaded_blocks);
    TEST_ASSERT_EQ_UINT(2U, g_ncBinaryProgramStatus.committed_blocks);
    TEST_ASSERT_EQ_UINT(2U, NcBuffer_Count());
    TEST_ASSERT_EQ_UINT(NC_READY, g_ncProgramStatus.state);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_BINARY_LOAD_READY, g_ncEventRing.items[0].code);
    return 0;
}

static int test_rt_start_rejects_then_accepts_and_consumes_aux(void)
{
    NC_EXEC_BLOCK block = make_aux_block(20U);

    setup();
    TEST_ASSERT_EQ_INT(0, NcProgram_RequestBinaryLoad(&block, 1U));
    TsNcProgram_ExecuteSlice();

    g_machineCtx.run_mode = RUN_MODE_MANUAL;
    g_machineCtx.machine_state = MACHINE_READY;
    g_ncProgramRequest.start_request = 1U;
    RtNcProgram_ProcessRequests(1U);
    TEST_ASSERT_EQ_UINT(NC_READY, g_ncProgramStatus.state);
    TEST_ASSERT_EQ_UINT(RESPONSE_REJECTED, g_ncProgramStatus.response_code);

    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_ncProgramRequest.start_request = 1U;
    RtNcProgram_ProcessRequests(0U);
    TEST_ASSERT_EQ_UINT(NC_READY, g_ncProgramStatus.state);
    TEST_ASSERT_EQ_UINT(RESPONSE_REJECTED, g_ncProgramStatus.response_code);

    g_ncProgramRequest.start_request = 1U;
    RtNcProgram_ProcessRequests(1U);
    TEST_ASSERT_EQ_UINT(NC_RUNNING, g_ncProgramStatus.state);
    TEST_ASSERT_EQ_UINT(MACHINE_RUNNING, g_machineCtx.machine_state);
    TEST_ASSERT_EQ_UINT(RESPONSE_COMPLETED, g_ncProgramStatus.response_code);

    RtNcProgram_ConsumeBlocks();
    TEST_ASSERT_EQ_UINT(NC_COMPLETED, g_ncProgramStatus.state);
    TEST_ASSERT_EQ_UINT(MACHINE_READY, g_machineCtx.machine_state);
    TEST_ASSERT_TRUE((g_ioOut.command_bits & (TEST_CMD_ENABLE_SERVO | TEST_CMD_START_MOTION)) != 0U);
    TEST_ASSERT_EQ_INT(12345, g_ioOut.axis_target[0]);
    TEST_ASSERT_EQ_UINT(1500U, g_ioOut.spindle_speed);
    TEST_ASSERT_EQ_UINT(4U, g_ioOut.tool_no);
    TEST_ASSERT_EQ_UINT(130U, g_ioOut.aux_m_code);
    return 0;
}

static int test_hold_resume_stop_and_alarm_branches(void)
{
    setup();
    g_ncProgramStatus.state = NC_RUNNING;
    g_ncProgramRequest.hold_request = 1U;
    RtNcProgram_ProcessRequests(1U);
    TEST_ASSERT_EQ_UINT(NC_HOLD, g_ncProgramStatus.state);
    TEST_ASSERT_EQ_UINT(RESPONSE_COMPLETED, g_ncProgramStatus.response_code);

    g_ncProgramRequest.resume_request = 1U;
    RtNcProgram_ProcessRequests(0U);
    TEST_ASSERT_EQ_UINT(NC_HOLD, g_ncProgramStatus.state);

    g_ncProgramRequest.resume_request = 1U;
    RtNcProgram_ProcessRequests(1U);
    TEST_ASSERT_EQ_UINT(NC_RUNNING, g_ncProgramStatus.state);

    g_ncProgramRequest.stop_request = 1U;
    RtNcProgram_ProcessRequests(1U);
    TEST_ASSERT_EQ_UINT(NC_COMPLETED, g_ncProgramStatus.state);

    g_ncProgramStatus.state = NC_RUNNING;
    g_ncProgramStatus.exec_line_no = 77U;
    g_machineCtx.machine_state = MACHINE_ALARM;
    RtNcProgram_ProcessRequests(1U);
    TEST_ASSERT_EQ_UINT(NC_ERROR, g_ncProgramStatus.state);
    TEST_ASSERT_EQ_UINT(NC_ERR_INTERLOCK, g_ncProgramStatus.error_code);
    TEST_ASSERT_EQ_UINT(77U, g_ncProgramStatus.error_line_no);
    return 0;
}

int main(void)
{
    RUN_TEST(test_binary_load_validation_and_ts_commit);
    RUN_TEST(test_rt_start_rejects_then_accepts_and_consumes_aux);
    RUN_TEST(test_hold_resume_stop_and_alarm_branches);
    printf("test_nc_program_binary_flow: PASS\n");
    return 0;
}
