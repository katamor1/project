#include "test_common.h"
#include "system_shared.h"
#include "nc_feed.h"

static NC_EXEC_BLOCK make_block(uint32_t line, NC_MOTION_TYPE motion)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.motion_type = motion;
    b.axis_mask = 0x01U;
    b.axis_target[0] = 10000;
    b.feed = 600000;
    b.feed_override_percent = 100U;
    b.interp_ticks = 10U;
    b.feed_profile = NC_FEED_PROFILE_TRAPEZOID;
    return b;
}

static int test_prepare_rapid_motion_ignores_zero_program_feed(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcFeed_Reset();
    b = make_block(1951U, NC_MOTION_RAPID);
    b.feed = 0;
    TEST_ASSERT_EQ_INT(0, NcFeed_PrepareMotionTs(&b, 2.0, &err));
    TEST_ASSERT_TRUE(b.interp_ticks >= NC_INTERP_MIN_TICKS);
    TEST_ASSERT_EQ_UINT(NC_FEED_PROFILE_TRAPEZOID, b.feed_profile);
    return 0;
}

static int test_prepare_per_rev_motion_uses_spindle_speed_to_compute_ticks(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcFeed_Reset();
    b = make_block(1952U, NC_MOTION_LINEAR);
    b.feed_mode = NC_FEED_MODE_PER_REV;
    b.feed = 10000; /* 1 unit/rev */
    b.spindle_speed = 60U;
    TEST_ASSERT_EQ_INT(0, NcFeed_PrepareMotionTs(&b, 1.0, &err));
    TEST_ASSERT_TRUE(b.interp_ticks > 0U);
    TEST_ASSERT_EQ_UINT(100U, b.feed_override_percent);
    return 0;
}

static int test_prepare_dwell_preserves_current_override_percent(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcFeed_Reset();
    b = make_block(1953U, NC_MOTION_DWELL);
    TEST_ASSERT_EQ_INT(0, NcFeed_SetOverride(150U));
    TEST_ASSERT_EQ_INT(0, NcFeed_PrepareDwellTs(&b, 5U, &err));
    TEST_ASSERT_EQ_UINT(5U, b.interp_ticks);
    TEST_ASSERT_EQ_UINT(150U, b.feed_override_percent);
    TEST_ASSERT_EQ_UINT(NC_FEED_PROFILE_DWELL, b.feed_profile);
    return 0;
}

static int test_update_without_done_advances_tick_but_keeps_active_state(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    NcFeed_Reset();
    b = make_block(1954U, NC_MOTION_LINEAR);
    TEST_ASSERT_EQ_INT(0, NcFeed_BeginBlockRt(&b));
    NcFeed_UpdateRt(&b, 4U, 0U);
    TEST_ASSERT_EQ_UINT(4U, g_ncFeedStatus.active_tick);
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_ACTIVE, g_ncFeedStatus.state);
    NcFeed_CancelRt();
    TEST_ASSERT_EQ_UINT(NC_FEED_CTRL_IDLE, g_ncFeedStatus.state);
    return 0;
}

int main(void)
{
    RUN_TEST(test_prepare_rapid_motion_ignores_zero_program_feed);
    RUN_TEST(test_prepare_per_rev_motion_uses_spindle_speed_to_compute_ticks);
    RUN_TEST(test_prepare_dwell_preserves_current_override_percent);
    RUN_TEST(test_update_without_done_advances_tick_but_keeps_active_state);
    printf("test_nc_feed_profiles_more: PASS\n");
    return 0;
}
