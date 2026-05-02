#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_safety_motion.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcSafetyMotion_Reset();
}

static int test_reset_restores_default_override_and_clears_latch(void)
{
    setup();
    g_ncSafetyMotionStatus.position_switch_latched = 1U;
    g_ncSafetyMotionStatus.active_override_percent = 1U;
    NcSafetyMotion_Reset();
    TEST_ASSERT_EQ_UINT(NC_FEED_OVERRIDE_DEFAULT_PERCENT, g_ncSafetyMotionStatus.active_override_percent);
    TEST_ASSERT_EQ_UINT(0U, g_ncSafetyMotionStatus.position_switch_latched);
    return 0;
}

static int test_external_decel_event_rearms_after_signal_goes_low(void)
{
    setup();
    g_ioIn.sensor_bits = SENSOR_EXTERNAL_DECEL_BIT;
    NcSafetyMotion_UpdateRt();
    g_ioIn.sensor_bits = 0U;
    NcSafetyMotion_UpdateRt();
    g_ioIn.sensor_bits = SENSOR_EXTERNAL_DECEL_BIT;
    NcSafetyMotion_UpdateRt();
    TEST_ASSERT_EQ_UINT(2U, g_ncSafetyMotionStatus.external_decel_events);
    TEST_ASSERT_EQ_UINT(NC_SAFETY_EXTERNAL_DECEL_PERCENT, g_ncSafetyMotionStatus.active_override_percent);
    return 0;
}

static int test_apply_block_does_not_raise_existing_lower_override(void)
{
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    b.line_no = 61U;
    b.feed_override_percent = 10U;
    g_ioIn.sensor_bits = SENSOR_EXTERNAL_DECEL_BIT;
    NcSafetyMotion_UpdateRt();
    NcSafetyMotion_ApplyBlockRt(&b);
    TEST_ASSERT_EQ_UINT(10U, b.feed_override_percent);
    TEST_ASSERT_EQ_UINT(0U, g_ncSafetyMotionStatus.last_line_no);
    return 0;
}

static int test_powerfail_minimum_ticks_do_not_shrink_longer_block(void)
{
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    b.line_no = 62U;
    b.feed_override_percent = 100U;
    b.interp_ticks = 100U;
    b.decel_ticks = 80U;
    g_ioIn.sensor_bits = SENSOR_POWER_FAIL_BIT;
    NcSafetyMotion_UpdateRt();
    NcSafetyMotion_ApplyBlockRt(&b);
    TEST_ASSERT_EQ_UINT(100U, b.interp_ticks);
    TEST_ASSERT_EQ_UINT(80U, b.decel_ticks);
    TEST_ASSERT_EQ_UINT(NC_SAFETY_POWERFAIL_DECEL_PERCENT, b.feed_override_percent);
    return 0;
}

int main(void)
{
    RUN_TEST(test_reset_restores_default_override_and_clears_latch);
    RUN_TEST(test_external_decel_event_rearms_after_signal_goes_low);
    RUN_TEST(test_apply_block_does_not_raise_existing_lower_override);
    RUN_TEST(test_powerfail_minimum_ticks_do_not_shrink_longer_block);
    printf("test_nc_safety_motion_v15_latch_release_more: PASS\n");
    return 0;
}
