#include "test_common.h"
#include "system_shared.h"
#include "nc_interpolation.h"
#include "nc_motion_filter.h"

static NC_EXEC_BLOCK make_block(uint32_t line, NC_MOTION_TYPE motion)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.motion_type = motion;
    b.axis_mask = 0x01U;
    b.axis_start[0] = 0;
    b.axis_target[0] = 1000;
    b.feed = 600000;
    b.interp_ticks = 4U;
    b.feed_override_percent = 100U;
    b.feed_profile = NC_FEED_PROFILE_TRAPEZOID;
    return b;
}

static int test_prepare_non_motion_block_is_successful_noop(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_RANGE;
    SystemShared_Initialize();
    NcInterpolation_Reset();
    b = make_block(1961U, NC_MOTION_AUX_ONLY);
    TEST_ASSERT_EQ_INT(0, NcInterpolation_PrepareBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_EQ_UINT(0U, NcInterpolation_HasActive());
    return 0;
}

static int test_begin_rejects_inactive_motion_type(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    NcInterpolation_Reset();
    b = make_block(1962U, NC_MOTION_NONE);
    TEST_ASSERT_EQ_INT(-1, NcInterpolation_BeginBlockRt(&b));
    TEST_ASSERT_EQ_UINT(NC_INTERP_IDLE, g_ncInterpStatus.state);
    return 0;
}

static int test_linear_block_steps_to_endpoint_and_completes(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    NcInterpolation_Reset();
    (void)NcMotionFilter_SetConfigRt(0U, NC_MOTION_FILTER_SECOND_STAGE_FIR, 3U, 3U);
    b = make_block(1963U, NC_MOTION_LINEAR);
    TEST_ASSERT_EQ_INT(0, NcInterpolation_BeginBlockRt(&b));
    for (uint32_t i = 0U; i < 4U; ++i) {
        TEST_ASSERT_EQ_INT(0, NcInterpolation_StepRt());
    }
    TEST_ASSERT_EQ_UINT(0U, NcInterpolation_HasActive());
    TEST_ASSERT_EQ_UINT(NC_INTERP_COMPLETED, g_ncInterpStatus.state);
    TEST_ASSERT_EQ_INT(1000, g_ioOut.axis_target[0]);
    return 0;
}

static int test_skip_motion_completes_early_on_skip_sequence_input(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    NcInterpolation_Reset();
    (void)NcMotionFilter_SetConfigRt(0U, NC_MOTION_FILTER_SECOND_STAGE_FIR, 3U, 3U);
    b = make_block(1964U, NC_MOTION_SKIP);
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
    RUN_TEST(test_prepare_non_motion_block_is_successful_noop);
    RUN_TEST(test_begin_rejects_inactive_motion_type);
    RUN_TEST(test_linear_block_steps_to_endpoint_and_completes);
    RUN_TEST(test_skip_motion_completes_early_on_skip_sequence_input);
    printf("test_nc_interpolation_runtime_more: PASS\n");
    return 0;
}
