#include <string.h>
#include "test_common.h"
#include "nc_interference.h"

static NC_EXEC_BLOCK make_block(uint32_t line)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x03U;
    b.axis_start[0] = 0;
    b.axis_start[1] = 0;
    b.axis_target[0] = 1000;
    b.axis_target[1] = 1000;
    b.feed_override_percent = 100U;
    return b;
}

static int test_reset_enables_check_and_set_enabled_toggles_generation(void)
{
    uint32_t before;
    SystemShared_Initialize();
    NcInterference_Reset();
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.enabled);
    before = g_ncInterferenceStatus.generation;
    TEST_ASSERT_EQ_INT(0, NcInterference_SetEnabled(0U));
    TEST_ASSERT_EQ_UINT(0U, g_ncInterferenceStatus.enabled);
    TEST_ASSERT_TRUE(g_ncInterferenceStatus.generation > before);
    return 0;
}

static int test_disabled_check_is_noop_for_warning_counters(void)
{
    NC_EXEC_BLOCK b = make_block(12U);
    SystemShared_Initialize();
    NcInterference_Reset();
    TEST_ASSERT_EQ_INT(0, NcInterference_SetEnabled(0U));
    NcInterference_CheckBlockTs(&b);
    TEST_ASSERT_EQ_UINT(0U, g_ncInterferenceStatus.checked_blocks);
    TEST_ASSERT_EQ_UINT(100U, b.feed_override_percent);
    return 0;
}

static int test_axis_limit_negative_clearance_applies_slowdown(void)
{
    NC_EXEC_BLOCK b = make_block(13U);
    SystemShared_Initialize();
    NcInterference_Reset();
    b.axis_target[0] = NC_INTERFERENCE_AXIS_LIMIT + 1000;
    NcInterference_CheckBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.axis_limit_warnings);
    TEST_ASSERT_EQ_UINT(3U, g_ncInterferenceStatus.last_warning_kind);
    TEST_ASSERT_EQ_UINT(NC_INTERFERENCE_SLOWDOWN_PERCENT, b.feed_override_percent);
    return 0;
}

static int test_small_arc_radius_records_path_warning(void)
{
    NC_EXEC_BLOCK b = make_block(14U);
    SystemShared_Initialize();
    NcInterference_Reset();
    b.motion_type = NC_MOTION_ARC_CW;
    b.arc_radius = 100;
    NcInterference_CheckBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.path_interference_warnings);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.last_warning_kind);
    TEST_ASSERT_EQ_UINT(14U, g_ncInterferenceStatus.last_warning_line_no);
    return 0;
}

int main(void)
{
    RUN_TEST(test_reset_enables_check_and_set_enabled_toggles_generation);
    RUN_TEST(test_disabled_check_is_noop_for_warning_counters);
    RUN_TEST(test_axis_limit_negative_clearance_applies_slowdown);
    RUN_TEST(test_small_arc_radius_records_path_warning);
    printf("test_nc_interference_more: PASS\n");
    return 0;
}
