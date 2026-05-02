/* ibm-bob/samples/base/source/inc/control_api.h */
/* Declares the public API for the IBM BOB baseline sample. */
/* This exists so external callers register requests without touching RT state directly. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/control_api.c, ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/src/nc_program.c */
#ifndef CONTROL_API_H
#define CONTROL_API_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Request run mode change through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param requestedMode Input value for requested mode.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_RequestRunModeChange(uint8_t requestedMode);
/**
 * @brief Copy the status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutSnapshot Output pointer receiving snapshot.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetStatusSnapshot(STATUS_SNAPSHOT* pOutSnapshot);
/**
 * @brief Request log flush through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_RequestLogFlush(void);
/**
 * @brief Copy the prefetch status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pReady Pointer to ready used by this operation.
 * @param pGeneration Pointer to generation used by this operation.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetPrefetchStatus(uint8_t* pReady, uint16_t* pGeneration);
/**
 * @brief Request NC program load through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param filePath Input value for file path.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_RequestNcProgramLoad(const char* filePath);
/**
 * @brief Request NC binary program load through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlocks Array of NC execution blocks supplied by the caller.
 * @param count Number of entries supplied by the caller.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_RequestNcBinaryProgramLoad(const NC_EXEC_BLOCK* pBlocks, uint32_t count);
/**
 * @brief Copy the NC binary program status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcBinaryProgramStatus(NC_BINARY_PROGRAM_STATUS* pOutStatus);
/**
 * @brief Request NC program start through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_RequestNcProgramStart(void);
/**
 * @brief Request NC program hold through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_RequestNcProgramHold(void);
/**
 * @brief Request NC program stop through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_RequestNcProgramStop(void);
/**
 * @brief Request NC program resume through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_RequestNcProgramResume(void);
/**
 * @brief Copy the NC program status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcProgramStatus(NC_PROGRAM_STATUS* pOutStatus);
/**
 * @brief Copy the NC interpolation status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcInterpolationStatus(NC_INTERP_STATUS* pOutStatus);
/**
 * @brief Set NC feed override through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param percent Override or percentage value to apply.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcFeedOverride(uint16_t percent);
/**
 * @brief Copy the NC feed status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcFeedStatus(NC_FEED_STATUS* pOutStatus);
/**
 * @brief Copy the NC coordinate status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcCoordinateStatus(NC_COORDINATE_STATE* pOutStatus);
/**
 * @brief Copy the NC coordinate transform status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcCoordinateTransformStatus(NC_COORDINATE_TRANSFORM_STATUS* pOutStatus);
/**
 * @brief Set NC coordinate transform enabled through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param dynamicFixture Input value for dynamic fixture.
 * @param workMountError Input value for work mount error.
 * @param rotaryFixture Input value for rotary fixture.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcCoordinateTransformEnabled(uint8_t dynamicFixture, uint8_t workMountError, uint8_t rotaryFixture);
/**
 * @brief Set NC work offset through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param workIndex Index selecting work.
 * @param axis Axis index to read or update.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcWorkOffset(uint8_t workIndex, uint8_t axis, int32_t offset);
/**
 * @brief Set NC local shift through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param shift Input value for shift.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcLocalShift(uint8_t axis, int32_t shift);
/**
 * @brief Set NC temporary absolute through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param programPosition Input value for program position.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcTemporaryAbsolute(uint8_t axis, int32_t programPosition);
/**
 * @brief Set NC dynamic fixture offset through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcDynamicFixtureOffset(uint8_t axis, int32_t offset);
/**
 * @brief Set NC work mount error through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param error Input value for error.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcWorkMountError(uint8_t axis, int32_t error);
/**
 * @brief Set NC rotary table offset through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcRotaryTableOffset(uint8_t axis, int32_t offset);
/**
 * @brief Copy the NC feature status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcFeatureStatus(NC_FEATURE_STATUS* pOutStatus);
/**
 * @brief Copy the prestart interlock status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetPrestartInterlockStatus(PRESTART_INTERLOCK_STATUS* pOutStatus);
/**
 * @brief Copy the NC aux status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcAuxStatus(NC_AUX_STATUS* pOutStatus);
/**
 * @brief Copy the NC cycle status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcCycleStatus(NC_CYCLE_STATUS* pOutStatus);
/**
 * @brief Copy the IO trace buffer snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutBuffer Output pointer receiving buffer.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetIoTraceBuffer(IO_TRACE_BUFFER* pOutBuffer);
/**
 * @brief Set NC tool length offset through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param offsetNo Input value for offset no.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcToolLengthOffset(uint16_t offsetNo, int32_t offset);
/**
 * @brief Set NC cutter radius offset through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param offsetNo Input value for offset no.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcCutterRadiusOffset(uint16_t offsetNo, int32_t offset);
/**
 * @brief Copy the NC compensation status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcCompensationStatus(NC_COMPENSATION_STATUS* pOutStatus);
/**
 * @brief Copy the NC path control status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcPathControlStatus(NC_PATH_CONTROL_STATUS* pOutStatus);
/**
 * @brief Set NC axis assignment through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param logicalAxis Input value for logical axis.
 * @param physicalAxis Input value for physical axis.
 * @param sign Input value for sign.
 * @param detached Input value for detached.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcAxisAssignment(uint8_t logicalAxis,
                                uint8_t physicalAxis,
                                int8_t sign,
                                uint8_t detached);
/**
 * @brief Set NC mirror mask through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axisMask Bit mask of axes affected by the request.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcMirrorMask(uint32_t axisMask);
/**
 * @brief Copy the NC kinematics status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcKinematicsStatus(NC_KINEMATICS_STATUS* pOutStatus);
/**
 * @brief Set NC tool life limit through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param toolNo Input value for tool no.
 * @param limit Input value for limit.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcToolLifeLimit(uint16_t toolNo, uint32_t limit);
/**
 * @brief Copy the NC axis load status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcAxisLoadStatus(NC_AXIS_LOAD_STATUS* pOutStatus);
/**
 * @brief Copy the NC tool life status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcToolLifeStatus(NC_TOOL_LIFE_STATUS* pOutStatus);
/**
 * @brief Copy the NC diagnostic snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutSnapshot Output pointer receiving snapshot.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcDiagnosticSnapshot(NC_DIAGNOSTIC_SNAPSHOT* pOutSnapshot);
/**
 * @brief Set NC motion filter config through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param secondStageMode Input value for second stage mode.
 * @param velocityWindow Input value for velocity window.
 * @param accelWindow Input value for accel window.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcMotionFilterConfig(uint8_t enabled,
                                      uint8_t secondStageMode,
                                      uint8_t velocityWindow,
                                      uint8_t accelWindow);
/**
 * @brief Copy the NC motion filter status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcMotionFilterStatus(NC_MOTION_FILTER_STATUS* pOutStatus);
/**
 * @brief Set NC motion filter axis limit through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param maxVelocityPerTick Input value for max velocity per tick.
 * @param maxAccelPerTick Input value for max accel per tick.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcMotionFilterAxisLimit(uint8_t axis,
                                       int32_t maxVelocityPerTick,
                                       int32_t maxAccelPerTick);
/**
 * @brief Copy the NC event ring snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutRing Output pointer receiving ring.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcEventRing(NC_EVENT_RING* pOutRing);
/**
 * @brief Copy the NC capability status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcCapabilityStatus(NC_CAPABILITY_STATUS* pOutStatus);
/**
 * @brief Set NC interference check enabled through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcInterferenceCheckEnabled(uint8_t enabled);
/**
 * @brief Copy the NC interference status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcInterferenceStatus(NC_INTERFERENCE_STATUS* pOutStatus);
/**
 * @brief Copy the NC safety motion status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcSafetyMotionStatus(NC_SAFETY_MOTION_STATUS* pOutStatus);
/**
 * @brief Copy the NC turning cycle status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcTurningCycleStatus(NC_TURNING_CYCLE_STATUS* pOutStatus);
/**
 * @brief Copy the NC thread cycle status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcThreadCycleStatus(NC_THREAD_CYCLE_STATUS* pOutStatus);
/**
 * @brief Set NC diameter mode through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcDiameterMode(uint8_t enabled);
/**
 * @brief Copy the NC reference status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcReferenceStatus(NC_REFERENCE_STATUS* pOutStatus);
/**
 * @brief Set NC reference axis config through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param referencePosition Input value for reference position.
 * @param approachDirection Input value for approach direction.
 * @param rolloverLimit Input value for rollover limit.
 * @param distanceCodedMarker Input value for distance coded marker.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcReferenceAxisConfig(uint8_t axis, int32_t referencePosition, int8_t approachDirection, int32_t rolloverLimit, uint8_t distanceCodedMarker);
/**
 * @brief Set NC one direction approach through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param approachAmount Input value for approach amount.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcOneDirectionApproach(uint8_t axis, int32_t approachAmount);
/**
 * @brief Copy the NC precision status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcPrecisionStatus(NC_PRECISION_STATUS* pOutStatus);
/**
 * @brief Set NC learning control through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param gainPercent Input value for gain percent.
 * @param memoryWindow Input value for memory window.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcLearningControl(uint8_t enabled, int32_t gainPercent, uint8_t memoryWindow);
/**
 * @brief Set NC vibration suppression through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param notchFreqHz Input value for notch freq hz.
 * @param dampingPercent Input value for damping percent.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcVibrationSuppression(uint8_t enabled, uint16_t notchFreqHz, uint16_t dampingPercent);
/**
 * @brief Set NC preview control through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param lookaheadBlocks Input value for lookahead blocks.
 * @param cornerTolerance Input value for corner tolerance.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcPreviewControl(uint8_t enabled, uint16_t lookaheadBlocks, uint16_t cornerTolerance);
/**
 * @brief Copy the NC spindle status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcSpindleStatus(NC_SPINDLE_STATUS* pOutStatus);
/**
 * @brief Set NC spindle speed limit through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param minRpm Input value for min RPM.
 * @param maxRpm Input value for max RPM.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcSpindleSpeedLimit(uint32_t minRpm, uint32_t maxRpm);
/**
 * @brief Set NC spindle orient window through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param timeoutTicks Input value for timeout ticks.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcSpindleOrientWindow(uint16_t timeoutTicks);
/**
 * @brief Copy the NC tool management status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcToolManagementStatus(NC_TOOL_MANAGEMENT_STATUS* pOutStatus);
/**
 * @brief Set NC tool pocket through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param toolNo Input value for tool no.
 * @param pocketNo Input value for pocket no.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcToolPocket(uint32_t toolNo, uint32_t pocketNo);
/**
 * @brief Request NC tool prepare through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param toolNo Input value for tool no.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_RequestNcToolPrepare(uint32_t toolNo);
/**
 * @brief Copy the NC synchronization status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcSynchronizationStatus(NC_SYNCHRONIZATION_STATUS* pOutStatus);
/**
 * @brief Set NC synchronization master slave through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param masterAxis Input value for master axis.
 * @param slaveAxisMask Bit mask for slave axis.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcSynchronizationMasterSlave(uint8_t masterAxis, uint32_t slaveAxisMask);
/**
 * @brief Set NC overlay axis through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcOverlayAxis(uint8_t axis, int32_t offset);
/**
 * @brief Set NC compound path mask through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pathMask Bit mask for path.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcCompoundPathMask(uint32_t pathMask);
/**
 * @brief Set NC double table control through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param slaveAxisMask Bit mask for slave axis.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcDoubleTableControl(uint8_t enabled, uint32_t slaveAxisMask);
/**
 * @brief Copy the NC rotary MCC status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcRotaryMccStatus(NC_ROTARY_MCC_STATUS* pOutStatus);
/**
 * @brief Set NC rotary axis radius through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param radius Input value for radius.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcRotaryAxisRadius(uint8_t axis, int32_t radius);
/**
 * @brief Set NC MCC output through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcMccOutput(uint8_t enabled);
/**
 * @brief Copy the NC axis config status snapshot to the public API caller.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutStatus Output pointer receiving status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_GetNcAxisConfigStatus(NC_AXIS_CONFIG_STATUS* pOutStatus);
/**
 * @brief Set NC axis definition through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param axisName Input value for axis name.
 * @param axisType Input value for axis type.
 * @param coordinateGroup Input value for coordinate group.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcAxisDefinition(uint8_t axis, uint8_t axisName, uint8_t axisType, uint8_t coordinateGroup);
/**
 * @brief Set NC path axis mask through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axisMask Bit mask of axes affected by the request.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcPathAxisMask(uint32_t axisMask);
/**
 * @brief Set NC axis detached mask through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axisMask Bit mask of axes affected by the request.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcAxisDetachedMask(uint32_t axisMask);
/**
 * @brief Set NC axis diameter mode through the public API.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t Api_SetNcAxisDiameterMode(uint8_t axis, uint8_t enabled);

int32_t Api_GetNcDesignFeatureStatus(NC_DESIGN_FEATURE_STATUS* pOutStatus);
int32_t Api_SetNcDesignAutoCornerOverride(uint8_t enabled, uint16_t overridePercent);

int32_t Api_GetNcImplementationBacklogStatus(NC_IMPLEMENTATION_BACKLOG_STATUS* pOutStatus);
int32_t Api_SetNcImplementationBacklogFeature(uint16_t featureId, uint8_t enabled, uint16_t parameter);
int32_t Api_EnableAllNcImplementationBacklogFeatures(uint16_t parameter);
int32_t Api_RunNcImplementationBacklogSelfCheck(void);

#endif /* CONTROL_API_H */
