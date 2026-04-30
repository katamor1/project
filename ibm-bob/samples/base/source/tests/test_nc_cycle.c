#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_cycle.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcCycle_ResetParserModal();
}

static NC_EXEC_BLOCK make_cycle(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_CANNED_CYCLE;
    b.axis_mask = (1UL << 2U);
    b.axis_start[2] = 10000;
    b.axis_target[2] = -5000;
    b.r_value = 2000;
    b.feed = 1000;
    b.feature_flags = NC_FEATURE_FLAG_PARAM_R;
    b.cycle_code10 = (uint16_t)NC_G_CODE_WHOLE(81);
    b.cycle_repeat = 1U;
    return b;
}

static int test_repeat_word_boundaries(void)
{
    NC_EXEC_BLOCK b = make_cycle(91U);
    setup();
    TEST_ASSERT_EQ_INT(-2, NcCycle_SetRepeatWord(0.0, &b));
    TEST_ASSERT_EQ_INT(-2, NcCycle_SetRepeatWord((double)NC_CYCLE_REPEAT_MAX + 1.0, &b));
    TEST_ASSERT_EQ_INT(-2, NcCycle_SetRepeatWord(2.0, NULL));
    TEST_ASSERT_EQ_INT(0, NcCycle_SetRepeatWord(2.4, &b));
    TEST_ASSERT_EQ_UINT(2U, b.cycle_repeat);
    TEST_ASSERT_TRUE((b.cycle_flags & NC_CYCLE_FLAG_HAS_K) != 0U);
    return 0;
}

static int test_finalize_rejects_missing_feed_or_r_plane(void)
{
    NC_EXEC_BLOCK b = make_cycle(92U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    b.feed = 0;
    TEST_ASSERT_EQ_INT(-1, NcCycle_FinalizeParserBlock(&b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_FEED, err);

    b = make_cycle(92U);
    b.feature_flags = 0U;
    err = NC_ERR_NONE;
    TEST_ASSERT_EQ_INT(-1, NcCycle_FinalizeParserBlock(&b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_RANGE, err);
    return 0;
}

static int test_finalize_valid_cycle_and_modal_replay(void)
{
    NC_EXEC_BLOCK b = make_cycle(93U);
    NC_EXEC_BLOCK replay;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcCycle_FinalizeParserBlock(&b, &err));
    memset(&replay, 0, sizeof(replay));
    replay.motion_type = NC_MOTION_CANNED_CYCLE;
    NcCycle_InitParserBlock(&replay);
    TEST_ASSERT_EQ_UINT((uint16_t)NC_G_CODE_WHOLE(81), replay.cycle_code10);
    TEST_ASSERT_TRUE((replay.cycle_flags & NC_CYCLE_FLAG_MODAL_REPLAY) != 0U);
    return 0;
}

static int test_build_segment_plan_for_g81(void)
{
    NC_EXEC_BLOCK b = make_cycle(94U);
    NC_ACTIVE_SEGMENT segs[NC_CANNED_MAX_SEGMENTS];
    uint32_t count = 0U;
    setup();
    memset(segs, 0, sizeof(segs));
    TEST_ASSERT_EQ_INT(-1, NcCycle_BuildSegmentPlanRt(NULL, segs, &count));
    TEST_ASSERT_EQ_INT(0, NcCycle_BuildSegmentPlanRt(&b, segs, &count));
    TEST_ASSERT_TRUE(count >= 3U);
    TEST_ASSERT_EQ_UINT((uint16_t)NC_G_CODE_WHOLE(81), g_ncCycleStatus.last_cycle_code10);
    TEST_ASSERT_TRUE(segs[0].end_tick > 0U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_repeat_word_boundaries);
    RUN_TEST(test_finalize_rejects_missing_feed_or_r_plane);
    RUN_TEST(test_finalize_valid_cycle_and_modal_replay);
    RUN_TEST(test_build_segment_plan_for_g81);
    printf("test_nc_cycle: PASS\n");
    return 0;
}
