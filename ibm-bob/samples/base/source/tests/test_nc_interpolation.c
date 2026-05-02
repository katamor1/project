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
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_start[0] = 0;
    b.axis_target[0] = 1000;
    b.feed = 1000;
    b.interp_ticks = 5U;
    b.feed_override_percent = 100U;
    b.feed_profile = NC_FEED_PROFILE_TRAPEZOID;
    return b;
}

static int test_prepare_null_and_noop_block(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(-1, NcInterpolation_PrepareBlockTs(NULL, &err));
    TEST_ASSERT_EQ_INT(-1, NcInterpolation_PrepareBlockTs(&b, NULL));
    memset(&b, 0, sizeof(b));
    b.motion_type = NC_MOTION_NONE;
    TEST_ASSERT_EQ_INT(0, NcInterpolation_PrepareBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    return 0;
}

static int test_linear_begin_step_and_completion(void)
{
    NC_EXEC_BLOCK b;
    uint32_t i;
    setup();
    b = make_linear(31U);
    TEST_ASSERT_EQ_INT(0, NcInterpolation_BeginBlockRt(&b));
    TEST_ASSERT_EQ_UINT(1U, NcInterpolation_HasActive());
    TEST_ASSERT_EQ_UINT(NC_INTERP_ACTIVE, g_ncInterpStatus.state);
    for (i = 0U; i < b.interp_ticks; ++i) {
        TEST_ASSERT_EQ_INT(0, NcInterpolation_StepRt());
    }
    TEST_ASSERT_EQ_UINT(0U, NcInterpolation_HasActive());
    TEST_ASSERT_EQ_UINT(NC_INTERP_COMPLETED, g_ncInterpStatus.state);
    TEST_ASSERT_EQ_INT(1000, g_ioOut.axis_target[0]);
    TEST_ASSERT_TRUE((g_ioOut.command_bits & 0x00000003UL) == 0x00000003UL);
    return 0;
}

static int test_cancel_resets_active_state(void)
{
    NC_EXEC_BLOCK b;
    setup();
    b = make_linear(32U);
    TEST_ASSERT_EQ_INT(0, NcInterpolation_BeginBlockRt(&b));
    NcInterpolation_Cancel();
    TEST_ASSERT_EQ_UINT(0U, NcInterpolation_HasActive());
    TEST_ASSERT_EQ_UINT(NC_INTERP_IDLE, g_ncInterpStatus.state);
    TEST_ASSERT_EQ_UINT(0U, g_ncCycleStatus.active);
    return 0;
}

static int test_skip_input_completes_skip_block_early(void)
{
    NC_EXEC_BLOCK b;
    setup();
    b = make_linear(33U);
    b.motion_type = NC_MOTION_SKIP;
    b.g_code10 = (uint16_t)NC_G_CODE_WHOLE(31);
    b.interp_ticks = 10U;
    TEST_ASSERT_EQ_INT(0, NcInterpolation_BeginBlockRt(&b));
    g_ioIn.seq_bits = SENSOR_SEQ_SKIP_BIT;
    TEST_ASSERT_EQ_INT(0, NcInterpolation_StepRt());
    TEST_ASSERT_EQ_UINT(0U, NcInterpolation_HasActive());
    TEST_ASSERT_EQ_UINT(1U, g_ncCycleStatus.skip_triggered);
    TEST_ASSERT_EQ_UINT(1U, g_ncCycleStatus.completed_skips);
    return 0;
}

int main(void)
{
    RUN_TEST(test_prepare_null_and_noop_block);
    RUN_TEST(test_linear_begin_step_and_completion);
    RUN_TEST(test_cancel_resets_active_state);
    RUN_TEST(test_skip_input_completes_skip_block_early);
    printf("test_nc_interpolation: PASS\n");
    return 0;
}
