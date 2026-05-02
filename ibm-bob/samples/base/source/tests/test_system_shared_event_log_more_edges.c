#include "test_common.h"
#include "system_shared.h"

/*
 * Additional shared-memory ring tests for cycle capture, out-of-range write
 * index recovery, and log queue full behavior after manual head movement.
 */

static int test_event_push_recovers_out_of_range_write_index(void)
{
    SystemShared_Initialize();
    g_machineCtx.cycle_count = 1234U;
    g_ncProgramStatus.state = NC_HOLD;
    g_ncEventRing.write_index = NC_EVENT_RING_SIZE + 5U;

    NcEvent_Push(0xABCDU, -10, 20, 77U);
    TEST_ASSERT_EQ_UINT(1U, g_ncEventRing.write_index);
    TEST_ASSERT_EQ_UINT(1U, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT(1U, g_ncEventRing.generation);
    TEST_ASSERT_EQ_UINT(0xABCDU, g_ncEventRing.items[0].code);
    TEST_ASSERT_EQ_INT(-10, g_ncEventRing.items[0].value0);
    TEST_ASSERT_EQ_INT(20, g_ncEventRing.items[0].value1);
    TEST_ASSERT_EQ_UINT(77U, g_ncEventRing.items[0].line_no);
    TEST_ASSERT_EQ_UINT(1234U, g_ncEventRing.items[0].cycle);
    TEST_ASSERT_EQ_UINT(NC_HOLD, g_ncEventRing.items[0].nc_state);
    return 0;
}

static int test_log_queue_preserves_cycle_and_allows_after_manual_head_advance(void)
{
    uint32_t i;
    SystemShared_Initialize();
    g_machineCtx.cycle_count = 42U;

    for (i = 0U; i < (LOG_QUEUE_MAX - 1U); ++i) {
        TEST_ASSERT_EQ_INT(0, LogQueue_Push(LOG_AUDIT, 2000U + i, (int32_t)i));
    }
    TEST_ASSERT_EQ_INT(-1, LogQueue_Push(LOG_ALARM, 9999U, -999));
    TEST_ASSERT_EQ_UINT(42U, g_logQueue.items[0].cycle);

    /* Simulate TS consuming one item by advancing head, then verify one slot opens. */
    g_logQueue.head = (uint16_t)((g_logQueue.head + 1U) % LOG_QUEUE_MAX);
    TEST_ASSERT_EQ_INT(0, LogQueue_Push(LOG_WARN, 3000U, -30));
    TEST_ASSERT_EQ_UINT(0U, g_logQueue.tail);
    TEST_ASSERT_EQ_UINT(3000U, g_logQueue.items[LOG_QUEUE_MAX - 1U].code);
    TEST_ASSERT_EQ_INT(-30, g_logQueue.items[LOG_QUEUE_MAX - 1U].value);
    return 0;
}

int main(void)
{
    RUN_TEST(test_event_push_recovers_out_of_range_write_index);
    RUN_TEST(test_log_queue_preserves_cycle_and_allows_after_manual_head_advance);
    printf("test_system_shared_event_log_more_edges: PASS\n");
    return 0;
}
