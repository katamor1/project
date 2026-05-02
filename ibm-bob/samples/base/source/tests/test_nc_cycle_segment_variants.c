#include "test_common.h"
#include "system_shared.h"
#include "nc_cycle.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_base_cycle(uint32_t lineNo, uint16_t code10)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_CANNED_CYCLE;
    b.axis_mask = (1UL << 2U);
    b.axis_start[2] = 5000;
    b.axis_target[2] = -3000;
    b.r_value = 1000;
    b.q_value = 1000;
    b.p_word = 4U;
    b.feed = 1000;
    b.spindle_speed = NC_TAP_MIN_SPINDLE_RPM;
    b.feature_flags = NC_FEATURE_FLAG_CANNED_CYCLE |
        NC_FEATURE_FLAG_PARAM_R | NC_FEATURE_FLAG_PARAM_Q | NC_FEATURE_FLAG_PARAM_P;
    b.cycle_code10 = code10;
    b.cycle_repeat = 1U;
    return b;
}

static int test_g82_generates_dwell_segment_between_feed_and_retract(void)
{
    NC_EXEC_BLOCK b = make_base_cycle(601U, (uint16_t)NC_G_CODE_WHOLE(82));
    NC_ACTIVE_SEGMENT segs[NC_CANNED_MAX_SEGMENTS];
    uint32_t count = 0U;
    SystemShared_Initialize();
    NcCycle_ResetParserModal();
    memset(segs, 0, sizeof(segs));

    TEST_ASSERT_EQ_INT(0, NcCycle_BuildSegmentPlanRt(&b, segs, &count));
    TEST_ASSERT_EQ_UINT(4U, count);
    TEST_ASSERT_EQ_UINT(NC_SEG_KIND_RAPID, segs[0].kind);
    TEST_ASSERT_EQ_UINT(NC_SEG_KIND_LINEAR, segs[1].kind);
    TEST_ASSERT_EQ_UINT(NC_SEG_KIND_DWELL, segs[2].kind);
    TEST_ASSERT_EQ_UINT(NC_SEG_KIND_RETRACT, segs[3].kind);
    TEST_ASSERT_EQ_UINT(4U, segs[2].end_tick - segs[1].end_tick);
    return 0;
}

static int test_g83_peck_cycle_generates_multiple_peck_and_retract_segments(void)
{
    NC_EXEC_BLOCK b = make_base_cycle(602U, (uint16_t)NC_G_CODE_WHOLE(83));
    NC_ACTIVE_SEGMENT segs[NC_CANNED_MAX_SEGMENTS];
    uint32_t count = 0U;
    SystemShared_Initialize();
    NcCycle_ResetParserModal();
    memset(segs, 0, sizeof(segs));

    TEST_ASSERT_EQ_INT(0, NcCycle_BuildSegmentPlanRt(&b, segs, &count));
    TEST_ASSERT_TRUE(count >= 10U);
    TEST_ASSERT_EQ_UINT(4U, g_ncCycleStatus.peck_index);
    TEST_ASSERT_EQ_UINT(1U, g_ncCycleStatus.peck_blocks);
    TEST_ASSERT_EQ_UINT(NC_SEG_KIND_RAPID, segs[0].kind);
    TEST_ASSERT_EQ_UINT(NC_SEG_KIND_RETRACT, segs[count - 1U].kind);
    return 0;
}

static int test_g88_manual_return_flag_updates_status(void)
{
    NC_EXEC_BLOCK b = make_base_cycle(603U, (uint16_t)NC_G_CODE_WHOLE(88));
    NC_ACTIVE_SEGMENT segs[NC_CANNED_MAX_SEGMENTS];
    uint32_t count = 0U;
    SystemShared_Initialize();
    NcCycle_ResetParserModal();
    NcCycle_ApplyCycleGCode((int32_t)NC_G_CODE_WHOLE(88), &b);
    memset(segs, 0, sizeof(segs));

    TEST_ASSERT_TRUE((b.cycle_flags & NC_CYCLE_FLAG_MANUAL_RETURN) != 0U);
    TEST_ASSERT_EQ_INT(0, NcCycle_BuildSegmentPlanRt(&b, segs, &count));
    TEST_ASSERT_TRUE(count >= 3U);
    TEST_ASSERT_EQ_UINT(1U, g_ncCycleStatus.boring_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncCycleStatus.manual_return_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_g82_generates_dwell_segment_between_feed_and_retract);
    RUN_TEST(test_g83_peck_cycle_generates_multiple_peck_and_retract_segments);
    RUN_TEST(test_g88_manual_return_flag_updates_status);
    printf("test_nc_cycle_segment_variants: PASS\n");
    return 0;
}
