#include <string.h>
#include "test_common.h"
#include "nc_feed.h"

static NC_EXEC_BLOCK make_block(NC_MOTION_TYPE motion, uint32_t ticks)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = 10U;
    b.motion_type = motion;
    b.feed = NC_POSITION_SCALE;
    b.interp_ticks = ticks;
    b.accel_ticks = 2U;
    b.decel_ticks = 2U;
    b.feed_override_percent = NC_FEED_OVERRIDE_DEFAULT_PERCENT;
    b.feed_profile = (motion == NC_MOTION_DWELL) ? NC_FEED_PROFILE_DWELL : NC_FEED_PROFILE_TRAPEZOID;
    return b;
}

static int test_cancel_after_begin_restores_idle_and_bumps_generation(void)
{
    NC_EXEC_BLOCK b = make_block(NC_MOTION_LINEAR, 20U);
    uint32_t before;
    SystemShared_Initialize();
    NcFeed_Reset();
    TEST_ASSERT_EQ_INT(0, NcFeed_BeginBlockRt(&b));
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_ACTIVE, g_ncFeedStatus.state);
    before = g_ncFeedStatus.generation;
    NcFeed_CancelRt();
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_IDLE, g_ncFeedStatus.state);
    TEST_ASSERT_TRUE(g_ncFeedStatus.generation > before);
    return 0;
}

static int test_update_null_preserves_active_tick(void)
{
    NC_EXEC_BLOCK b = make_block(NC_MOTION_LINEAR, 20U);
    SystemShared_Initialize();
    NcFeed_Reset();
    TEST_ASSERT_EQ_INT(0, NcFeed_BeginBlockRt(&b));
    NcFeed_UpdateRt(&b, 7U, 0U);
    TEST_ASSERT_EQ_UINT(7U, g_ncFeedStatus.active_tick);
    NcFeed_UpdateRt(NULL, 99U, 1U);
    TEST_ASSERT_EQ_UINT(7U, g_ncFeedStatus.active_tick);
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_ACTIVE, g_ncFeedStatus.state);
    return 0;
}

static int test_dwell_done_accumulates_completed_ticks(void)
{
    NC_EXEC_BLOCK b = make_block(NC_MOTION_DWELL, 5U);
    SystemShared_Initialize();
    NcFeed_Reset();
    TEST_ASSERT_EQ_INT(0, NcFeed_BeginBlockRt(&b));
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_DWELL, g_ncFeedStatus.state);
    NcFeed_UpdateRt(&b, 5U, 1U);
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_COMPLETED, g_ncFeedStatus.state);
    TEST_ASSERT_EQ_UINT(5U, g_ncFeedStatus.completed_dwell_ticks);
    return 0;
}

static int test_profile_ratio_zero_and_midpoint_paths(void)
{
    NC_EXEC_BLOCK b = make_block(NC_MOTION_LINEAR, 0U);
    double r;
    SystemShared_Initialize();
    NcFeed_Reset();
    r = NcFeed_ProfileRatioRt(&b, 0U);
    TEST_ASSERT_TRUE(r > 0.99 && r < 1.01);
    b.interp_ticks = 10U;
    b.accel_ticks = 0U;
    b.decel_ticks = 0U;
    r = NcFeed_ProfileRatioRt(&b, 5U);
    TEST_ASSERT_TRUE(r > 0.49 && r < 0.51);
    return 0;
}

int main(void)
{
    RUN_TEST(test_cancel_after_begin_restores_idle_and_bumps_generation);
    RUN_TEST(test_update_null_preserves_active_tick);
    RUN_TEST(test_dwell_done_accumulates_completed_ticks);
    RUN_TEST(test_profile_ratio_zero_and_midpoint_paths);
    printf("test_nc_feed_v14_rt_update_more: PASS\n");
    return 0;
}
