#include "test_common.h"
#include "system_shared.h"

static int test_defaults_after_initialize_are_safe_and_deterministic(void)
{
    SystemShared_Initialize();
    TEST_ASSERT_EQ_UINT(RUN_MODE_MANUAL, g_machineCtx.run_mode);
    TEST_ASSERT_EQ_UINT(MACHINE_STOPPED, g_machineCtx.machine_state);
    TEST_ASSERT_EQ_UINT(NC_IDLE, g_ncProgramStatus.state);
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, g_ncProgramStatus.error_code);
    TEST_ASSERT_EQ_UINT(NC_FEED_OVERRIDE_DEFAULT_PERCENT, g_ncFeedStatus.feed_override_percent);
    TEST_ASSERT_EQ_UINT(1U, g_ncMotionFilterStatus.enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncTurningCycleStatus.radius_mode_active);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.table_initialized);
    TEST_ASSERT_EQ_UINT(NC_AXIS_CONFIG_DEFAULT_PATH_MASK, g_ncAxisConfigStatus.active_path_axis_mask);
    TEST_ASSERT_EQ_UINT(NC_ROTARY_MCC_DEFAULT_AXIS_MASK, g_ncRotaryMccStatus.configured_axis_mask);
    return 0;
}

static int test_event_records_cycle_state_and_reinitialize_clears_side_effects(void)
{
    SystemShared_Initialize();
    g_machineCtx.cycle_count = 321U;
    g_ncProgramStatus.state = NC_RUNNING;
    NcEvent_Push(12345U, -7, 8, 99U);
    TEST_ASSERT_EQ_UINT(1U, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT(12345U, g_ncEventRing.items[0].code);
    TEST_ASSERT_EQ_UINT(321U, g_ncEventRing.items[0].cycle);
    TEST_ASSERT_EQ_UINT(NC_RUNNING, g_ncEventRing.items[0].nc_state);
    TEST_ASSERT_EQ_INT(-7, g_ncEventRing.items[0].value0);
    TEST_ASSERT_EQ_INT(8, g_ncEventRing.items[0].value1);

    (void)LogQueue_Push(LOG_ALARM, 777U, -1);
    TEST_ASSERT_TRUE(g_logQueue.tail != g_logQueue.head);
    SystemShared_Initialize();
    TEST_ASSERT_EQ_UINT(0U, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT(0U, g_ncEventRing.write_index);
    TEST_ASSERT_EQ_UINT(g_logQueue.head, g_logQueue.tail);
    TEST_ASSERT_EQ_UINT(NC_IDLE, g_ncProgramStatus.state);
    return 0;
}

static int test_default_tables_are_restored_after_mutation(void)
{
    SystemShared_Initialize();
    g_ncToolManagementStatus.tool_to_pocket[1] = 99U;
    g_ncRotaryMccStatus.radius[3] = 123U;
    g_ncMotionFilterStatus.max_velocity_per_tick[0] = 1;
    g_ncReferenceStatus.approach_direction[0] = -1;
    SystemShared_Initialize();
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.tool_to_pocket[1]);
    TEST_ASSERT_EQ_INT(NC_ROTARY_MCC_DEFAULT_RADIUS, g_ncRotaryMccStatus.radius[3]);
    TEST_ASSERT_EQ_INT(NC_MOTION_FILTER_DEFAULT_MAX_VELOCITY, g_ncMotionFilterStatus.max_velocity_per_tick[0]);
    TEST_ASSERT_EQ_INT(1, g_ncReferenceStatus.approach_direction[0]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_defaults_after_initialize_are_safe_and_deterministic);
    RUN_TEST(test_event_records_cycle_state_and_reinitialize_clears_side_effects);
    RUN_TEST(test_default_tables_are_restored_after_mutation);
    printf("test_system_shared_defaults_reinit_edges: PASS\n");
    return 0;
}
