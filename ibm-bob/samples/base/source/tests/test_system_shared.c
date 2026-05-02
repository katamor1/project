#include "test_common.h"
#include "system_shared.h"

static int test_initialize_sets_safe_defaults(void)
{
    SystemShared_Initialize();
    TEST_ASSERT_EQ_UINT(RUN_MODE_MANUAL, g_machineCtx.run_mode);
    TEST_ASSERT_EQ_UINT(MACHINE_STOPPED, g_machineCtx.machine_state);
    TEST_ASSERT_EQ_UINT(NC_IDLE, g_ncProgramStatus.state);
    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, g_ncProgramStatus.error_code);
    TEST_ASSERT_EQ_UINT(NC_FEED_OVERRIDE_DEFAULT_PERCENT, g_ncFeedStatus.feed_override_percent);
    TEST_ASSERT_TRUE((g_prestartInterlockStatus.mask & PRESTART_ILK_READY_SWITCH) != 0U);
    TEST_ASSERT_EQ_UINT(0U, g_prestartInterlockStatus.ok);
    TEST_ASSERT_EQ_UINT(1U, g_ncMotionFilterStatus.enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncInterferenceStatus.enabled);
    return 0;
}

static int test_log_queue_push_and_full_boundary(void)
{
    uint32_t i;
    SystemShared_Initialize();
    for (i = 0U; i < (LOG_QUEUE_MAX - 1U); ++i) {
        TEST_ASSERT_EQ_INT(0, LogQueue_Push(LOG_INFO, 1000U + i, (int32_t)i));
    }
    TEST_ASSERT_EQ_UINT(0U, g_logQueue.head);
    TEST_ASSERT_EQ_UINT((uint16_t)(LOG_QUEUE_MAX - 1U), g_logQueue.tail);
    TEST_ASSERT_EQ_INT(-1, LogQueue_Push(LOG_WARN, 9999U, -1));
    TEST_ASSERT_EQ_UINT((uint16_t)(LOG_QUEUE_MAX - 1U), g_logQueue.tail);
    TEST_ASSERT_EQ_UINT(1000U, g_logQueue.items[0].code);
    return 0;
}

static int test_nc_event_ring_wrap_and_lost_count(void)
{
    uint32_t i;
    SystemShared_Initialize();
    g_machineCtx.cycle_count = 77U;
    g_ncProgramStatus.state = NC_RUNNING;
    for (i = 0U; i < (NC_EVENT_RING_SIZE + 2U); ++i) {
        NcEvent_Push(8000U + i, (int32_t)i, -(int32_t)i, i + 1U);
    }
    TEST_ASSERT_EQ_UINT(NC_EVENT_RING_SIZE, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT(2U, g_ncEventRing.lost_count);
    TEST_ASSERT_EQ_UINT(2U, g_ncEventRing.write_index);
    TEST_ASSERT_EQ_UINT(8064U, g_ncEventRing.items[0].code);
    TEST_ASSERT_EQ_UINT(8065U, g_ncEventRing.items[1].code);
    TEST_ASSERT_EQ_UINT(NC_RUNNING, g_ncEventRing.items[1].nc_state);
    return 0;
}

int main(void)
{
    RUN_TEST(test_initialize_sets_safe_defaults);
    RUN_TEST(test_log_queue_push_and_full_boundary);
    RUN_TEST(test_nc_event_ring_wrap_and_lost_count);
    printf("test_system_shared: PASS\n");
    return 0;
}
