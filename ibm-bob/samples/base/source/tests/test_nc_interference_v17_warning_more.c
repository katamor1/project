#include "test_common.h"
#include "system_shared.h"
#include "nc_interference.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_linear(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.feed_override_percent = 100U;
    return b;
}

static int test_disabled_interference_check_is_noop(void)
{
    NC_EXEC_BLOCK b = make_linear(1801U);
    SystemShared_Initialize();
    NcInterference_Reset();
    TEST_ASSERT_EQ_INT(0, NcInterference_SetEnabled(0U));
    b.axis_target[0] = NC_INTERFERENCE_AXIS_LIMIT + 100;
    NcInterference_CheckBlockTs(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncInterferenceStatus.checked_blocks);
    TEST_ASSERT_EQ_UINT(100U, b.feed_override_percent);
    return 0;
}

static int test_axis_limit_warning_reduces_feed_override_and_records_axis(void)
{
    NC_EXEC_BLOCK b = make_linear(1802U);
    SystemShared_Initialize();
    NcInterference_Reset();
    b.axis_start[0] = 0;
    b.axis_target[0] = NC_INTERFERENCE_AXIS_LIMIT + 1;
    NcInterference_CheckBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.checked_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.axis_limit_warnings);
    TEST_ASSERT_EQ_UINT(3U, g_ncInterferenceStatus.last_warning_kind);
    TEST_ASSERT_EQ_UINT(NC_INTERFERENCE_SLOWDOWN_PERCENT, b.feed_override_percent);
    TEST_ASSERT_EQ_UINT(1U, g_ncEventRing.captured_count);
    return 0;
}

static int test_three_axis_near_limit_warns_three_d_interference(void)
{
    NC_EXEC_BLOCK b = make_linear(1803U);
    SystemShared_Initialize();
    NcInterference_Reset();
    b.axis_start[0] = NC_INTERFERENCE_AXIS_LIMIT - 1000;
    b.axis_start[1] = NC_INTERFERENCE_AXIS_LIMIT - 1000;
    b.axis_start[2] = NC_INTERFERENCE_AXIS_LIMIT - 1000;
    b.axis_target[0] = NC_INTERFERENCE_AXIS_LIMIT - 100;
    b.axis_target[1] = NC_INTERFERENCE_AXIS_LIMIT - 100;
    b.axis_target[2] = NC_INTERFERENCE_AXIS_LIMIT - 100;
    NcInterference_CheckBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.three_d_interference_warnings);
    TEST_ASSERT_EQ_UINT(2U, g_ncInterferenceStatus.last_warning_kind);
    TEST_ASSERT_EQ_UINT(NC_INTERFERENCE_SLOWDOWN_PERCENT, b.feed_override_percent);
    return 0;
}

static int test_small_radius_arc_warns_path_interference(void)
{
    NC_EXEC_BLOCK b = make_linear(1804U);
    SystemShared_Initialize();
    NcInterference_Reset();
    b.motion_type = NC_MOTION_ARC_CW;
    b.axis_start[0] = 0;
    b.axis_start[1] = 0;
    b.axis_target[0] = 100;
    b.axis_target[1] = 100;
    b.arc_radius = NC_INTERFERENCE_MIN_CLEARANCE - 1;
    NcInterference_CheckBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.path_interference_warnings);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.last_warning_kind);
    TEST_ASSERT_EQ_UINT(NC_INTERFERENCE_SLOWDOWN_PERCENT, b.feed_override_percent);
    NcInterference_CheckBlockTs(NULL);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.path_interference_warnings);
    return 0;
}

int main(void)
{
    RUN_TEST(test_disabled_interference_check_is_noop);
    RUN_TEST(test_axis_limit_warning_reduces_feed_override_and_records_axis);
    RUN_TEST(test_three_axis_near_limit_warns_three_d_interference);
    RUN_TEST(test_small_radius_arc_warns_path_interference);
    printf("test_nc_interference_v17_warning_more: PASS\n");
    return 0;
}
