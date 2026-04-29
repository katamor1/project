/* ibm-bob/samples/base/source/src/control_api.c */
#include "nc_motion_filter.h"
/* Implements public request APIs for the IBM BOB baseline sample. */
/* This exists so callers register work and read snapshots without doing RT work. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/control_api.h, ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/src/nc_program.c */
#include <string.h>
#include "control_api.h"
#include "nc_program.h"
#include "nc_feed.h"
#include "nc_diagnostics.h"
#include "nc_compensation.h"
#include "nc_kinematics.h"
#include "nc_interference.h"

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

int32_t Api_RequestNcBinaryProgramLoad(const NC_EXEC_BLOCK* pBlocks, uint32_t count)
{
    if ((pBlocks == 0) || (count == 0U) ||
        (count > NC_BINARY_PROGRAM_MAX_BLOCKS) ||
        (g_machineCtx.run_mode != RUN_MODE_AUTO) || IsNcBusyForLoad()) {
        g_ncProgramStatus.response_code = RESPONSE_REJECTED;
        return -1;
    }
    if (NcProgram_RequestBinaryLoad(pBlocks, count) != 0) {
        g_ncProgramStatus.response_code = RESPONSE_REJECTED;
        return -1;
    }
    MarkNcRequestAccepted();
    return 0;
}

int32_t Api_GetNcBinaryProgramStatus(NC_BINARY_PROGRAM_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }
    (void)memcpy(pOutStatus,
                 (const void*)&g_ncBinaryProgramStatus,
                 sizeof(*pOutStatus));
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

int32_t Api_GetNcFeatureStatus(NC_FEATURE_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_ncFeatureStatus,
                 sizeof(*pOutStatus));
    return 0;
}


int32_t Api_GetPrestartInterlockStatus(PRESTART_INTERLOCK_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_prestartInterlockStatus,
                 sizeof(*pOutStatus));
    return 0;
}

int32_t Api_GetNcAuxStatus(NC_AUX_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_ncAuxStatus,
                 sizeof(*pOutStatus));
    return 0;
}

int32_t Api_GetNcCycleStatus(NC_CYCLE_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_ncCycleStatus,
                 sizeof(*pOutStatus));
    return 0;
}

int32_t Api_GetIoTraceBuffer(IO_TRACE_BUFFER* pOutBuffer)
{
    if (pOutBuffer == 0) {
        return -1;
    }

    (void)memcpy(pOutBuffer,
                 (const void*)&g_ioTraceBuffer,
                 sizeof(*pOutBuffer));
    return 0;
}

int32_t Api_SetNcToolLengthOffset(uint16_t offsetNo, int32_t offset)
{
    return NcCompensation_SetToolLengthOffset(offsetNo, offset);
}

int32_t Api_SetNcCutterRadiusOffset(uint16_t offsetNo, int32_t offset)
{
    return NcCompensation_SetCutterRadiusOffset(offsetNo, offset);
}

int32_t Api_GetNcCompensationStatus(NC_COMPENSATION_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_ncCompensationStatus,
                 sizeof(*pOutStatus));
    return 0;
}

int32_t Api_GetNcPathControlStatus(NC_PATH_CONTROL_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_ncPathControlStatus,
                 sizeof(*pOutStatus));
    return 0;
}


int32_t Api_SetNcAxisAssignment(uint8_t logicalAxis,
                                uint8_t physicalAxis,
                                int8_t sign,
                                uint8_t detached)
{
    return NcKinematics_SetAxisAssignment(logicalAxis, physicalAxis, sign, detached);
}

int32_t Api_SetNcMirrorMask(uint32_t axisMask)
{
    return NcKinematics_SetMirrorMask(axisMask);
}

int32_t Api_GetNcKinematicsStatus(NC_KINEMATICS_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_ncKinematicsStatus,
                 sizeof(*pOutStatus));
    return 0;
}


int32_t Api_SetNcToolLifeLimit(uint16_t toolNo, uint32_t limit)
{
    return NcDiagnostics_SetToolLifeLimit(toolNo, limit);
}

int32_t Api_GetNcAxisLoadStatus(NC_AXIS_LOAD_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_ncAxisLoadStatus,
                 sizeof(*pOutStatus));
    return 0;
}

int32_t Api_GetNcToolLifeStatus(NC_TOOL_LIFE_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }

    (void)memcpy(pOutStatus,
                 (const void*)&g_ncToolLifeStatus,
                 sizeof(*pOutStatus));
    return 0;
}

int32_t Api_GetNcDiagnosticSnapshot(NC_DIAGNOSTIC_SNAPSHOT* pOutSnapshot)
{
    if (pOutSnapshot == 0) {
        return -1;
    }

    (void)memcpy(pOutSnapshot,
                 (const void*)&g_ncDiagnosticSnapshot,
                 sizeof(*pOutSnapshot));
    return 0;
}

int32_t Api_SetNcMotionFilterConfig(uint8_t enabled,
                                      uint8_t secondStageMode,
                                      uint8_t velocityWindow,
                                      uint8_t accelWindow)
{
    return NcMotionFilter_SetConfigRt(enabled,
                                      secondStageMode,
                                      velocityWindow,
                                      accelWindow);
}

int32_t Api_GetNcMotionFilterStatus(NC_MOTION_FILTER_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncMotionFilterStatus;
    return 0;
}

int32_t Api_GetNcEventRing(NC_EVENT_RING* pOutRing)
{
    if (pOutRing == NULL) {
        return -1;
    }
    *pOutRing = g_ncEventRing;
    return 0;
}

int32_t Api_SetNcMotionFilterAxisLimit(uint8_t axis,
                                       int32_t maxVelocityPerTick,
                                       int32_t maxAccelPerTick)
{
    return NcMotionFilter_SetAxisLimitRt(axis, maxVelocityPerTick, maxAccelPerTick);
}

int32_t Api_GetNcCapabilityStatus(NC_CAPABILITY_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncCapabilityStatus;
    return 0;
}

int32_t Api_SetNcInterferenceCheckEnabled(uint8_t enabled)
{
    return NcInterference_SetEnabled(enabled);
}

int32_t Api_GetNcInterferenceStatus(NC_INTERFERENCE_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncInterferenceStatus;
    return 0;
}

int32_t Api_GetNcSafetyMotionStatus(NC_SAFETY_MOTION_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncSafetyMotionStatus;
    return 0;
}
