#include "test_common.h"
#include "system_shared.h"
#include "nc_interference.h"

static NC_EXEC_BLOCK make_block(uint32_t lineNo, NC_MOTION_TYPE motion)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = motion;
    b.axis_mask = 0x03U;
    b.feed_override_percent = 100U;
    return b;
}

static int test_disabled_and_non_motion_blocks_are_noop(void)
{
    NC_EXEC_BLOCK b = make_block(701U, NC_MOTION_AUX_ONLY);
    SystemShared_Initialize();
    NcInterference_Reset();

    NcInterference_CheckBlockTs(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncInterferenceStatus.checked_blocks);

    b.motion_type = NC_MOTION_LINEAR;
    TEST_ASSERT_EQ_INT(0, NcInterference_SetEnabled(0U));
    NcInterference_CheckBlockTs(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncInterferenceStatus.checked_blocks);
    TEST_ASSERT_EQ_UINT(0U, g_ncEventRing.captured_count);
    return 0;
}

static int test_axis_limit_warning_slows_block_and_records_event(void)
{
    NC_EXEC_BLOCK b = make_block(702U, NC_MOTION_LINEAR);
    SystemShared_Initialize();
    NcInterference_Reset();

    b.axis_target[0] = NC_INTERFERENCE_AXIS_LIMIT + 1;
    NcInterference_CheckBlockTs(&b);

    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.checked_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.axis_limit_warnings);
    TEST_ASSERT_EQ_UINT(3U, g_ncInterferenceStatus.last_warning_kind);
    TEST_ASSERT_EQ_UINT(702U, g_ncInterferenceStatus.last_warning_line_no);
    TEST_ASSERT_EQ_UINT(NC_INTERFERENCE_SLOWDOWN_PERCENT, b.feed_override_percent);
    TEST_ASSERT_EQ_UINT(1U, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_INTERFERENCE_WARN, g_ncEventRing.items[0].code);
    return 0;
}

static int test_arc_small_radius_path_warning_without_axis_limit(void)
{
    NC_EXEC_BLOCK b = make_block(703U, NC_MOTION_ARC_CW);
    SystemShared_Initialize();
    NcInterference_Reset();

    b.axis_target[0] = 1000;
    b.axis_target[1] = 1000;
    b.arc_radius = NC_INTERFERENCE_MIN_CLEARANCE - 1;
    NcInterference_CheckBlockTs(&b);

    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.checked_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.path_interference_warnings);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.last_warning_kind);
    TEST_ASSERT_EQ_UINT(NC_INTERFERENCE_SLOWDOWN_PERCENT, b.feed_override_percent);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_INTERFERENCE_WARN, g_ncEventRing.items[0].code);
    return 0;
}

int main(void)
{
    RUN_TEST(test_disabled_and_non_motion_blocks_are_noop);
    RUN_TEST(test_axis_limit_warning_slows_block_and_records_event);
    RUN_TEST(test_arc_small_radius_path_warning_without_axis_limit);
    printf("test_nc_interference_warning_edges: PASS\n");
    return 0;
}
