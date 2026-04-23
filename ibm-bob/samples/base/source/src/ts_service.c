#include <stdio.h>
#include <string.h>
#include "ts_service.h"
#include "system_shared.h"
#include "system_config.h"

static FILE* s_logFile = NULL;

static void EnsureLogFileOpen(void)
{
    if (s_logFile == NULL) {
        s_logFile = fopen("runtime_log.csv", "a");
    }
}

void TsService_ExecuteSlice(void)
{
    TsService_HandleLogFlush();
    TsService_WritePendingLogs();
    TsService_ExecutePrefetch();
    TsService_BuildUiSnapshot();
}

void TsService_HandleLogFlush(void)
{
    if (g_uiRequest.log_flush_request == 0U) {
        return;
    }

    if (s_logFile != NULL) {
        (void)fflush(s_logFile);
        (void)fclose(s_logFile);
        s_logFile = NULL;
    }

    g_uiRequest.log_flush_request = 0U;
    (void)LogQueue_Push(LOG_AUDIT, 2001U, 0);
}

void TsService_WritePendingLogs(void)
{
    EnsureLogFileOpen();
    if (s_logFile == NULL) {
        return;
    }

    while (g_logQueue.head != g_logQueue.tail) {
        const LOG_ITEM* pItem = &g_logQueue.items[g_logQueue.head];
        (void)fprintf(s_logFile, "%u,%u,%d,%u\n",
                      pItem->event_type,
                      pItem->code,
                      pItem->value,
                      pItem->cycle);
        g_logQueue.head = (uint16_t)((g_logQueue.head + 1U) % LOG_QUEUE_MAX);
    }

    (void)fflush(s_logFile);
}

void TsService_ExecutePrefetch(void)
{
    uint32_t i;
    float base;

    if ((g_prefetchReq.request == 0U) || (g_prefetchReq.busy != 0U)) {
        return;
    }

    g_prefetchReq.busy = 1U;
    base = (float)(g_prefetchReq.section_id + 1U);

    for (i = 0U; i < PREFETCH_TABLE_SIZE; ++i) {
        g_prefetchResult.table[i] = base + ((float)i * 0.05F);
    }

    g_prefetchResult.section_id = g_prefetchReq.section_id;
    g_prefetchReq.generation++;
    g_prefetchResult.generation = g_prefetchReq.generation;
    g_prefetchReq.ready = 1U;
    g_prefetchReq.request = 0U;
    g_prefetchReq.busy = 0U;
}

void TsService_BuildUiSnapshot(void)
{
    (void)snprintf((char*)g_uiRequest.ui_message_text,
                   UI_MESSAGE_TEXT_MAX,
                   "mode=%u state=%u alarm=%u cycle=%u",
                   (unsigned int)g_machineCtx.run_mode,
                   (unsigned int)g_machineCtx.machine_state,
                   (unsigned int)g_machineCtx.alarm_code,
                   (unsigned int)g_machineCtx.cycle_count);
    g_uiRequest.ui_message_id++;
}
