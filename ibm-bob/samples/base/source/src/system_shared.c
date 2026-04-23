#include <string.h>
#include "system_shared.h"

volatile IO_INPUT_AREA      g_ioIn;
volatile IO_OUTPUT_AREA     g_ioOut;
volatile MACHINE_CONTEXT    g_machineCtx;
volatile UI_REQUEST_AREA    g_uiRequest;
volatile LOG_QUEUE          g_logQueue;
volatile PREFETCH_REQ_CTRL  g_prefetchReq;
volatile PREFETCH_RESULT    g_prefetchResult;

void SystemShared_Initialize(void)
{
    (void)memset((void*)&g_ioIn, 0, sizeof(g_ioIn));
    (void)memset((void*)&g_ioOut, 0, sizeof(g_ioOut));
    (void)memset((void*)&g_machineCtx, 0, sizeof(g_machineCtx));
    (void)memset((void*)&g_uiRequest, 0, sizeof(g_uiRequest));
    (void)memset((void*)&g_logQueue, 0, sizeof(g_logQueue));
    (void)memset((void*)&g_prefetchReq, 0, sizeof(g_prefetchReq));
    (void)memset((void*)&g_prefetchResult, 0, sizeof(g_prefetchResult));

    g_machineCtx.run_mode = RUN_MODE_MANUAL;
    g_machineCtx.machine_state = MACHINE_STOPPED;
    g_uiRequest.mode_response = RESPONSE_NONE;
}

int32_t LogQueue_Push(uint32_t eventType, uint32_t code, int32_t value)
{
    uint16_t nextTail = (uint16_t)((g_logQueue.tail + 1U) % LOG_QUEUE_MAX);
    LOG_ITEM* pItem;

    if (nextTail == g_logQueue.head) {
        return -1;
    }

    pItem = (LOG_ITEM*)&g_logQueue.items[g_logQueue.tail];
    pItem->event_type = eventType;
    pItem->code = code;
    pItem->value = value;
    pItem->cycle = g_machineCtx.cycle_count;
    g_logQueue.tail = nextTail;

    return 0;
}
