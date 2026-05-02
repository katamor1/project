#include "test_common.h"
#include "system_shared.h"
#include "ts_service.h"

static void setup(void)
{
    SystemShared_Initialize();
}

static int test_prefetch_request_populates_result_once(void)
{
    setup();
    g_prefetchReq.request = 1U;
    g_prefetchReq.section_id = 2U;
    TsService_ExecutePrefetch();
    TEST_ASSERT_EQ_UINT(0U, g_prefetchReq.request);
    TEST_ASSERT_EQ_UINT(0U, g_prefetchReq.busy);
    TEST_ASSERT_EQ_UINT(1U, g_prefetchReq.ready);
    TEST_ASSERT_EQ_UINT(1U, g_prefetchReq.generation);
    TEST_ASSERT_EQ_UINT(1U, g_prefetchResult.generation);
    TEST_ASSERT_EQ_UINT(2U, g_prefetchResult.section_id);
    TEST_ASSERT_TRUE(g_prefetchResult.table[0] > 2.9F);
    TEST_ASSERT_TRUE(g_prefetchResult.table[1] > g_prefetchResult.table[0]);
    return 0;
}

static int test_prefetch_busy_is_noop(void)
{
    setup();
    g_prefetchReq.request = 1U;
    g_prefetchReq.busy = 1U;
    g_prefetchReq.section_id = 3U;
    TsService_ExecutePrefetch();
    TEST_ASSERT_EQ_UINT(1U, g_prefetchReq.request);
    TEST_ASSERT_EQ_UINT(1U, g_prefetchReq.busy);
    TEST_ASSERT_EQ_UINT(0U, g_prefetchReq.generation);
    TEST_ASSERT_EQ_UINT(0U, g_prefetchResult.generation);
    return 0;
}

static int test_log_flush_request_cleared_and_audit_log_pushed(void)
{
    setup();
    g_uiRequest.log_flush_request = 1U;
    TsService_HandleLogFlush();
    TEST_ASSERT_EQ_UINT(0U, g_uiRequest.log_flush_request);
    TEST_ASSERT_EQ_UINT(1U, g_logQueue.tail);
    TEST_ASSERT_EQ_UINT(LOG_AUDIT, g_logQueue.items[0].event_type);
    TEST_ASSERT_EQ_UINT(2001U, g_logQueue.items[0].code);
    return 0;
}

static int test_build_ui_snapshot_increments_message_id(void)
{
    setup();
    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_machineCtx.machine_state = MACHINE_RUNNING;
    g_machineCtx.alarm_code = 1234U;
    g_machineCtx.cycle_count = 55U;
    TsService_BuildUiSnapshot();
    TEST_ASSERT_EQ_UINT(1U, g_uiRequest.ui_message_id);
    TEST_ASSERT_TRUE(strstr((const char*)g_uiRequest.ui_message_text, "mode=1") != NULL);
    TEST_ASSERT_TRUE(strstr((const char*)g_uiRequest.ui_message_text, "state=2") != NULL);
    TEST_ASSERT_TRUE(strstr((const char*)g_uiRequest.ui_message_text, "alarm=1234") != NULL);
    return 0;
}

int main(void)
{
    RUN_TEST(test_prefetch_request_populates_result_once);
    RUN_TEST(test_prefetch_busy_is_noop);
    RUN_TEST(test_log_flush_request_cleared_and_audit_log_pushed);
    RUN_TEST(test_build_ui_snapshot_increments_message_id);
    printf("test_ts_service: PASS\n");
    return 0;
}
