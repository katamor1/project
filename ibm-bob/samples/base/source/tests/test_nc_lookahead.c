#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_lookahead.h"

static NC_EXEC_BLOCK make_line(uint32_t lineNo, int32_t sx, int32_t sy, int32_t ex, int32_t ey)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x03U;
    b.axis_start[0] = sx;
    b.axis_start[1] = sy;
    b.axis_target[0] = ex;
    b.axis_target[1] = ey;
    b.interp_ticks = 10U;
    b.feed_override_percent = 100U;
    return b;
}

static void setup(void)
{
    SystemShared_Initialize();
    NcLookahead_Reset();
}

static int test_g08_command_enables_smoothing_for_following_block(void)
{
    NC_EXEC_BLOCK cmd;
    NC_EXEC_BLOCK motion;
    setup();
    memset(&cmd, 0, sizeof(cmd));
    cmd.line_no = 21U;
    cmd.g_code10 = (uint16_t)NC_G_CODE_WHOLE(8);
    cmd.p_word = 2U;
    NcLookahead_ApplyBlockTs(&cmd);
    TEST_ASSERT_EQ_UINT(21U, g_ncPathControlStatus.last_line_no);

    motion = make_line(22U, 0, 0, 1000, 0);
    NcLookahead_ApplyBlockTs(&motion);
    TEST_ASSERT_TRUE((motion.feature_flags & NC_FEATURE_FLAG_SMOOTHING) != 0U);
    TEST_ASSERT_TRUE((motion.feature_flags & NC_FEATURE_FLAG_HIGH_PRECISION) != 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ncPathControlStatus.lookahead_blocks);
    return 0;
}

static int test_corner_slowdown_on_sharp_corner(void)
{
    NC_EXEC_BLOCK first;
    NC_EXEC_BLOCK second;
    setup();
    first = make_line(23U, 0, 0, 10000, 0);
    second = make_line(24U, 10000, 0, 10000, 10000);
    NcLookahead_ApplyBlockTs(&first);
    NcLookahead_ApplyBlockTs(&second);
    TEST_ASSERT_TRUE(g_ncPathControlStatus.last_corner_angle_deg >= NC_LOOKAHEAD_CORNER_WARN_DEG);
    TEST_ASSERT_TRUE(g_ncPathControlStatus.corner_slowdown_events >= 1U);
    TEST_ASSERT_TRUE(second.feed_override_percent <= NC_LOOKAHEAD_CORNER_SLOW_PERCENT);
    TEST_ASSERT_TRUE(second.interp_ticks >= 10U);
    return 0;
}

static int test_hpcc_command_caps_feed_override(void)
{
    NC_EXEC_BLOCK cmd;
    NC_EXEC_BLOCK motion;
    setup();
    memset(&cmd, 0, sizeof(cmd));
    cmd.line_no = 25U;
    cmd.g_code10 = (uint16_t)NC_G_CODE_WHOLE(5);
    cmd.p_word = 10000U;
    NcLookahead_ApplyBlockTs(&cmd);
    motion = make_line(26U, 0, 0, 1000, 0);
    motion.feed_override_percent = 150U;
    NcLookahead_ApplyBlockTs(&motion);
    TEST_ASSERT_TRUE((motion.feature_flags & NC_FEATURE_FLAG_HPCC) != 0U);
    TEST_ASSERT_TRUE(motion.feed_override_percent <= NC_HPCC_FEED_OVERRIDE_PERCENT);
    return 0;
}

static int test_null_and_non_motion_are_noop(void)
{
    NC_EXEC_BLOCK b;
    setup();
    NcLookahead_ApplyBlockTs(NULL);
    TEST_ASSERT_EQ_UINT(0U, g_ncPathControlStatus.generation);
    memset(&b, 0, sizeof(b));
    b.line_no = 27U;
    b.motion_type = NC_MOTION_NONE;
    NcLookahead_ApplyBlockTs(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncPathControlStatus.lookahead_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_g08_command_enables_smoothing_for_following_block);
    RUN_TEST(test_corner_slowdown_on_sharp_corner);
    RUN_TEST(test_hpcc_command_caps_feed_override);
    RUN_TEST(test_null_and_non_motion_are_noop);
    printf("test_nc_lookahead: PASS\n");
    return 0;
}
