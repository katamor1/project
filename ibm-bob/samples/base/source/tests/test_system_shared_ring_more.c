#include "test_common.h"
#include "system_shared.h"

static int test_initialize_sets_cross_module_defaults(void)
{
    SystemShared_Initialize();
    TEST_ASSERT_EQ_UINT(RUN_MODE_MANUAL, g_machineCtx.run_mode);
    TEST_ASSERT_EQ_UINT(MACHINE_STOPPED, g_machineCtx.machine_state);
    TEST_ASSERT_EQ_UINT(NC_IDLE, g_ncProgramStatus.state);
    TEST_ASSERT_EQ_UINT(NC_MOTION_FILTER_SECOND_STAGE_FIR, g_ncMotionFilterStatus.second_stage_mode);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncRotaryMccStatus.mcc_output_enabled);
    return 0;
}

static int test_log_queue_push_captures_cycle_and_rejects_when_full(void)
{
    uint32_t pushed = 0U;
    SystemShared_Initialize();
    g_machineCtx.cycle_count = 1234U;
    TEST_ASSERT_EQ_INT(0, LogQueue_Push(LOG_WARN, 11U, -22));
    TEST_ASSERT_EQ_UINT(LOG_WARN, g_logQueue.items[0].event_type);
    TEST_ASSERT_EQ_UINT(1234U, g_logQueue.items[0].cycle);
    while (LogQueue_Push(LOG_INFO, pushed, 0) == 0) {
        pushed++;
        if (pushed > (LOG_QUEUE_MAX + 2U)) {
            break;
        }
    }
    TEST_ASSERT_TRUE(pushed <= LOG_QUEUE_MAX);
    TEST_ASSERT_EQ_INT(-1, LogQueue_Push(LOG_INFO, 999U, 0));
    return 0;
}

static int test_nc_event_ring_wraps_and_counts_lost_events(void)
{
    uint32_t i;
    SystemShared_Initialize();
    g_machineCtx.cycle_count = 5U;
    g_ncProgramStatus.state = NC_RUNNING;
    for (i = 0U; i < (NC_EVENT_RING_SIZE + 3U); ++i) {
        NcEvent_Push(8000U + i, (int32_t)i, -(int32_t)i, 100U + i);
    }
    TEST_ASSERT_EQ_UINT(NC_EVENT_RING_SIZE, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT(3U, g_ncEventRing.lost_count);
    TEST_ASSERT_EQ_UINT(3U, g_ncEventRing.write_index);
    TEST_ASSERT_EQ_UINT(NC_RUNNING, g_ncEventRing.items[2].nc_state);
    TEST_ASSERT_EQ_UINT(8000U + NC_EVENT_RING_SIZE + 2U, g_ncEventRing.items[2].code);
    return 0;
}

static int test_axis_and_tool_default_tables_are_initialized(void)
{
    SystemShared_Initialize();
    TEST_ASSERT_EQ_UINT((uint8_t)'X', g_ncAxisConfigStatus.axis_name[0]);
    TEST_ASSERT_EQ_UINT((uint8_t)'A', g_ncAxisConfigStatus.axis_name[3]);
    TEST_ASSERT_EQ_UINT(NC_AXIS_TYPE_ROTARY, g_ncAxisConfigStatus.axis_type[3]);
    TEST_ASSERT_EQ_UINT(NC_TOOL_LIFE_DEFAULT_LIMIT, g_ncToolLifeStatus.tool_life_limit[1]);
    TEST_ASSERT_TRUE(g_ncToolManagementStatus.registered_tools > 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.table_initialized);
    return 0;
}

int main(void)
{
    RUN_TEST(test_initialize_sets_cross_module_defaults);
    RUN_TEST(test_log_queue_push_captures_cycle_and_rejects_when_full);
    RUN_TEST(test_nc_event_ring_wraps_and_counts_lost_events);
    RUN_TEST(test_axis_and_tool_default_tables_are_initialized);
    printf("test_system_shared_ring_more: PASS\n");
    return 0;
}
