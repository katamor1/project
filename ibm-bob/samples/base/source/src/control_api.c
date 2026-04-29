/* ibm-bob/samples/base/source/src/control_api.c */
/* Implements public request APIs for the IBM BOB baseline sample. */
/* This exists so callers register work and read snapshots without doing RT work. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/control_api.h, ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/src/nc_program.c */
#include <string.h>
#include "control_api.h"
#include "nc_feed.h"

static int32_t IsNcBusyForLoad(void)
{
    return ((g_ncProgramStatus.state == NC_LOAD_REQUESTED) ||
            (g_ncProgramStatus.state == NC_PREFETCHING) ||
            (g_ncProgramStatus.state == NC_READY) ||
            (g_ncProgramStatus.state == NC_RUNNING) ||
            (g_ncProgramStatus.state == NC_HOLD));
}

static void MarkNcRequestAccepted(void)
{
    g_ncProgramStatus.response_code = RESPONSE_ACCEPTED;
    g_ncProgramRequest.request_seq++;
}

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

int32_t Api_RequestNcProgramLoad(const char* filePath)
{
    size_t length;

    if (filePath == 0) {
        g_ncProgramStatus.response_code = RESPONSE_PARAM_ERROR;
        return -1;
    }

    length = strlen(filePath);
    if ((length == 0U) || (length >= NC_FILE_PATH_MAX) ||
        (g_machineCtx.run_mode != RUN_MODE_AUTO) || IsNcBusyForLoad()) {
        g_ncProgramStatus.response_code = RESPONSE_REJECTED;
        return -1;
    }

    (void)memset((void*)g_ncProgramRequest.file_path, 0, NC_FILE_PATH_MAX);
    (void)memcpy((void*)g_ncProgramRequest.file_path, filePath, length);
    g_ncProgramRequest.load_request = 1U;
    g_ncProgramRequest.start_request = 0U;
    g_ncProgramRequest.hold_request = 0U;
    g_ncProgramRequest.stop_request = 0U;
    g_ncProgramRequest.resume_request = 0U;
    g_ncProgramRequest.reset_request = 0U;
    MarkNcRequestAccepted();
    return 0;
}

int32_t Api_RequestNcProgramStart(void)
{
    if (g_ncProgramStatus.state != NC_READY) {
        g_ncProgramStatus.response_code = RESPONSE_REJECTED;
        return -1;
    }

    g_ncProgramRequest.start_request = 1U;
    MarkNcRequestAccepted();
    return 0;
}

int32_t Api_RequestNcProgramHold(void)
{
    if (g_ncProgramStatus.state != NC_RUNNING) {
        g_ncProgramStatus.response_code = RESPONSE_REJECTED;
        return -1;
    }

    g_ncProgramRequest.hold_request = 1U;
    MarkNcRequestAccepted();
    return 0;
}

int32_t Api_RequestNcProgramStop(void)
{
    g_ncProgramRequest.stop_request = 1U;
    MarkNcRequestAccepted();
    return 0;
}

int32_t Api_RequestNcProgramResume(void)
{
    if (g_ncProgramStatus.state != NC_HOLD) {
        g_ncProgramStatus.response_code = RESPONSE_REJECTED;
        return -1;
    }

    g_ncProgramRequest.resume_request = 1U;
    MarkNcRequestAccepted();
    return 0;
}

int32_t Api_GetNcProgramStatus(NC_PROGRAM_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_ncProgramStatus,
                 sizeof(*pOutStatus));
    return 0;
}

int32_t Api_GetNcInterpolationStatus(NC_INTERP_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_ncInterpStatus,
                 sizeof(*pOutStatus));
    return 0;
}

int32_t Api_SetNcFeedOverride(uint16_t percent)
{
    return NcFeed_SetOverride(percent);
}

int32_t Api_GetNcFeedStatus(NC_FEED_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_ncFeedStatus,
                 sizeof(*pOutStatus));
    return 0;
}

int32_t Api_GetNcCoordinateStatus(NC_COORDINATE_STATE* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_ncCoordinateState,
                 sizeof(*pOutStatus));
    return 0;
}
