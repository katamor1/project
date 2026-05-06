#include "test_common.h"
#include <string.h>
#include "system_shared.h"
#include "ts_service.h"

static void setup(void)
{
    SystemShared_Initialize();
}

static int test_execute_prefetch_fills_table_and_clears_request(void)
{
    setup();
    g_prefetchReq.request = 1U;
    g_prefetchReq.section_id = 2U;
    TsService_ExecutePrefetch();
    TEST_ASSERT_EQ_UINT(0U, g_prefetchReq.request);
    TEST_ASSERT_EQ_UINT(1U, g_prefetchReq.ready);
    TEST_ASSERT_EQ_UINT(2U, g_prefetchResult.section_id);
    TEST_ASSERT_TRUE(g_prefetchResult.table[0] > 2.99F && g_prefetchResult.table[0] < 3.01F);
    TEST_ASSERT_EQ_UINT(g_prefetchReq.generation, g_prefetchResult.generation);
    return 0;
}

static int test_execute_prefetch_noops_while_busy(void)
{
    setup();
    g_prefetchReq.request = 1U;
    g_prefetchReq.busy = 1U;
    TsService_ExecutePrefetch();
    TEST_ASSERT_EQ_UINT(1U, g_prefetchReq.request);
    TEST_ASSERT_EQ_UINT(0U, g_prefetchReq.ready);
    TEST_ASSERT_EQ_UINT(0U, g_prefetchResult.generation);
    return 0;
}

static int test_build_ui_snapshot_increments_id_and_contains_core_state(void)
{
    setup();
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_machineCtx.machine_state = MACHINE_READY;
    g_machineCtx.alarm_code = 777U;
    g_machineCtx.cycle_count = 42U;
    TsService_BuildUiSnapshot();
    TEST_ASSERT_EQ_UINT(1U, g_uiRequest.ui_message_id);
    TEST_ASSERT_TRUE(strstr((const char*)g_uiRequest.ui_message_text, "mode=1") != NULL);
    TEST_ASSERT_TRUE(strstr((const char*)g_uiRequest.ui_message_text, "alarm=777") != NULL);
    return 0;
}

static int test_handle_log_flush_clears_request_and_queues_audit_event(void)
{
    setup();
    g_uiRequest.log_flush_request = 1U;
    TsService_HandleLogFlush();
    TEST_ASSERT_EQ_UINT(0U, g_uiRequest.log_flush_request);
    TEST_ASSERT_TRUE(g_logQueue.tail != g_logQueue.head);
    TEST_ASSERT_EQ_UINT(LOG_AUDIT, g_logQueue.items[g_logQueue.head].event_type);
    TEST_ASSERT_EQ_UINT(2001U, g_logQueue.items[g_logQueue.head].code);
    return 0;
}

int main(void)
{
    RUN_TEST(test_execute_prefetch_fills_table_and_clears_request);
    RUN_TEST(test_execute_prefetch_noops_while_busy);
    RUN_TEST(test_build_ui_snapshot_increments_id_and_contains_core_state);
    RUN_TEST(test_handle_log_flush_clears_request_and_queues_audit_event);
    printf("test_ts_service_prefetch_ui_more: PASS\n");
    return 0;
}
