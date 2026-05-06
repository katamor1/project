#include "test_common.h"
#include "system_shared.h"
#include "nc_interpolation.h"
#include "nc_program.h"

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
    b.feed = 100000;
    b.feed_override_percent = 100U;
    b.interp_ticks = 4U;
    return b;
}

static int test_begin_rejects_null_and_inactive_blocks(void)
{
    NC_EXEC_BLOCK b = make_linear(1641U);
    setup();
    TEST_ASSERT_EQ_INT(-1, NcInterpolation_BeginBlockRt(NULL));
    b.motion_type = NC_MOTION_NONE;
    TEST_ASSERT_EQ_INT(-1, NcInterpolation_BeginBlockRt(&b));
    TEST_ASSERT_EQ_UINT(0U, NcInterpolation_HasActive());
    return 0;
}

static int test_linear_step_reaches_final_target_and_completes(void)
{
    NC_EXEC_BLOCK b = make_linear(1642U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcInterpolation_BeginBlockRt(&b));
    TEST_ASSERT_EQ_UINT(1U, NcInterpolation_HasActive());
    for (uint32_t i = 0U; i < 4U; ++i) {
        TEST_ASSERT_EQ_INT(0, NcInterpolation_StepRt());
    }
    TEST_ASSERT_EQ_UINT(0U, NcInterpolation_HasActive());
    TEST_ASSERT_EQ_UINT(NC_INTERP_COMPLETED, g_ncInterpStatus.state);
    TEST_ASSERT_EQ_INT(1000, g_ioOut.axis_target[0]);
    TEST_ASSERT_TRUE((g_ioOut.command_bits & 0x00000003UL) == 0x00000003UL);
    return 0;
}

static int test_cancel_after_begin_clears_active_and_cycle_state(void)
{
    NC_EXEC_BLOCK b = make_linear(1643U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcInterpolation_BeginBlockRt(&b));
    NcInterpolation_Cancel();
    TEST_ASSERT_EQ_UINT(0U, NcInterpolation_HasActive());
    TEST_ASSERT_EQ_UINT(NC_INTERP_IDLE, g_ncInterpStatus.state);
    TEST_ASSERT_EQ_UINT(0U, g_ncCycleStatus.active);
    return 0;
}

static int test_dwell_step_does_not_start_motion_command_bits(void)
{
    NC_EXEC_BLOCK b = make_linear(1644U);
    setup();
    b.motion_type = NC_MOTION_DWELL;
    b.interp_ticks = 2U;
    g_ioOut.command_bits = 0U;
    TEST_ASSERT_EQ_INT(0, NcInterpolation_BeginBlockRt(&b));
    TEST_ASSERT_EQ_INT(0, NcInterpolation_StepRt());
    TEST_ASSERT_TRUE((g_ioOut.command_bits & 0x00000003UL) == 0U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_begin_rejects_null_and_inactive_blocks);
    RUN_TEST(test_linear_step_reaches_final_target_and_completes);
    RUN_TEST(test_cancel_after_begin_clears_active_and_cycle_state);
    RUN_TEST(test_dwell_step_does_not_start_motion_command_bits);
    printf("test_nc_interpolation_step_more: PASS\n");
    return 0;
}
