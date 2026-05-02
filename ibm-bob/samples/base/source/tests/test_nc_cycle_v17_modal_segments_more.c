#include "test_common.h"
#include "system_shared.h"
#include "nc_cycle.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_cycle(uint16_t code10)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = 1741U;
    b.motion_type = NC_MOTION_CANNED_CYCLE;
    b.cycle_code10 = code10;
    b.axis_start[2] = 1000;
    b.axis_target[0] = 10;
    b.axis_target[1] = 20;
    b.axis_target[2] = -2000;
    b.axis_mask = 0x07U;
    b.r_value = 500;
    b.q_value = 700;
    b.p_word = 3U;
    b.feed = 100;
    b.spindle_speed = 500U;
    b.feature_flags = NC_FEATURE_FLAG_CANNED_CYCLE | NC_FEATURE_FLAG_PARAM_R;
    b.cycle_repeat = 1U;
    return b;
}

static int test_apply_cycle_gcode_sets_tap_and_repeat_defaults(void)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    NcCycle_ApplyCycleGCode(NC_G_CODE_WHOLE(84), &b);
    TEST_ASSERT_EQ_UINT(NC_G_CODE_WHOLE(84), b.cycle_code10);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_CANNED_CYCLE) != 0U);
    TEST_ASSERT_TRUE((b.cycle_flags & NC_CYCLE_FLAG_TAP) != 0U);
    TEST_ASSERT_EQ_UINT(1U, b.cycle_repeat);
    return 0;
}

static int test_set_repeat_word_rounds_and_rejects_invalid_values(void)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    TEST_ASSERT_EQ_INT(0, NcCycle_SetRepeatWord(2.51, &b));
    TEST_ASSERT_EQ_UINT(3U, b.cycle_repeat);
    TEST_ASSERT_TRUE((b.cycle_flags & NC_CYCLE_FLAG_HAS_K) != 0U);
    TEST_ASSERT_EQ_INT(-2, NcCycle_SetRepeatWord(0.0, &b));
    TEST_ASSERT_EQ_INT(-2, NcCycle_SetRepeatWord((double)NC_CYCLE_REPEAT_MAX + 1.0, &b));
    TEST_ASSERT_EQ_INT(-2, NcCycle_SetRepeatWord(1.0, NULL));
    return 0;
}

static int test_finalize_rejects_peck_cycle_without_positive_q(void)
{
    NC_EXEC_BLOCK b = make_cycle((uint16_t)NC_G_CODE_WHOLE(83));
    NC_ERROR_CODE err = NC_ERR_NONE;
    b.feature_flags &= ~NC_FEATURE_FLAG_PARAM_Q;
    b.q_value = 0;
    TEST_ASSERT_EQ_INT(-1, NcCycle_FinalizeParserBlock(&b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_RANGE, err);
    return 0;
}

static int test_build_segment_plan_repeats_and_tracks_status(void)
{
    NC_EXEC_BLOCK b = make_cycle((uint16_t)NC_G_CODE_WHOLE(82));
    NC_ACTIVE_SEGMENT segs[NC_CANNED_MAX_SEGMENTS];
    uint32_t count = 0U;
    SystemShared_Initialize();
    b.cycle_repeat = 2U;
    b.feature_flags |= NC_FEATURE_FLAG_PARAM_P;
    TEST_ASSERT_EQ_INT(0, NcCycle_BuildSegmentPlanRt(&b, segs, &count));
    TEST_ASSERT_TRUE(count >= 8U);
    TEST_ASSERT_EQ_UINT(NC_SEG_KIND_RAPID, segs[0].kind);
    TEST_ASSERT_EQ_UINT(NC_SEG_KIND_DWELL, segs[2].kind);
    TEST_ASSERT_EQ_UINT(NC_G_CODE_WHOLE(82), g_ncCycleStatus.last_cycle_code10);
    TEST_ASSERT_EQ_UINT(2U, g_ncCycleStatus.cycle_repeat);
    TEST_ASSERT_EQ_INT(-1, NcCycle_BuildSegmentPlanRt(NULL, segs, &count));
    return 0;
}

int main(void)
{
    RUN_TEST(test_apply_cycle_gcode_sets_tap_and_repeat_defaults);
    RUN_TEST(test_set_repeat_word_rounds_and_rejects_invalid_values);
    RUN_TEST(test_finalize_rejects_peck_cycle_without_positive_q);
    RUN_TEST(test_build_segment_plan_repeats_and_tracks_status);
    printf("test_nc_cycle_v17_modal_segments_more: PASS\n");
    return 0;
}
