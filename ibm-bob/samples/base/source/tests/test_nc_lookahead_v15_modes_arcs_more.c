#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_lookahead.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcLookahead_Reset();
}

static NC_EXEC_BLOCK line(uint32_t no, int32_t sx, int32_t sy, int32_t ex, int32_t ey)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = no;
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

static int test_invalid_g08_p_word_does_not_enable_smoothing(void)
{
    NC_EXEC_BLOCK cmd;
    NC_EXEC_BLOCK b;
    setup();
    memset(&cmd, 0, sizeof(cmd));
    cmd.g_code10 = (uint16_t)NC_G_CODE_WHOLE(8);
    cmd.p_word = 9U;
    NcLookahead_ApplyBlockTs(&cmd);
    b = line(11U, 0, 0, 1000, 0);
    NcLookahead_ApplyBlockTs(&b);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_SMOOTHING) == 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ncPathControlStatus.lookahead_blocks);
    return 0;
}

static int test_hpcc_cancel_p0_removes_feed_cap_for_next_motion(void)
{
    NC_EXEC_BLOCK cmd;
    NC_EXEC_BLOCK b;
    setup();
    memset(&cmd, 0, sizeof(cmd));
    cmd.g_code10 = (uint16_t)NC_G_CODE_WHOLE(5);
    cmd.p_word = 10000U;
    NcLookahead_ApplyBlockTs(&cmd);
    cmd.p_word = 0U;
    NcLookahead_ApplyBlockTs(&cmd);
    b = line(12U, 0, 0, 1000, 0);
    b.feed_override_percent = 150U;
    NcLookahead_ApplyBlockTs(&b);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_HPCC) == 0U);
    TEST_ASSERT_EQ_UINT(150U, b.feed_override_percent);
    return 0;
}

static int test_exact_stop_block_prevents_g08_smoothing_flags(void)
{
    NC_EXEC_BLOCK cmd;
    NC_EXEC_BLOCK b;
    setup();
    memset(&cmd, 0, sizeof(cmd));
    cmd.g_code10 = (uint16_t)NC_G_CODE_WHOLE(8);
    cmd.p_word = 2U;
    NcLookahead_ApplyBlockTs(&cmd);
    b = line(13U, 0, 0, 1000, 0);
    b.feature_flags = NC_FEATURE_FLAG_EXACT_STOP;
    NcLookahead_ApplyBlockTs(&b);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_SMOOTHING) == 0U);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_HIGH_PRECISION) == 0U);
    return 0;
}

static int test_arc_with_out_of_plane_delta_counts_helical_block(void)
{
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    b.line_no = 14U;
    b.motion_type = NC_MOTION_ARC_CCW;
    b.plane = NC_PLANE_XY;
    b.axis_mask = 0x07U;
    b.axis_target[0] = 1000;
    b.axis_target[1] = 1000;
    b.axis_target[2] = 500;
    b.interp_ticks = 20U;
    b.feed_override_percent = 100U;
    NcLookahead_ApplyBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncPathControlStatus.arc_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncPathControlStatus.helical_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_invalid_g08_p_word_does_not_enable_smoothing);
    RUN_TEST(test_hpcc_cancel_p0_removes_feed_cap_for_next_motion);
    RUN_TEST(test_exact_stop_block_prevents_g08_smoothing_flags);
    RUN_TEST(test_arc_with_out_of_plane_delta_counts_helical_block);
    printf("test_nc_lookahead_v15_modes_arcs_more: PASS\n");
    return 0;
}
