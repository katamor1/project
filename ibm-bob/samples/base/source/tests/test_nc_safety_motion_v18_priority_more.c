#include "test_common.h"
#include "system_shared.h"
#include "nc_safety_motion.h"
#include "nc_program.h"

static int test_external_decel_rising_edge_counts_once(void)
{
    SystemShared_Initialize();
    NcSafetyMotion_Reset();
    g_ioIn.sensor_bits = SENSOR_EXTERNAL_DECEL_BIT;
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(NC_SAFETY_EXTERNAL_DECEL_PERCENT, g_ncSafetyMotionStatus.active_override_percent);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.external_decel_events);
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.external_decel_events);
    return 0;
}

static int test_powerfail_has_priority_over_external_decel(void)
{
    SystemShared_Initialize();
    NcSafetyMotion_Reset();
    g_ioIn.sensor_bits = SENSOR_EXTERNAL_DECEL_BIT | SENSOR_POWER_FAIL_BIT;
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(NC_SAFETY_POWERFAIL_DECEL_PERCENT, g_ncSafetyMotionStatus.active_override_percent);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.powerfail_events);
    TEST_ASSERT_EQ_UINT(0U, g_ncSafetyMotionStatus.external_decel_events);
    return 0;
}

static int test_position_switch_latches_only_on_rising_edge(void)
{
    SystemShared_Initialize();
    NcSafetyMotion_Reset();
    g_ioIn.sensor_bits = SENSOR_POSITION_SWITCH_BIT;
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.position_switch_latched);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.position_switch_events);
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.position_switch_events);
    return 0;
}

static int test_apply_block_reduces_override_and_extends_powerfail_ticks(void)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = 1835U;
    b.feed_override_percent = 100U;
    b.interp_ticks = 2U;
    b.decel_ticks = 0U;
    SystemShared_Initialize();
    NcSafetyMotion_Reset();
    g_ioIn.sensor_bits = SENSOR_POWER_FAIL_BIT;
    NcSafetyMotion_UpdateRt();
    NcSafetyMotion_ApplyBlockRt(&b);
    TEST_ASSERT_EQ_UINT(NC_SAFETY_POWERFAIL_DECEL_PERCENT, b.feed_override_percent);
    TEST_ASSERT_EQ_UINT(5U, b.interp_ticks);
    TEST_ASSERT_EQ_UINT(5U, b.decel_ticks);
    TEST_ASSERT_EQ_UINT(1835U, g_ncSafetyMotionStatus.last_line_no);
    return 0;
}

int main(void)
{
    RUN_TEST(test_external_decel_rising_edge_counts_once);
    RUN_TEST(test_powerfail_has_priority_over_external_decel);
    RUN_TEST(test_position_switch_latches_only_on_rising_edge);
    RUN_TEST(test_apply_block_reduces_override_and_extends_powerfail_ticks);
    printf("test_nc_safety_motion_v18_priority_more: PASS\n");
    return 0;
}
