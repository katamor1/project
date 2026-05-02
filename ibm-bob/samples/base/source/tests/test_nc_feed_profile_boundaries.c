#include "test_common.h"
#include "system_shared.h"
#include "nc_feed.h"

static NC_EXEC_BLOCK make_profile_block(uint32_t ticks, uint32_t accel, uint32_t decel)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = 501U;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.feed = 1000;
    b.interp_ticks = ticks;
    b.accel_ticks = accel;
    b.decel_ticks = decel;
    b.feed_override_percent = 100U;
    b.feed_profile = NC_FEED_PROFILE_TRAPEZOID;
    return b;
}

static int test_profile_ratio_zero_ticks_and_no_accel_linear_ratio(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    NcFeed_Reset();

    b = make_profile_block(0U, 0U, 0U);
    TEST_ASSERT_TRUE(NcFeed_ProfileRatioRt(&b, 0U) == 1.0);

    b = make_profile_block(8U, 0U, 0U);
    TEST_ASSERT_TRUE(NcFeed_ProfileRatioRt(&b, 0U) == 0.0);
    TEST_ASSERT_TRUE(NcFeed_ProfileRatioRt(&b, 4U) == 0.5);
    TEST_ASSERT_TRUE(NcFeed_ProfileRatioRt(&b, 8U) == 1.0);
    return 0;
}

static int test_profile_ratio_trapezoid_boundaries_are_monotonic(void)
{
    NC_EXEC_BLOCK b;
    double r1;
    double r2;
    double r3;
    SystemShared_Initialize();
    NcFeed_Reset();

    b = make_profile_block(10U, 2U, 2U);
    r1 = NcFeed_ProfileRatioRt(&b, 1U);
    r2 = NcFeed_ProfileRatioRt(&b, 5U);
    r3 = NcFeed_ProfileRatioRt(&b, 9U);

    TEST_ASSERT_TRUE(r1 > 0.0 && r1 < r2);
    TEST_ASSERT_TRUE(r2 > 0.49 && r2 < 0.51);
    TEST_ASSERT_TRUE(r3 > r2 && r3 < 1.0);
    TEST_ASSERT_TRUE(NcFeed_ProfileRatioRt(&b, 10U) == 1.0);
    TEST_ASSERT_TRUE(NcFeed_ProfileRatioRt(NULL, 10U) == 1.0);
    return 0;
}

static int test_cancel_after_active_block_restores_idle_and_generation(void)
{
    NC_EXEC_BLOCK b;
    uint32_t genBefore;
    SystemShared_Initialize();
    NcFeed_Reset();

    b = make_profile_block(12U, 3U, 3U);
    TEST_ASSERT_EQ_INT(0, NcFeed_BeginBlockRt(&b));
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_ACTIVE, g_ncFeedStatus.state);
    genBefore = g_ncFeedStatus.generation;
    NcFeed_CancelRt();
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_IDLE, g_ncFeedStatus.state);
    TEST_ASSERT_TRUE(g_ncFeedStatus.generation > genBefore);
    return 0;
}

int main(void)
{
    RUN_TEST(test_profile_ratio_zero_ticks_and_no_accel_linear_ratio);
    RUN_TEST(test_profile_ratio_trapezoid_boundaries_are_monotonic);
    RUN_TEST(test_cancel_after_active_block_restores_idle_and_generation);
    printf("test_nc_feed_profile_boundaries: PASS\n");
    return 0;
}
