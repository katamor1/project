#include <string.h>
#include "control_api.h"

int32_t Api_RequestRunModeChange(uint8_t requestedMode)
{
    if (requestedMode > (uint8_t)RUN_MODE_MAINT) {
        g_uiRequest.mode_response = RESPONSE_PARAM_ERROR;
        return -1;
    }

    g_uiRequest.requested_mode = requestedMode;
    g_uiRequest.mode_change_request = 1U;
    g_uiRequest.mode_response = RESPONSE_ACCEPTED;
    return 0;
}

int32_t Api_GetStatusSnapshot(STATUS_SNAPSHOT* pOutSnapshot)
{
    if (pOutSnapshot == 0) {
        return -1;
    }

    (void)memset(pOutSnapshot, 0, sizeof(*pOutSnapshot));
    pOutSnapshot->run_mode = g_machineCtx.run_mode;
    pOutSnapshot->machine_state = g_machineCtx.machine_state;
    pOutSnapshot->alarm_code = g_machineCtx.alarm_code;
    pOutSnapshot->cycle_count = g_machineCtx.cycle_count;
    pOutSnapshot->command_bits = g_ioOut.command_bits;
    pOutSnapshot->sensor_bits = g_ioIn.sensor_bits;
    pOutSnapshot->mode_response = g_uiRequest.mode_response;
    pOutSnapshot->prefetch_ready = g_prefetchReq.ready;
    pOutSnapshot->prefetch_generation = g_prefetchReq.generation;
    return 0;
}

int32_t Api_RequestLogFlush(void)
{
    g_uiRequest.log_flush_request = 1U;
    return 0;
}

int32_t Api_GetPrefetchStatus(uint8_t* pReady, uint16_t* pGeneration)
{
    if ((pReady == 0) || (pGeneration == 0)) {
        return -1;
    }

    *pReady = g_prefetchReq.ready;
    *pGeneration = g_prefetchReq.generation;
    return 0;
}
