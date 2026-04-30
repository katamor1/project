/* ibm-bob/samples/base/source/src/ts_service.c */
/* Runs non-real-time service work for logs, prefetch, UI, and NC file parsing. */
/* This exists so file I/O and variable-time parsing stay out of RT control. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/ts_service.h, ibm-bob/samples/base/source/inc/nc_program.h, ibm-bob/samples/base/source/src/nc_program.c */
#include <stdio.h>
#include <string.h>
#include "nc_program.h"
#include "ts_service.h"
#include "system_shared.h"
#include "system_config.h"

static FILE* s_logFile = NULL;

/**
 * @brief Handle ensure log file open for this module.
 */
static void EnsureLogFileOpen(void)
{
    if (s_logFile == NULL) {
        s_logFile = fopen("runtime_log.csv", "a");
    }
}

/**
 * @brief Run one TS service time-share service slice.
 */
void TsService_ExecuteSlice(void)
{
    TsService_HandleLogFlush();
    TsService_WritePendingLogs();
    TsService_ExecutePrefetch();
    TsNcProgram_ExecuteSlice();
    TsService_BuildUiSnapshot();
}

/**
 * @brief Handle TS service handle log flush for this module.
 */
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

/**
 * @brief Write pending logs for the smoke-test scenario.
 */
void TsService_WritePendingLogs(void)
{
    EnsureLogFileOpen();
    if (s_logFile == NULL) {
        return;
    }

    while (g_logQueue.head != g_logQueue.tail) {
        const volatile LOG_ITEM* pItem = &g_logQueue.items[g_logQueue.head];
        (void)fprintf(s_logFile, "%u,%u,%d,%u\n",
                      pItem->event_type,
                      pItem->code,
                      pItem->value,
                      pItem->cycle);
        g_logQueue.head = (uint16_t)((g_logQueue.head + 1U) % LOG_QUEUE_MAX);
    }

    (void)fflush(s_logFile);
}

/**
 * @brief Handle TS service execute prefetch for this module.
 */
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

/**
 * @brief Build UI snapshot from current shared state.
 */
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
