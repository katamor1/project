#include "test_common.h"
#include "system_shared.h"
#include "nc_feed.h"

static NC_EXEC_BLOCK make_feed_block(void)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = 610U;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.feed = 1000 * NC_POSITION_SCALE;
    b.feed_mode = NC_FEED_MODE_PER_MIN;
    return b;
}

static int test_per_rev_feed_requires_spindle_speed(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcFeed_Reset();

    b = make_feed_block();
    b.feed_mode = NC_FEED_MODE_PER_REV;
    b.spindle_speed = 0U;
    TEST_ASSERT_EQ_INT(-1, NcFeed_PrepareMotionTs(&b, 10.0, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_FEED, err);

    err = NC_ERR_NONE;
    b.spindle_speed = 100U;
    TEST_ASSERT_EQ_INT(0, NcFeed_PrepareMotionTs(&b, 10.0, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_TRUE(b.interp_ticks >= NC_INTERP_MIN_TICKS);
    TEST_ASSERT_EQ_UINT(NC_FEED_PROFILE_TRAPEZOID, b.feed_profile);
    return 0;
}

static int test_prepare_motion_rejects_negative_path_and_nulls(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcFeed_Reset();

    b = make_feed_block();
    TEST_ASSERT_EQ_INT(-1, NcFeed_PrepareMotionTs(NULL, 1.0, &err));
    TEST_ASSERT_EQ_INT(-1, NcFeed_PrepareMotionTs(&b, 1.0, NULL));
    TEST_ASSERT_EQ_INT(-1, NcFeed_PrepareMotionTs(&b, -0.1, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_FEED, err);
    return 0;
}

static int test_dwell_range_and_zero_tick_normalization(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcFeed_Reset();

    memset(&b, 0, sizeof(b));
    TEST_ASSERT_EQ_INT(0, NcFeed_PrepareDwellTs(&b, 0U, &err));
    TEST_ASSERT_EQ_UINT(1U, b.interp_ticks);
    TEST_ASSERT_EQ_UINT(NC_FEED_PROFILE_DWELL, b.feed_profile);

    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(-1, NcFeed_PrepareDwellTs(&b, NC_DWELL_MAX_TICKS + 1U, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_RANGE, err);
    return 0;
}

static int test_zero_override_holds_non_dwell_but_not_dwell(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    NcFeed_Reset();

    TEST_ASSERT_EQ_INT(0, NcFeed_SetOverride(0U));
    b = make_feed_block();
    b.feed_override_percent = 0U;
    b.interp_ticks = 5U;
    b.feed_profile = NC_FEED_PROFILE_TRAPEZOID;
    TEST_ASSERT_EQ_INT(-2, NcFeed_BeginBlockRt(&b));
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_HOLD, g_ncFeedStatus.state);

    b.motion_type = NC_MOTION_DWELL;
    b.feed_profile = NC_FEED_PROFILE_DWELL;
    TEST_ASSERT_EQ_INT(0, NcFeed_BeginBlockRt(&b));
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_DWELL, g_ncFeedStatus.state);
    return 0;
}

int main(void)
{
    RUN_TEST(test_per_rev_feed_requires_spindle_speed);
    RUN_TEST(test_prepare_motion_rejects_negative_path_and_nulls);
    RUN_TEST(test_dwell_range_and_zero_tick_normalization);
    RUN_TEST(test_zero_override_holds_non_dwell_but_not_dwell);
    printf("test_nc_feed_per_rev_and_error_edges: PASS\n");
    return 0;
}
