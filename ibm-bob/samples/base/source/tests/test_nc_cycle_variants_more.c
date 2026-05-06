#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_cycle.h"

static NC_EXEC_BLOCK make_cycle(uint32_t line, uint16_t code10)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.motion_type = NC_MOTION_CANNED_CYCLE;
    b.axis_mask = (1UL << 2U);
    b.axis_start[2] = 10000;
    b.axis_target[2] = -5000;
    b.r_value = 2000;
    b.feed = 1000;
    b.spindle_speed = 1200U;
    b.feature_flags = NC_FEATURE_FLAG_PARAM_R;
    b.cycle_code10 = code10;
    b.cycle_repeat = 1U;
    b.cycle_flags = 0U;
    return b;
}

static int test_apply_g74_sets_tap_and_left_tap_flags(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    NcCycle_ResetParserModal();
    memset(&b, 0, sizeof(b));
    NcCycle_ApplyCycleGCode(NC_G_CODE_WHOLE(74), &b);
    TEST_ASSERT_EQ_UINT((uint16_t)NC_G_CODE_WHOLE(74), b.cycle_code10);
    TEST_ASSERT_TRUE((b.cycle_flags & NC_CYCLE_FLAG_TAP) != 0U);
    TEST_ASSERT_TRUE((b.cycle_flags & NC_CYCLE_FLAG_LEFT_TAP) != 0U);
    return 0;
}

static int test_finalize_tapping_cycle_rejects_zero_spindle_speed(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcCycle_ResetParserModal();
    b = make_cycle(1941U, (uint16_t)NC_G_CODE_WHOLE(84));
    b.cycle_flags = NC_CYCLE_FLAG_TAP;
    b.spindle_speed = 0U;
    TEST_ASSERT_EQ_INT(-1, NcCycle_FinalizeParserBlock(&b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_FEED, err);
    return 0;
}

static int test_g83_peck_cycle_builds_multiple_peck_segments(void)
{
    NC_EXEC_BLOCK b;
    NC_ACTIVE_SEGMENT segs[NC_CANNED_MAX_SEGMENTS];
    uint32_t count = 0U;
    SystemShared_Initialize();
    NcCycle_ResetParserModal();
    memset(segs, 0, sizeof(segs));
    b = make_cycle(1942U, (uint16_t)NC_G_CODE_WHOLE(83));
    b.q_value = 2000;
    b.feature_flags |= NC_FEATURE_FLAG_PARAM_Q;
    TEST_ASSERT_EQ_INT(0, NcCycle_BuildSegmentPlanRt(&b, segs, &count));
    TEST_ASSERT_TRUE(count > 5U);
    TEST_ASSERT_TRUE(g_ncCycleStatus.peck_index > 1U);
    TEST_ASSERT_EQ_UINT(1U, g_ncCycleStatus.peck_blocks);
    return 0;
}

static int test_g82_dwell_cycle_includes_dwell_segment_kind(void)
{
    NC_EXEC_BLOCK b;
    NC_ACTIVE_SEGMENT segs[NC_CANNED_MAX_SEGMENTS];
    uint32_t count = 0U;
    uint32_t found = 0U;
    SystemShared_Initialize();
    NcCycle_ResetParserModal();
    memset(segs, 0, sizeof(segs));
    b = make_cycle(1943U, (uint16_t)NC_G_CODE_WHOLE(82));
    b.p_word = 7U;
    b.feature_flags |= NC_FEATURE_FLAG_PARAM_P;
    TEST_ASSERT_EQ_INT(0, NcCycle_BuildSegmentPlanRt(&b, segs, &count));
    for (uint32_t i = 0U; i < count; ++i) {
        if (segs[i].kind == NC_SEG_KIND_DWELL) {
            found = 1U;
        }
    }
    TEST_ASSERT_EQ_UINT(1U, found);
    TEST_ASSERT_TRUE(count >= 4U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_apply_g74_sets_tap_and_left_tap_flags);
    RUN_TEST(test_finalize_tapping_cycle_rejects_zero_spindle_speed);
    RUN_TEST(test_g83_peck_cycle_builds_multiple_peck_segments);
    RUN_TEST(test_g82_dwell_cycle_includes_dwell_segment_kind);
    printf("test_nc_cycle_variants_more: PASS\n");
    return 0;
}
