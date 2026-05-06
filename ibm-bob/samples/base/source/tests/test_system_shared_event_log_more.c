#include "test_common.h"
#include "system_shared.h"

static int test_initialize_restores_selected_default_tables_and_modes(void)
{
    SystemShared_Initialize();
    TEST_ASSERT_EQ_UINT(RUN_MODE_MANUAL, g_machineCtx.run_mode);
    TEST_ASSERT_EQ_UINT(MACHINE_STOPPED, g_machineCtx.machine_state);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.table_initialized);
    TEST_ASSERT_EQ_UINT(1U, g_ncToolManagementStatus.tool_to_pocket[1]);
    TEST_ASSERT_EQ_UINT(NC_SYNC_DEFAULT_SLAVE_MASK, g_ncSynchronizationStatus.slave_axis_mask);
    TEST_ASSERT_EQ_UINT(NC_AXIS_CONFIG_DEFAULT_PATH_MASK, g_ncAxisConfigStatus.active_path_axis_mask);
    return 0;
}

static int test_log_queue_records_event_type_code_value_and_cycle(void)
{
    SystemShared_Initialize();
    g_machineCtx.cycle_count = 55U;
    TEST_ASSERT_EQ_INT(0, LogQueue_Push(LOG_ALARM, 9001U, -123));
    TEST_ASSERT_EQ_UINT(1U, g_logQueue.tail);
    TEST_ASSERT_EQ_UINT(LOG_ALARM, g_logQueue.items[0].event_type);
    TEST_ASSERT_EQ_UINT(9001U, g_logQueue.items[0].code);
    TEST_ASSERT_EQ_INT(-123, g_logQueue.items[0].value);
    TEST_ASSERT_EQ_UINT(55U, g_logQueue.items[0].cycle);
    return 0;
}

static int test_nc_event_ring_wraps_and_counts_lost_entries(void)
{
    uint32_t i;
    SystemShared_Initialize();
    g_ncProgramStatus.state = NC_RUNNING;
    for (i = 0U; i < (NC_EVENT_RING_SIZE + 1U); ++i) {
        NcEvent_Push(100U + i, (int32_t)i, 0, i);
    }
    TEST_ASSERT_EQ_UINT(NC_EVENT_RING_SIZE, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT(1U, g_ncEventRing.lost_count);
    TEST_ASSERT_EQ_UINT(1U, g_ncEventRing.write_index);
    TEST_ASSERT_EQ_UINT(100U + NC_EVENT_RING_SIZE, g_ncEventRing.items[0].code);
    TEST_ASSERT_EQ_UINT(NC_RUNNING, g_ncEventRing.items[0].nc_state);
    return 0;
}

static int test_reinitialize_clears_rings_and_restores_axis_names(void)
{
    SystemShared_Initialize();
    (void)LogQueue_Push(LOG_INFO, 1U, 2);
    NcEvent_Push(3U, 4, 5, 6U);
    g_ncAxisConfigStatus.axis_name[0] = (uint8_t)'Q';
    SystemShared_Initialize();
    TEST_ASSERT_EQ_UINT(g_logQueue.head, g_logQueue.tail);
    TEST_ASSERT_EQ_UINT(0U, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT((uint8_t)'X', g_ncAxisConfigStatus.axis_name[0]);
    TEST_ASSERT_EQ_UINT((uint8_t)'A', g_ncAxisConfigStatus.axis_name[3]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_initialize_restores_selected_default_tables_and_modes);
    RUN_TEST(test_log_queue_records_event_type_code_value_and_cycle);
    RUN_TEST(test_nc_event_ring_wraps_and_counts_lost_entries);
    RUN_TEST(test_reinitialize_clears_rings_and_restores_axis_names);
    printf("test_system_shared_event_log_more: PASS\n");
    return 0;
}
