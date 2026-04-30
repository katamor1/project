#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_interference.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcInterference_Reset();
}

static NC_EXEC_BLOCK make_linear(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x07U;
    b.feed_override_percent = 100U;
    return b;
}

static int test_axis_limit_warning_slows_block_and_pushes_event(void)
{
    NC_EXEC_BLOCK b = make_linear(21U);
    setup();
    b.axis_target[0] = NC_INTERFERENCE_AXIS_LIMIT + 100;

    NcInterference_CheckBlockTs(&b);

    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.checked_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.axis_limit_warnings);
    TEST_ASSERT_EQ_UINT(NC_INTERFERENCE_SLOWDOWN_PERCENT, b.feed_override_percent);
    TEST_ASSERT_EQ_UINT(1U, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_INTERFERENCE_WARN, g_ncEventRing.items[0].code);
    TEST_ASSERT_EQ_UINT(21U, g_ncInterferenceStatus.last_warning_line_no);
    return 0;
}

static int test_disabled_or_non_motion_block_is_noop(void)
{
    NC_EXEC_BLOCK b = make_linear(22U);
    setup();
    TEST_ASSERT_EQ_INT(0, NcInterference_SetEnabled(0U));
    b.axis_target[0] = NC_INTERFERENCE_AXIS_LIMIT + 100;
    NcInterference_CheckBlockTs(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncInterferenceStatus.checked_blocks);
    TEST_ASSERT_EQ_UINT(100U, b.feed_override_percent);

    TEST_ASSERT_EQ_INT(0, NcInterference_SetEnabled(1U));
    b.motion_type = NC_MOTION_DWELL;
    NcInterference_CheckBlockTs(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncInterferenceStatus.checked_blocks);
    return 0;
}

static int test_arc_with_small_radius_records_path_warning(void)
{
    NC_EXEC_BLOCK b = make_linear(23U);
    setup();
    b.motion_type = NC_MOTION_ARC_CW;
    b.axis_mask = 0x03U;
    b.axis_start[0] = 0;
    b.axis_start[1] = 0;
    b.axis_target[0] = 1000;
    b.axis_target[1] = 1000;
    b.arc_radius = NC_INTERFERENCE_MIN_CLEARANCE - 1;

    NcInterference_CheckBlockTs(&b);

    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.path_interference_warnings);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.slowdown_applied_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_limit_warning_slows_block_and_pushes_event);
    RUN_TEST(test_disabled_or_non_motion_block_is_noop);
    RUN_TEST(test_arc_with_small_radius_records_path_warning);
    printf("test_nc_interference: PASS\n");
    return 0;
}
