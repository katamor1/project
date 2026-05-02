#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_safety_motion.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcSafetyMotion_Reset();
}

static int test_powerfail_has_priority_over_external_decel_and_events_are_edge_triggered(void)
{
    setup();
    g_ncProgramStatus.exec_line_no = 801U;
    g_ioIn.sensor_bits = SENSOR_POWER_FAIL_BIT | SENSOR_EXTERNAL_DECEL_BIT | SENSOR_POSITION_SWITCH_BIT;
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.powerfail_decel_stop_active);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.external_decel_active);
    TEST_ASSERT_EQ_UINT(NC_SAFETY_POWERFAIL_DECEL_PERCENT, g_ncSafetyMotionStatus.active_override_percent);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.powerfail_events);
    TEST_ASSERT_EQ_UINT(0U, g_ncSafetyMotionStatus.external_decel_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.position_switch_events);

    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.powerfail_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.position_switch_events);
    return 0;
}

static int test_absolute_reference_loss_is_counted_on_falling_edge(void)
{
    setup();
    g_ioIn.sensor_bits = SENSOR_ABSOLUTE_REF_VALID_BIT;
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.absolute_reference_valid);

    g_ioIn.sensor_bits = 0U;
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(0U, g_ncSafetyMotionStatus.absolute_reference_valid);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.absolute_ref_lost_events);

    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.absolute_ref_lost_events);
    return 0;
}

static int test_apply_block_null_noop_and_powerfail_minimum_decel_ticks(void)
{
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    b.line_no = 802U;
    b.feed_override_percent = 100U;
    b.interp_ticks = 1U;
    b.decel_ticks = 0U;

    NcSafetyMotion_ApplyBlockRt(NULL);
    g_ioIn.sensor_bits = SENSOR_POWER_FAIL_BIT;
    NcSafetyMotion_UpdateRt();
    NcSafetyMotion_ApplyBlockRt(&b);

    TEST_ASSERT_EQ_UINT(NC_SAFETY_POWERFAIL_DECEL_PERCENT, b.feed_override_percent);
    TEST_ASSERT_EQ_UINT(5U, b.interp_ticks);
    TEST_ASSERT_EQ_UINT(5U, b.decel_ticks);
    TEST_ASSERT_EQ_UINT(802U, g_ncSafetyMotionStatus.last_line_no);
    return 0;
}

int main(void)
{
    RUN_TEST(test_powerfail_has_priority_over_external_decel_and_events_are_edge_triggered);
    RUN_TEST(test_absolute_reference_loss_is_counted_on_falling_edge);
    RUN_TEST(test_apply_block_null_noop_and_powerfail_minimum_decel_ticks);
    printf("test_nc_safety_motion_edges: PASS\n");
    return 0;
}
