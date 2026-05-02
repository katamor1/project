/* ibm-bob/samples/base/source/src/control_api.c */
/* Implements public request APIs for the IBM BOB baseline sample. */
/* This exists so callers register work and read snapshots without doing RT work. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/control_api.h, ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/src/nc_program.c */
#include <string.h>
#include "control_api.h"
#include "nc_program.h"
#include "nc_feed.h"
#include "nc_diagnostics.h"
#include "nc_compensation.h"
#include "nc_coordinate_transform.h"
#include "nc_kinematics.h"
#include "nc_interference.h"
#include "nc_lathe_cycle.h"
#include "nc_reference.h"
#include "nc_precision.h"
#include "nc_spindle.h"
#include "nc_tool_management.h"
#include "nc_synchronization.h"
#include "nc_rotary_mcc.h"
#include "nc_axis_config.h"
#include "nc_design_features.h"
#include "nc_feature_backlog.h"
#include "nc_motion_filter.h"

/**
 * @brief Return true when the NC program state cannot accept a new load request.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
static int32_t IsNcBusyForLoad(void)
{
    return ((g_ncProgramStatus.state == NC_LOAD_REQUESTED) ||
            (g_ncProgramStatus.state == NC_PREFETCHING) ||
            (g_ncProgramStatus.state == NC_READY) ||
            (g_ncProgramStatus.state == NC_RUNNING) ||
            (g_ncProgramStatus.state == NC_HOLD));
}

/**
 * @brief Mark the current NC request as accepted and advance its request sequence.
 */
static void MarkNcRequestAccepted(void)
{
    g_ncProgramStatus.response_code = RESPONSE_ACCEPTED;
    g_ncProgramRequest.request_seq++;
}

/**
 * @brief Request run mode change through the public API.
 * @param requestedMode Input value for requested mode.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the status snapshot to the public API caller.
 * @param pOutSnapshot Output pointer receiving snapshot.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Request log flush through the public API.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_RequestLogFlush(void)
{
    g_uiRequest.log_flush_request = 1U;
    return 0;
}

/**
 * @brief Copy the prefetch status snapshot to the public API caller.
 * @param pReady Pointer to ready used by this operation.
 * @param pGeneration Pointer to generation used by this operation.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetPrefetchStatus(uint8_t* pReady, uint16_t* pGeneration)
{
    if ((pReady == 0) || (pGeneration == 0)) {
        return -1;
    }

    *pReady = g_prefetchReq.ready;
    *pGeneration = g_prefetchReq.generation;
    return 0;
}

/**
 * @brief Request NC program load through the public API.
 * @param filePath Input value for file path.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Request NC binary program load through the public API.
 * @param pBlocks Array of NC execution blocks supplied by the caller.
 * @param count Number of entries supplied by the caller.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the NC binary program status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Request NC program start through the public API.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Request NC program hold through the public API.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Request NC program stop through the public API.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_RequestNcProgramStop(void)
{
    g_ncProgramRequest.stop_request = 1U;
    MarkNcRequestAccepted();
    return 0;
}

/**
 * @brief Request NC program resume through the public API.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the NC program status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the NC interpolation status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Set NC feed override through the public API.
 * @param percent Override or percentage value to apply.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcFeedOverride(uint16_t percent)
{
    return NcFeed_SetOverride(percent);
}

/**
 * @brief Copy the NC feed status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the NC coordinate status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the NC coordinate transform status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcCoordinateTransformStatus(NC_COORDINATE_TRANSFORM_STATUS* pOutStatus)
{
    if (pOutStatus == 0) {
        return -1;
    }
    (void)memcpy(pOutStatus,
                 (const void*)&g_ncCoordinateTransformStatus,
                 sizeof(*pOutStatus));
    return 0;
}

/**
 * @brief Set NC coordinate transform enabled through the public API.
 * @param dynamicFixture Input value for dynamic fixture.
 * @param workMountError Input value for work mount error.
 * @param rotaryFixture Input value for rotary fixture.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcCoordinateTransformEnabled(uint8_t dynamicFixture,
                                            uint8_t workMountError,
                                            uint8_t rotaryFixture)
{
    return NcCoordinateTransform_SetEnabled(dynamicFixture,
                                            workMountError,
                                            rotaryFixture);
}

/**
 * @brief Set NC work offset through the public API.
 * @param workIndex Index selecting work.
 * @param axis Axis index to read or update.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcWorkOffset(uint8_t workIndex, uint8_t axis, int32_t offset)
{
    return NcCoordinateTransform_SetWorkOffset(workIndex, axis, offset);
}

/**
 * @brief Set NC local shift through the public API.
 * @param axis Axis index to read or update.
 * @param shift Input value for shift.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcLocalShift(uint8_t axis, int32_t shift)
{
    return NcCoordinateTransform_SetLocalShift(axis, shift);
}

/**
 * @brief Set NC temporary absolute through the public API.
 * @param axis Axis index to read or update.
 * @param programPosition Input value for program position.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcTemporaryAbsolute(uint8_t axis, int32_t programPosition)
{
    return NcCoordinateTransform_SetTemporaryAbsolute(axis, programPosition);
}

/**
 * @brief Set NC dynamic fixture offset through the public API.
 * @param axis Axis index to read or update.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcDynamicFixtureOffset(uint8_t axis, int32_t offset)
{
    return NcCoordinateTransform_SetDynamicFixtureOffset(axis, offset);
}

/**
 * @brief Set NC work mount error through the public API.
 * @param axis Axis index to read or update.
 * @param error Input value for error.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcWorkMountError(uint8_t axis, int32_t error)
{
    return NcCoordinateTransform_SetWorkMountError(axis, error);
}

/**
 * @brief Set NC rotary table offset through the public API.
 * @param axis Axis index to read or update.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcRotaryTableOffset(uint8_t axis, int32_t offset)
{
    return NcCoordinateTransform_SetRotaryTableOffset(axis, offset);
}

/**
 * @brief Copy the NC feature status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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


/**
 * @brief Copy the prestart interlock status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the NC aux status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the NC cycle status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the IO trace buffer snapshot to the public API caller.
 * @param pOutBuffer Output pointer receiving buffer.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Set NC tool length offset through the public API.
 * @param offsetNo Input value for offset no.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcToolLengthOffset(uint16_t offsetNo, int32_t offset)
{
    return NcCompensation_SetToolLengthOffset(offsetNo, offset);
}

/**
 * @brief Set NC cutter radius offset through the public API.
 * @param offsetNo Input value for offset no.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcCutterRadiusOffset(uint16_t offsetNo, int32_t offset)
{
    return NcCompensation_SetCutterRadiusOffset(offsetNo, offset);
}

/**
 * @brief Copy the NC compensation status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the NC path control status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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


/**
 * @brief Set NC axis assignment through the public API.
 * @param logicalAxis Input value for logical axis.
 * @param physicalAxis Input value for physical axis.
 * @param sign Input value for sign.
 * @param detached Input value for detached.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcAxisAssignment(uint8_t logicalAxis,
                                uint8_t physicalAxis,
                                int8_t sign,
                                uint8_t detached)
{
    return NcKinematics_SetAxisAssignment(logicalAxis, physicalAxis, sign, detached);
}

/**
 * @brief Set NC mirror mask through the public API.
 * @param axisMask Bit mask of axes affected by the request.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcMirrorMask(uint32_t axisMask)
{
    return NcKinematics_SetMirrorMask(axisMask);
}

/**
 * @brief Copy the NC kinematics status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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


/**
 * @brief Set NC tool life limit through the public API.
 * @param toolNo Input value for tool no.
 * @param limit Input value for limit.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcToolLifeLimit(uint16_t toolNo, uint32_t limit)
{
    return NcDiagnostics_SetToolLifeLimit(toolNo, limit);
}

/**
 * @brief Copy the NC axis load status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the NC tool life status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the NC diagnostic snapshot to the public API caller.
 * @param pOutSnapshot Output pointer receiving snapshot.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Set NC motion filter config through the public API.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param secondStageMode Input value for second stage mode.
 * @param velocityWindow Input value for velocity window.
 * @param accelWindow Input value for accel window.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
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

/**
 * @brief Copy the NC motion filter status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcMotionFilterStatus(NC_MOTION_FILTER_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncMotionFilterStatus;
    return 0;
}

/**
 * @brief Copy the NC event ring snapshot to the public API caller.
 * @param pOutRing Output pointer receiving ring.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcEventRing(NC_EVENT_RING* pOutRing)
{
    if (pOutRing == NULL) {
        return -1;
    }
    *pOutRing = g_ncEventRing;
    return 0;
}

/**
 * @brief Set NC motion filter axis limit through the public API.
 * @param axis Axis index to read or update.
 * @param maxVelocityPerTick Input value for max velocity per tick.
 * @param maxAccelPerTick Input value for max accel per tick.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcMotionFilterAxisLimit(uint8_t axis,
                                       int32_t maxVelocityPerTick,
                                       int32_t maxAccelPerTick)
{
    return NcMotionFilter_SetAxisLimitRt(axis, maxVelocityPerTick, maxAccelPerTick);
}

/**
 * @brief Copy the NC capability status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcCapabilityStatus(NC_CAPABILITY_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncCapabilityStatus;
    return 0;
}

/**
 * @brief Set NC interference check enabled through the public API.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcInterferenceCheckEnabled(uint8_t enabled)
{
    return NcInterference_SetEnabled(enabled);
}

/**
 * @brief Copy the NC interference status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcInterferenceStatus(NC_INTERFERENCE_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncInterferenceStatus;
    return 0;
}

/**
 * @brief Copy the NC safety motion status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcSafetyMotionStatus(NC_SAFETY_MOTION_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncSafetyMotionStatus;
    return 0;
}

/**
 * @brief Copy the NC turning cycle status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcTurningCycleStatus(NC_TURNING_CYCLE_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncTurningCycleStatus;
    return 0;
}

/**
 * @brief Copy the NC thread cycle status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcThreadCycleStatus(NC_THREAD_CYCLE_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncThreadCycleStatus;
    return 0;
}

/**
 * @brief Set NC diameter mode through the public API.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcDiameterMode(uint8_t enabled)
{
    int32_t result = NcLatheCycle_SetDiameterMode(enabled);
    if (result == 0) {
        (void)NcAxisConfig_SetDiameterMode(0U, enabled);
    }
    return result;
}

/**
 * @brief Copy the NC reference status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcReferenceStatus(NC_REFERENCE_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncReferenceStatus;
    return 0;
}

/**
 * @brief Set NC reference axis config through the public API.
 * @param axis Axis index to read or update.
 * @param referencePosition Input value for reference position.
 * @param approachDirection Input value for approach direction.
 * @param rolloverLimit Input value for rollover limit.
 * @param distanceCodedMarker Input value for distance coded marker.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcReferenceAxisConfig(uint8_t axis,
                                     int32_t referencePosition,
                                     int8_t approachDirection,
                                     int32_t rolloverLimit,
                                     uint8_t distanceCodedMarker)
{
    return NcReference_SetAxisConfig(axis,
                                     referencePosition,
                                     approachDirection,
                                     rolloverLimit,
                                     distanceCodedMarker);
}

/**
 * @brief Set NC one direction approach through the public API.
 * @param axis Axis index to read or update.
 * @param approachAmount Input value for approach amount.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcOneDirectionApproach(uint8_t axis, int32_t approachAmount)
{
    return NcReference_SetOneDirectionApproach(axis, approachAmount);
}

/**
 * @brief Copy the NC precision status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcPrecisionStatus(NC_PRECISION_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncPrecisionStatus;
    return 0;
}

/**
 * @brief Set NC learning control through the public API.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param gainPercent Input value for gain percent.
 * @param memoryWindow Input value for memory window.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcLearningControl(uint8_t enabled,
                                 int32_t gainPercent,
                                 uint8_t memoryWindow)
{
    return NcPrecision_SetLearningControl(enabled, gainPercent, memoryWindow);
}

/**
 * @brief Set NC vibration suppression through the public API.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param notchFreqHz Input value for notch freq hz.
 * @param dampingPercent Input value for damping percent.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcVibrationSuppression(uint8_t enabled,
                                       uint16_t notchFreqHz,
                                       uint16_t dampingPercent)
{
    return NcPrecision_SetVibrationControl(enabled, notchFreqHz, dampingPercent);
}

/**
 * @brief Set NC preview control through the public API.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param lookaheadBlocks Input value for lookahead blocks.
 * @param cornerTolerance Input value for corner tolerance.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcPreviewControl(uint8_t enabled,
                                uint16_t lookaheadBlocks,
                                uint16_t cornerTolerance)
{
    return NcPrecision_SetPreviewControl(enabled, lookaheadBlocks, cornerTolerance);
}

/**
 * @brief Copy the NC spindle status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcSpindleStatus(NC_SPINDLE_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncSpindleStatus;
    return 0;
}

/**
 * @brief Set NC spindle speed limit through the public API.
 * @param minRpm Input value for min RPM.
 * @param maxRpm Input value for max RPM.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcSpindleSpeedLimit(uint32_t minRpm, uint32_t maxRpm)
{
    return NcSpindle_SetSpeedLimit(minRpm, maxRpm);
}

/**
 * @brief Set NC spindle orient window through the public API.
 * @param timeoutTicks Input value for timeout ticks.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcSpindleOrientWindow(uint16_t timeoutTicks)
{
    return NcSpindle_SetOrientWindow(timeoutTicks);
}

/**
 * @brief Copy the NC tool management status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcToolManagementStatus(NC_TOOL_MANAGEMENT_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncToolManagementStatus;
    return 0;
}

/**
 * @brief Set NC tool pocket through the public API.
 * @param toolNo Input value for tool no.
 * @param pocketNo Input value for pocket no.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcToolPocket(uint32_t toolNo, uint32_t pocketNo)
{
    return NcToolManagement_SetPocket(toolNo, pocketNo);
}

/**
 * @brief Request NC tool prepare through the public API.
 * @param toolNo Input value for tool no.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_RequestNcToolPrepare(uint32_t toolNo)
{
    return NcToolManagement_RequestPrepare(toolNo);
}

/**
 * @brief Copy the NC synchronization status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcSynchronizationStatus(NC_SYNCHRONIZATION_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncSynchronizationStatus;
    return 0;
}

/**
 * @brief Set NC synchronization master slave through the public API.
 * @param masterAxis Input value for master axis.
 * @param slaveAxisMask Bit mask for slave axis.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcSynchronizationMasterSlave(uint8_t masterAxis, uint32_t slaveAxisMask)
{
    return NcSynchronization_SetMasterSlave(masterAxis, slaveAxisMask);
}

/**
 * @brief Set NC overlay axis through the public API.
 * @param axis Axis index to read or update.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcOverlayAxis(uint8_t axis, int32_t offset)
{
    return NcSynchronization_SetOverlayAxis(axis, offset);
}

/**
 * @brief Set NC compound path mask through the public API.
 * @param pathMask Bit mask for path.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcCompoundPathMask(uint32_t pathMask)
{
    return NcSynchronization_SetCompoundPathMask(pathMask);
}

/**
 * @brief Set NC double table control through the public API.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param slaveAxisMask Bit mask for slave axis.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcDoubleTableControl(uint8_t enabled, uint32_t slaveAxisMask)
{
    return NcSynchronization_SetDoubleTable(enabled, slaveAxisMask);
}

/**
 * @brief Copy the NC rotary MCC status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcRotaryMccStatus(NC_ROTARY_MCC_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncRotaryMccStatus;
    return 0;
}

/**
 * @brief Set NC rotary axis radius through the public API.
 * @param axis Axis index to read or update.
 * @param radius Input value for radius.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcRotaryAxisRadius(uint8_t axis, int32_t radius)
{
    return NcRotaryMcc_SetAxisRadius(axis, radius);
}

/**
 * @brief Set NC MCC output through the public API.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcMccOutput(uint8_t enabled)
{
    return NcRotaryMcc_SetMccOutput(enabled);
}

/**
 * @brief Copy the NC axis config status snapshot to the public API caller.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcAxisConfigStatus(NC_AXIS_CONFIG_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) {
        return -1;
    }
    *pOutStatus = g_ncAxisConfigStatus;
    return 0;
}

/**
 * @brief Set NC axis definition through the public API.
 * @param axis Axis index to read or update.
 * @param axisName Input value for axis name.
 * @param axisType Input value for axis type.
 * @param coordinateGroup Input value for coordinate group.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcAxisDefinition(uint8_t axis,
                                uint8_t axisName,
                                uint8_t axisType,
                                uint8_t coordinateGroup)
{
    return NcAxisConfig_SetAxisDefinition(axis, axisName, axisType, coordinateGroup);
}

/**
 * @brief Set NC path axis mask through the public API.
 * @param axisMask Bit mask of axes affected by the request.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcPathAxisMask(uint32_t axisMask)
{
    return NcAxisConfig_SetPathAxisMask(axisMask);
}

/**
 * @brief Set NC axis detached mask through the public API.
 * @param axisMask Bit mask of axes affected by the request.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcAxisDetachedMask(uint32_t axisMask)
{
    return NcAxisConfig_SetDetachedMask(axisMask);
}

/**
 * @brief Set NC axis diameter mode through the public API.
 * @param axis Axis index to read or update.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcAxisDiameterMode(uint8_t axis, uint8_t enabled)
{
    return NcAxisConfig_SetDiameterMode(axis, enabled);
}

int32_t Api_GetNcDesignFeatureStatus(NC_DESIGN_FEATURE_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) { return -1; }
    *pOutStatus = g_ncDesignFeatureStatus;
    return 0;
}

int32_t Api_SetNcDesignAutoCornerOverride(uint8_t enabled, uint16_t overridePercent)
{
    return NcDesignFeatures_SetAutoCornerOverride(enabled, overridePercent);
}

int32_t Api_GetNcImplementationBacklogStatus(NC_IMPLEMENTATION_BACKLOG_STATUS* pOutStatus)
{
    if (pOutStatus == NULL) { return -1; }
    *pOutStatus = g_ncImplementationBacklogStatus;
    return 0;
}

int32_t Api_SetNcImplementationBacklogFeature(uint16_t featureId, uint8_t enabled, uint16_t parameter)
{
    return NcFeatureBacklog_SetFeature(featureId, enabled, parameter);
}

int32_t Api_EnableAllNcImplementationBacklogFeatures(uint16_t parameter)
{
    return NcFeatureBacklog_EnableAll(parameter);
}

int32_t Api_RunNcImplementationBacklogSelfCheck(void)
{
    return NcFeatureBacklog_RunSelfCheck();
}
