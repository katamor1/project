#include "test_common.h"
#include "ts_service.h"
#include "system_shared.h"

static int test_prefetch_and_ui_snapshot_generation(void)
{
    SystemShared_Initialize();
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_machineCtx.machine_state = MACHINE_RUNNING;
    g_machineCtx.cycle_count = 55U;
    g_prefetchReq.request = 1U;
    g_prefetchReq.section_id = 7U;
    TsService_ExecutePrefetch();
    TEST_ASSERT_EQ_UINT(0U, g_prefetchReq.request);
    TEST_ASSERT_EQ_UINT(1U, g_prefetchReq.ready);
    TEST_ASSERT_EQ_UINT(7U, g_prefetchResult.section_id);
    TEST_ASSERT_TRUE(g_prefetchResult.table[1] > g_prefetchResult.table[0]);
    TsService_BuildUiSnapshot();
    TEST_ASSERT_TRUE(g_uiRequest.ui_message_id > 0U);
    TEST_ASSERT_TRUE(strstr((const char*)g_uiRequest.ui_message_text, "mode=1") != NULL);
    return 0;
}

static int test_log_flush_request_clears_flag_and_pushes_audit(void)
{
    SystemShared_Initialize();
    g_uiRequest.log_flush_request = 1U;
    TsService_HandleLogFlush();
    TEST_ASSERT_EQ_UINT(0U, g_uiRequest.log_flush_request);
    TEST_ASSERT_TRUE(g_logQueue.tail != g_logQueue.head);
    TEST_ASSERT_EQ_UINT(LOG_AUDIT, g_logQueue.items[g_logQueue.head].event_type);
    return 0;
}

int main(void)
{
    RUN_TEST(test_prefetch_and_ui_snapshot_generation);
    RUN_TEST(test_log_flush_request_clears_flag_and_pushes_audit);
    printf("test_ts_service_log_and_slice_edges: PASS\n");
    return 0;
}
