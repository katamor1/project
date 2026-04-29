/* ibm-bob/samples/base/source/src/system_shared.c */
/* Defines and initializes shared memory for the IBM BOB baseline sample. */
/* This exists so RT, TS, API, and NC modules start from known safe values. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/src/control_api.c, ibm-bob/samples/base/source/src/nc_program.c */
#include <string.h>
#include "system_shared.h"

volatile IO_INPUT_AREA      g_ioIn;
volatile IO_OUTPUT_AREA     g_ioOut;
volatile MACHINE_CONTEXT    g_machineCtx;
volatile UI_REQUEST_AREA    g_uiRequest;
volatile LOG_QUEUE          g_logQueue;
volatile PREFETCH_REQ_CTRL  g_prefetchReq;
volatile PREFETCH_RESULT    g_prefetchResult;
volatile NC_PROGRAM_REQUEST g_ncProgramRequest;
volatile NC_PROGRAM_STATUS  g_ncProgramStatus;
volatile NC_EXEC_BUFFER     g_ncProgramBuffer;
volatile NC_COORDINATE_STATE g_ncCoordinateState;
volatile NC_INTERP_STATUS   g_ncInterpStatus;
volatile NC_FEED_STATUS     g_ncFeedStatus;

void SystemShared_Initialize(void)
{
    (void)memset((void*)&g_ioIn, 0, sizeof(g_ioIn));
    (void)memset((void*)&g_ioOut, 0, sizeof(g_ioOut));
    (void)memset((void*)&g_machineCtx, 0, sizeof(g_machineCtx));
    (void)memset((void*)&g_uiRequest, 0, sizeof(g_uiRequest));
    (void)memset((void*)&g_logQueue, 0, sizeof(g_logQueue));
    (void)memset((void*)&g_prefetchReq, 0, sizeof(g_prefetchReq));
    (void)memset((void*)&g_prefetchResult, 0, sizeof(g_prefetchResult));
    (void)memset((void*)&g_ncProgramRequest, 0, sizeof(g_ncProgramRequest));
    (void)memset((void*)&g_ncProgramStatus, 0, sizeof(g_ncProgramStatus));
    (void)memset((void*)&g_ncProgramBuffer, 0, sizeof(g_ncProgramBuffer));
    (void)memset((void*)&g_ncCoordinateState, 0, sizeof(g_ncCoordinateState));
    (void)memset((void*)&g_ncInterpStatus, 0, sizeof(g_ncInterpStatus));
    (void)memset((void*)&g_ncFeedStatus, 0, sizeof(g_ncFeedStatus));

    g_machineCtx.run_mode = RUN_MODE_MANUAL;
    g_machineCtx.machine_state = MACHINE_STOPPED;
    g_uiRequest.mode_response = RESPONSE_NONE;
    g_ncProgramStatus.state = NC_IDLE;
    g_ncProgramStatus.error_code = NC_ERR_NONE;
    g_ncProgramStatus.response_code = RESPONSE_NONE;
    g_ncInterpStatus.state = NC_INTERP_IDLE;
    g_ncInterpStatus.last_error = NC_ERR_NONE;
    g_ncFeedStatus.state = NC_FEED_CTRL_IDLE;
    g_ncFeedStatus.feed_override_percent = NC_FEED_OVERRIDE_DEFAULT_PERCENT;
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
