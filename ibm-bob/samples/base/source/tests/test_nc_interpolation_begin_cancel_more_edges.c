#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_interpolation.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcInterpolation_Reset();
}

static NC_EXEC_BLOCK make_linear(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.valid = 1U;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_start[0] = 0;
    b.axis_target[0] = 1000;
    b.interp_ticks = 5U;
    b.feed_override_percent = NC_FEED_OVERRIDE_DEFAULT_PERCENT;
    return b;
}

static int test_prepare_and_begin_reject_null_or_inactive_block(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    memset(&b, 0, sizeof(b));
    TEST_ASSERT_EQ_INT(-1, NcInterpolation_PrepareBlockTs(NULL, &err));
    TEST_ASSERT_EQ_INT(-1, NcInterpolation_PrepareBlockTs(&b, NULL));
    TEST_ASSERT_EQ_INT(-1, NcInterpolation_BeginBlockRt(NULL));
    b.motion_type = NC_MOTION_NONE;
    TEST_ASSERT_EQ_INT(-1, NcInterpolation_BeginBlockRt(&b));
    TEST_ASSERT_EQ_UINT(0U, NcInterpolation_HasActive());
    TEST_ASSERT_EQ_INT(0, NcInterpolation_StepRt());
    return 0;
}

static int test_cancel_clears_active_linear_block_and_feed_state(void)
{
    NC_EXEC_BLOCK b = make_linear(721U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcInterpolation_BeginBlockRt(&b));
    TEST_ASSERT_EQ_UINT(1U, NcInterpolation_HasActive());
    TEST_ASSERT_EQ_UINT(NC_INTERP_ACTIVE, g_ncInterpStatus.state);
    NcInterpolation_Cancel();
    TEST_ASSERT_EQ_UINT(0U, NcInterpolation_HasActive());
    TEST_ASSERT_EQ_UINT(NC_INTERP_IDLE, g_ncInterpStatus.state);
    TEST_ASSERT_EQ_UINT(0U, g_ncCycleStatus.active);
    return 0;
}

static int test_dwell_advances_without_servo_command_bits(void)
{
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    b.line_no = 722U;
    b.valid = 1U;
    b.motion_type = NC_MOTION_DWELL;
    b.interp_ticks = 2U;
    b.feed_profile = NC_FEED_PROFILE_DWELL;
    g_ioOut.command_bits = 0U;

    TEST_ASSERT_EQ_INT(0, NcInterpolation_BeginBlockRt(&b));
    TEST_ASSERT_EQ_INT(0, NcInterpolation_StepRt());
    TEST_ASSERT_EQ_UINT(0U, g_ioOut.command_bits & 0x00000003UL);
    TEST_ASSERT_EQ_INT(0, NcInterpolation_StepRt());
    TEST_ASSERT_EQ_UINT(0U, NcInterpolation_HasActive());
    TEST_ASSERT_EQ_UINT(NC_INTERP_COMPLETED, g_ncInterpStatus.state);
    TEST_ASSERT_EQ_UINT(0U, g_ioOut.command_bits & 0x00000003UL);
    return 0;
}

int main(void)
{
    RUN_TEST(test_prepare_and_begin_reject_null_or_inactive_block);
    RUN_TEST(test_cancel_clears_active_linear_block_and_feed_state);
    RUN_TEST(test_dwell_advances_without_servo_command_bits);
    printf("test_nc_interpolation_begin_cancel_more_edges: PASS\n");
    return 0;
}
