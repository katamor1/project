#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_safety_motion.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcSafetyMotion_Reset();
}

static int test_external_decel_and_apply_block_reduces_override(void)
{
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    b.line_no = 31U;
    b.feed_override_percent = 100U;
    g_ncProgramStatus.exec_line_no = 31U;
    g_ioIn.sensor_bits = SENSOR_EXTERNAL_DECEL_BIT | SENSOR_ABSOLUTE_REF_VALID_BIT;

    NcSafetyMotion_UpdateRt();
    NcSafetyMotion_ApplyBlockRt(&b);

    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.external_decel_active);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.external_decel_events);
    TEST_ASSERT_EQ_UINT(NC_SAFETY_EXTERNAL_DECEL_PERCENT, g_ncSafetyMotionStatus.active_override_percent);
    TEST_ASSERT_EQ_UINT(NC_SAFETY_EXTERNAL_DECEL_PERCENT, b.feed_override_percent);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_EXTERNAL_DECEL, g_ncEventRing.items[0].code);
    return 0;
}

static int test_powerfail_has_priority_and_forces_minimum_ticks(void)
{
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    b.line_no = 32U;
    b.feed_override_percent = 100U;
    b.interp_ticks = 1U;
    b.decel_ticks = 1U;
    g_ioIn.sensor_bits = SENSOR_POWER_FAIL_BIT | SENSOR_EXTERNAL_DECEL_BIT;

    NcSafetyMotion_UpdateRt();
    NcSafetyMotion_ApplyBlockRt(&b);

    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.powerfail_decel_stop_active);
    TEST_ASSERT_EQ_UINT(NC_SAFETY_POWERFAIL_DECEL_PERCENT, b.feed_override_percent);
    TEST_ASSERT_EQ_UINT(5U, b.interp_ticks);
    TEST_ASSERT_EQ_UINT(5U, b.decel_ticks);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.powerfail_events);
    return 0;
}

static int test_position_switch_latches_only_on_rising_edge(void)
{
    setup();
    g_ioIn.sensor_bits = SENSOR_POSITION_SWITCH_BIT;
    NcSafetyMotion_UpdateRt();
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.position_switch_latched);
    TEST_ASSERT_EQ_UINT(1U, g_ncSafetyMotionStatus.position_switch_events);
    return 0;
}

int main(void)
{
    RUN_TEST(test_external_decel_and_apply_block_reduces_override);
    RUN_TEST(test_powerfail_has_priority_and_forces_minimum_ticks);
    RUN_TEST(test_position_switch_latches_only_on_rising_edge);
    printf("test_nc_safety_motion: PASS\n");
    return 0;
}
