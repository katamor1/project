#include "test_common.h"
#include "stub_external_io.h"
#include "nc_feed.h"

/* Internal TS helpers are intentionally unit-tested through explicit externs. */
extern int32_t NcFeed_PrepareMotionTs(NC_EXEC_BLOCK* pBlock, double pathLengthUnits, NC_ERROR_CODE* pOutError);
extern int32_t NcFeed_PrepareDwellTs(NC_EXEC_BLOCK* pBlock, uint32_t dwellTicks, NC_ERROR_CODE* pOutError);

static int test_feed_reset_override_validation_and_generation(void)
{
    Test_ResetSystem();
    NcFeed_Reset();

    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_IDLE, g_ncFeedStatus.state);
    TEST_ASSERT_EQ_UINT(NC_FEED_OVERRIDE_DEFAULT_PERCENT, g_ncFeedStatus.feed_override_percent);

    TEST_ASSERT_EQ_INT(-1, NcFeed_SetOverride((uint16_t)(NC_FEED_OVERRIDE_MAX_PERCENT + 1U)));
    TEST_ASSERT_EQ_UINT(NC_FEED_OVERRIDE_DEFAULT_PERCENT, g_ncFeedStatus.feed_override_percent);

    TEST_ASSERT_EQ_INT(0, NcFeed_SetOverride(150U));
    TEST_ASSERT_EQ_UINT(150U, g_ncFeedStatus.feed_override_percent);
    TEST_ASSERT_TRUE(g_ncFeedStatus.generation > 0U);
    return 0;
}

static int test_prepare_motion_rejects_null_bad_feed_negative_path_and_per_rev_without_spindle(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;

    Test_ResetSystem();
    NcFeed_Reset();
    block = Test_MakeMotionBlock(10U, NC_MOTION_LINEAR, 0x1U, 1000, 0, 0, 0);
    block.feed = 0;

    TEST_ASSERT_EQ_INT(-1, NcFeed_PrepareMotionTs(NULL, 1.0, &err));
    TEST_ASSERT_EQ_INT(-1, NcFeed_PrepareMotionTs(&block, 1.0, NULL));
    TEST_ASSERT_EQ_INT(-1, NcFeed_PrepareMotionTs(&block, 1.0, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_FEED, err);

    block.feed = 1000;
    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(-1, NcFeed_PrepareMotionTs(&block, -0.1, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_FEED, err);

    block.feed_mode = NC_FEED_MODE_PER_REV;
    block.spindle_speed = 0U;
    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(-1, NcFeed_PrepareMotionTs(&block, 1.0, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_FEED, err);
    return 0;
}

static int test_prepare_motion_normal_and_override_zero_hold_path(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;

    Test_ResetSystem();
    NcFeed_Reset();
    block = Test_MakeMotionBlock(20U, NC_MOTION_LINEAR, 0x1U, 1000, 0, 0, 0);
    block.feed = 600000; /* 60 units/min at NC_POSITION_SCALE=10000 */

    TEST_ASSERT_EQ_INT(0, NcFeed_PrepareMotionTs(&block, 1.0, &err));
    TEST_ASSERT_EQ_UINT(NC_FEED_PROFILE_TRAPEZOID, block.feed_profile);
    TEST_ASSERT_EQ_UINT(NC_FEED_OVERRIDE_DEFAULT_PERCENT, block.feed_override_percent);
    TEST_ASSERT_TRUE(block.interp_ticks >= NC_INTERP_MIN_TICKS);
    TEST_ASSERT_TRUE(block.interp_ticks <= NC_INTERP_MAX_TICKS);

    TEST_ASSERT_EQ_INT(0, NcFeed_SetOverride(0U));
    TEST_ASSERT_EQ_INT(0, NcFeed_PrepareMotionTs(&block, 1.0, &err));
    TEST_ASSERT_EQ_UINT(NC_INTERP_MIN_TICKS, block.interp_ticks);
    TEST_ASSERT_EQ_UINT(0U, block.feed_override_percent);
    return 0;
}

static int test_prepare_dwell_boundary_and_begin_update_rt(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;

    Test_ResetSystem();
    NcFeed_Reset();
    block = Test_MakeMotionBlock(30U, NC_MOTION_DWELL, 0U, 0, 0, 0, 0);

    TEST_ASSERT_EQ_INT(-1, NcFeed_PrepareDwellTs(NULL, 1U, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_RANGE, err);
    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(-1, NcFeed_PrepareDwellTs(&block, (uint32_t)(NC_DWELL_MAX_TICKS + 1U), &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_RANGE, err);

    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(0, NcFeed_PrepareDwellTs(&block, 0U, &err));
    TEST_ASSERT_EQ_UINT(1U, block.interp_ticks);
    TEST_ASSERT_EQ_UINT(NC_FEED_PROFILE_DWELL, block.feed_profile);

    TEST_ASSERT_EQ_INT(-1, NcFeed_BeginBlockRt(NULL));
    TEST_ASSERT_EQ_INT(0, NcFeed_BeginBlockRt(&block));
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_DWELL, g_ncFeedStatus.state);
    TEST_ASSERT_EQ_UINT(30U, g_ncFeedStatus.line_no);
    NcFeed_UpdateRt(&block, 1U, 1U);
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_COMPLETED, g_ncFeedStatus.state);
    TEST_ASSERT_EQ_UINT(1U, g_ncFeedStatus.completed_dwell_ticks);
    return 0;
}

static int test_begin_motion_with_zero_override_enters_hold_and_profile_ratio_edges(void)
{
    NC_EXEC_BLOCK block;
    double r0;
    double rMid;
    double rEnd;

    Test_ResetSystem();
    NcFeed_Reset();
    block = Test_MakeMotionBlock(40U, NC_MOTION_LINEAR, 0x1U, 100, 0, 0, 0);
    block.feed_override_percent = 0U;
    TEST_ASSERT_EQ_INT(-2, NcFeed_BeginBlockRt(&block));
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_HOLD, g_ncFeedStatus.state);

    block.feed_override_percent = 100U;
    block.interp_ticks = 10U;
    block.accel_ticks = 2U;
    block.decel_ticks = 2U;
    r0 = NcFeed_ProfileRatioRt(&block, 0U);
    rMid = NcFeed_ProfileRatioRt(&block, 5U);
    rEnd = NcFeed_ProfileRatioRt(&block, 10U);
    TEST_ASSERT_TRUE(r0 >= 0.0 && r0 <= 0.01);
    TEST_ASSERT_TRUE(rMid > 0.4 && rMid < 0.6);
    TEST_ASSERT_TRUE(rEnd == 1.0);

    TEST_ASSERT_TRUE(NcFeed_ProfileRatioRt(NULL, 1U) == 1.0);
    return 0;
}

int main(void)
{
    RUN_TEST(test_feed_reset_override_validation_and_generation);
    RUN_TEST(test_prepare_motion_rejects_null_bad_feed_negative_path_and_per_rev_without_spindle);
    RUN_TEST(test_prepare_motion_normal_and_override_zero_hold_path);
    RUN_TEST(test_prepare_dwell_boundary_and_begin_update_rt);
    RUN_TEST(test_begin_motion_with_zero_override_enters_hold_and_profile_ratio_edges);
    printf("[  PASSED  ] test_nc_feed\n");
    return 0;
}
