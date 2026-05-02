#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_motion_filter.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcProgram_ClearBuffer();
    NcMotionFilter_ResetRt();
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_machineCtx.machine_state = MACHINE_READY;
}

static NC_EXEC_BLOCK make_feature_block(uint32_t lineNo, uint16_t gCode10, uint32_t flags, uint32_t pWord)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_NONE;
    b.g_code10 = gCode10;
    b.feature_flags = flags;
    b.p_word = pWord;
    return b;
}

static int load_and_start(const NC_EXEC_BLOCK* blocks, uint32_t count)
{
    TEST_ASSERT_EQ_INT(0, NcProgram_RequestBinaryLoad(blocks, count));
    TsNcProgram_ExecuteSlice();
    g_ncProgramRequest.start_request = 1U;
    RtNcProgram_ProcessRequests(1U);
    TEST_ASSERT_EQ_UINT(NC_RUNNING, g_ncProgramStatus.state);
    return 0;
}

static int test_g08_g05_feature_status_enable_and_cancel(void)
{
    NC_EXEC_BLOCK blocks[4];

    setup();
    blocks[0] = make_feature_block(10U, (uint16_t)NC_G_CODE_WHOLE(8), NC_FEATURE_FLAG_PARAM_P, 1U);
    blocks[1] = make_feature_block(11U, (uint16_t)NC_G_CODE_WHOLE(8), NC_FEATURE_FLAG_PARAM_P, 2U);
    blocks[2] = make_feature_block(12U, (uint16_t)NC_G_CODE_WHOLE(5), NC_FEATURE_FLAG_PARAM_P, 10000U);
    blocks[3] = make_feature_block(13U, (uint16_t)NC_G_CODE_WHOLE(5), NC_FEATURE_FLAG_PARAM_P, 0U);

    TEST_ASSERT_EQ_INT(0, load_and_start(blocks, 4U));
    RtNcProgram_ConsumeBlocks();

    TEST_ASSERT_EQ_UINT(4U, g_ncFeatureStatus.executed_feature_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncFeatureStatus.g08_standard_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncFeatureStatus.g08_high_precision_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncFeatureStatus.hpcc_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncFeatureStatus.hpcc_cancel_blocks);
    TEST_ASSERT_EQ_UINT(0U, g_ncFeatureStatus.hpcc_enabled);
    TEST_ASSERT_EQ_UINT(0U, g_ncFeatureStatus.smoothing_enabled);
    TEST_ASSERT_EQ_UINT(13U, g_ncFeatureStatus.last_line_no);
    return 0;
}

static int test_feature_status_sync_compound_overlay_and_cancel(void)
{
    NC_EXEC_BLOCK blocks[6];

    setup();
    blocks[0] = make_feature_block(20U, (uint16_t)NC_G_CODE10(51, 4), NC_FEATURE_FLAG_SYNC, 0U);
    blocks[1] = make_feature_block(21U, (uint16_t)NC_G_CODE10(50, 4), NC_FEATURE_FLAG_SYNC, 0U);
    blocks[2] = make_feature_block(22U, (uint16_t)NC_G_CODE10(51, 5), NC_FEATURE_FLAG_COMPOUND, 0U);
    blocks[3] = make_feature_block(23U, (uint16_t)NC_G_CODE10(50, 5), NC_FEATURE_FLAG_COMPOUND, 0U);
    blocks[4] = make_feature_block(24U, (uint16_t)NC_G_CODE10(51, 6), NC_FEATURE_FLAG_OVERLAY, 0U);
    blocks[5] = make_feature_block(25U, (uint16_t)NC_G_CODE10(50, 6), NC_FEATURE_FLAG_OVERLAY, 0U);

    TEST_ASSERT_EQ_INT(0, load_and_start(blocks, 6U));
    RtNcProgram_ConsumeBlocks();

    TEST_ASSERT_EQ_UINT(0U, g_ncFeatureStatus.sync_enabled);
    TEST_ASSERT_EQ_UINT(0U, g_ncFeatureStatus.compound_enabled);
    TEST_ASSERT_EQ_UINT(0U, g_ncFeatureStatus.overlay_enabled);
    TEST_ASSERT_EQ_UINT(6U, g_ncFeatureStatus.executed_feature_blocks);
    TEST_ASSERT_EQ_UINT(25U, g_ncFeatureStatus.last_line_no);
    return 0;
}

static int test_aux_wait_mfin_holds_then_completes(void)
{
    NC_EXEC_BLOCK block;

    setup();
    block = make_feature_block(30U, 0U, 0U, 0U);
    block.motion_type = NC_MOTION_AUX_ONLY;
    block.aux_flags = NC_AUX_FLAG_M_CODE | NC_AUX_FLAG_MFIN_WAIT;
    block.aux_m_code = 6U;

    TEST_ASSERT_EQ_INT(0, load_and_start(&block, 1U));
    RtNcProgram_ConsumeBlocks();
    TEST_ASSERT_EQ_UINT(NC_AUX_STATE_WAIT_MFIN, g_ncAuxStatus.state);
    TEST_ASSERT_EQ_UINT(6U, g_ncAuxStatus.active_m_code);

    RtNcProgram_ConsumeBlocks();
    TEST_ASSERT_EQ_UINT(NC_AUX_STATE_WAIT_MFIN, g_ncAuxStatus.state);
    TEST_ASSERT_EQ_UINT(1U, g_ncAuxStatus.wait_cycles);

    g_ioIn.seq_bits = SENSOR_SEQ_MFIN_BIT;
    RtNcProgram_ConsumeBlocks();
    TEST_ASSERT_EQ_UINT(NC_AUX_STATE_IDLE, g_ncAuxStatus.state);
    TEST_ASSERT_EQ_UINT(1U, g_ncAuxStatus.completed_m_codes);
    TEST_ASSERT_EQ_UINT(1U, g_ncAuxStatus.last_wait_cycles);
    return 0;
}

int main(void)
{
    RUN_TEST(test_g08_g05_feature_status_enable_and_cancel);
    RUN_TEST(test_feature_status_sync_compound_overlay_and_cancel);
    RUN_TEST(test_aux_wait_mfin_holds_then_completes);
    printf("test_nc_program_feature_status: PASS\n");
    return 0;
}
