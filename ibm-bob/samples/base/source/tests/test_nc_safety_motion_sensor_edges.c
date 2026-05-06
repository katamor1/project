#include "test_common.h"
#include "system_shared.h"
#include "nc_safety_motion.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_block(void)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = 1771U;
    b.feed_override_percent = 100U;
    b.interp_ticks = 1U;
    return b;
}

static int test_external_decel_sets_override_and_event_once(void)
{
    SystemShared_Initialize();
    NcSafetyMotion_Reset();
    g_ioIn.sensor_bits = SENSOR_EXTERNAL_DECEL_BIT;
    NcSafetyMotion_UpdateRt();
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.external_decel_active);
    TEST_ASSERT_EQ_UINT(NC_SAFETY_EXTERNAL_DECEL_PERCENT, g_ncSafetyMotionStatus.active_override_percent);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.external_decel_events);
    return 0;
}

static int test_powerfail_has_priority_over_external_decel_and_extends_short_block(void)
{
    NC_EXEC_BLOCK b = make_block();
    SystemShared_Initialize();
    NcSafetyMotion_Reset();
    g_ioIn.sensor_bits = SENSOR_POWER_FAIL_BIT | SENSOR_EXTERNAL_DECEL_BIT;
    NcSafetyMotion_UpdateRt();
    NcSafetyMotion_ApplyBlockRt(&b);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.powerfail_decel_stop_active);
    TEST_ASSERT_EQ_UINT(NC_SAFETY_POWERFAIL_DECEL_PERCENT, g_ncSafetyMotionStatus.active_override_percent);
    TEST_ASSERT_EQ_UINT(NC_SAFETY_POWERFAIL_DECEL_PERCENT, b.feed_override_percent);
    TEST_ASSERT_EQ_UINT(5U, b.interp_ticks);
    TEST_ASSERT_EQ_UINT(5U, b.decel_ticks);
    return 0;
}

static int test_position_switch_latch_is_rising_edge_only(void)
{
    SystemShared_Initialize();
    NcSafetyMotion_Reset();
    g_ioIn.sensor_bits = SENSOR_POSITION_SWITCH_BIT;
    NcSafetyMotion_UpdateRt();
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.position_switch_latched);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.position_switch_events);
    return 0;
}

static int test_absolute_reference_lost_detects_valid_to_invalid_transition(void)
{
    SystemShared_Initialize();
    NcSafetyMotion_Reset();
    g_ioIn.sensor_bits = SENSOR_ABSOLUTE_REF_VALID_BIT;
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.absolute_reference_valid);
    g_ioIn.sensor_bits = 0U;
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(0U, g_ncSafetyMotionStatus.absolute_reference_valid);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.absolute_ref_lost_events);
    NcSafetyMotion_ApplyBlockRt(NULL);
    return 0;
}

int main(void)
{
    RUN_TEST(test_external_decel_sets_override_and_event_once);
    RUN_TEST(test_powerfail_has_priority_over_external_decel_and_extends_short_block);
    RUN_TEST(test_position_switch_latch_is_rising_edge_only);
    RUN_TEST(test_absolute_reference_lost_detects_valid_to_invalid_transition);
    printf("test_nc_safety_motion_sensor_edges: PASS\n");
    return 0;
}
