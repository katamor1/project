/* ibm-bob/samples/base/source/inc/control_api.h */
/* Declares the public API for the IBM BOB baseline sample. */
/* This exists so external callers register requests without touching RT state directly. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/control_api.c, ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/src/nc_program.c */
#ifndef CONTROL_API_H
#define CONTROL_API_H

#include <stdint.h>
#include "system_shared.h"

int32_t Api_RequestRunModeChange(uint8_t requestedMode);
int32_t Api_GetStatusSnapshot(STATUS_SNAPSHOT* pOutSnapshot);
int32_t Api_RequestLogFlush(void);
int32_t Api_GetPrefetchStatus(uint8_t* pReady, uint16_t* pGeneration);
int32_t Api_RequestNcProgramLoad(const char* filePath);
int32_t Api_RequestNcBinaryProgramLoad(const NC_EXEC_BLOCK* pBlocks, uint32_t count);
int32_t Api_GetNcBinaryProgramStatus(NC_BINARY_PROGRAM_STATUS* pOutStatus);
int32_t Api_RequestNcProgramStart(void);
int32_t Api_RequestNcProgramHold(void);
int32_t Api_RequestNcProgramStop(void);
int32_t Api_RequestNcProgramResume(void);
int32_t Api_GetNcProgramStatus(NC_PROGRAM_STATUS* pOutStatus);
int32_t Api_GetNcInterpolationStatus(NC_INTERP_STATUS* pOutStatus);
int32_t Api_SetNcFeedOverride(uint16_t percent);
int32_t Api_GetNcFeedStatus(NC_FEED_STATUS* pOutStatus);
int32_t Api_GetNcCoordinateStatus(NC_COORDINATE_STATE* pOutStatus);
int32_t Api_GetNcCoordinateTransformStatus(NC_COORDINATE_TRANSFORM_STATUS* pOutStatus);
int32_t Api_SetNcCoordinateTransformEnabled(uint8_t dynamicFixture, uint8_t workMountError, uint8_t rotaryFixture);
int32_t Api_SetNcWorkOffset(uint8_t workIndex, uint8_t axis, int32_t offset);
int32_t Api_SetNcLocalShift(uint8_t axis, int32_t shift);
int32_t Api_SetNcTemporaryAbsolute(uint8_t axis, int32_t programPosition);
int32_t Api_SetNcDynamicFixtureOffset(uint8_t axis, int32_t offset);
int32_t Api_SetNcWorkMountError(uint8_t axis, int32_t error);
int32_t Api_SetNcRotaryTableOffset(uint8_t axis, int32_t offset);
int32_t Api_GetNcFeatureStatus(NC_FEATURE_STATUS* pOutStatus);
int32_t Api_GetPrestartInterlockStatus(PRESTART_INTERLOCK_STATUS* pOutStatus);
int32_t Api_GetNcAuxStatus(NC_AUX_STATUS* pOutStatus);
int32_t Api_GetNcCycleStatus(NC_CYCLE_STATUS* pOutStatus);
int32_t Api_GetIoTraceBuffer(IO_TRACE_BUFFER* pOutBuffer);
int32_t Api_SetNcToolLengthOffset(uint16_t offsetNo, int32_t offset);
int32_t Api_SetNcCutterRadiusOffset(uint16_t offsetNo, int32_t offset);
int32_t Api_GetNcCompensationStatus(NC_COMPENSATION_STATUS* pOutStatus);
int32_t Api_GetNcPathControlStatus(NC_PATH_CONTROL_STATUS* pOutStatus);
int32_t Api_SetNcAxisAssignment(uint8_t logicalAxis,
                                uint8_t physicalAxis,
                                int8_t sign,
                                uint8_t detached);
int32_t Api_SetNcMirrorMask(uint32_t axisMask);
int32_t Api_GetNcKinematicsStatus(NC_KINEMATICS_STATUS* pOutStatus);
int32_t Api_SetNcToolLifeLimit(uint16_t toolNo, uint32_t limit);
int32_t Api_GetNcAxisLoadStatus(NC_AXIS_LOAD_STATUS* pOutStatus);
int32_t Api_GetNcToolLifeStatus(NC_TOOL_LIFE_STATUS* pOutStatus);
int32_t Api_GetNcDiagnosticSnapshot(NC_DIAGNOSTIC_SNAPSHOT* pOutSnapshot);
int32_t Api_SetNcMotionFilterConfig(uint8_t enabled,
                                      uint8_t secondStageMode,
                                      uint8_t velocityWindow,
                                      uint8_t accelWindow);
int32_t Api_GetNcMotionFilterStatus(NC_MOTION_FILTER_STATUS* pOutStatus);
int32_t Api_SetNcMotionFilterAxisLimit(uint8_t axis,
                                       int32_t maxVelocityPerTick,
                                       int32_t maxAccelPerTick);
int32_t Api_GetNcEventRing(NC_EVENT_RING* pOutRing);
int32_t Api_GetNcCapabilityStatus(NC_CAPABILITY_STATUS* pOutStatus);
int32_t Api_SetNcInterferenceCheckEnabled(uint8_t enabled);
int32_t Api_GetNcInterferenceStatus(NC_INTERFERENCE_STATUS* pOutStatus);
int32_t Api_GetNcSafetyMotionStatus(NC_SAFETY_MOTION_STATUS* pOutStatus);
int32_t Api_GetNcTurningCycleStatus(NC_TURNING_CYCLE_STATUS* pOutStatus);
int32_t Api_GetNcThreadCycleStatus(NC_THREAD_CYCLE_STATUS* pOutStatus);
int32_t Api_SetNcDiameterMode(uint8_t enabled);
int32_t Api_GetNcReferenceStatus(NC_REFERENCE_STATUS* pOutStatus);
int32_t Api_SetNcReferenceAxisConfig(uint8_t axis, int32_t referencePosition, int8_t approachDirection, int32_t rolloverLimit, uint8_t distanceCodedMarker);
int32_t Api_SetNcOneDirectionApproach(uint8_t axis, int32_t approachAmount);
int32_t Api_GetNcPrecisionStatus(NC_PRECISION_STATUS* pOutStatus);
int32_t Api_SetNcLearningControl(uint8_t enabled, int32_t gainPercent, uint8_t memoryWindow);
int32_t Api_SetNcVibrationSuppression(uint8_t enabled, uint16_t notchFreqHz, uint16_t dampingPercent);
int32_t Api_SetNcPreviewControl(uint8_t enabled, uint16_t lookaheadBlocks, uint16_t cornerTolerance);
int32_t Api_GetNcSpindleStatus(NC_SPINDLE_STATUS* pOutStatus);
int32_t Api_SetNcSpindleSpeedLimit(uint32_t minRpm, uint32_t maxRpm);
int32_t Api_SetNcSpindleOrientWindow(uint16_t timeoutTicks);
int32_t Api_GetNcToolManagementStatus(NC_TOOL_MANAGEMENT_STATUS* pOutStatus);
int32_t Api_SetNcToolPocket(uint32_t toolNo, uint32_t pocketNo);
int32_t Api_RequestNcToolPrepare(uint32_t toolNo);
int32_t Api_GetNcSynchronizationStatus(NC_SYNCHRONIZATION_STATUS* pOutStatus);
int32_t Api_SetNcSynchronizationMasterSlave(uint8_t masterAxis, uint32_t slaveAxisMask);
int32_t Api_SetNcOverlayAxis(uint8_t axis, int32_t offset);
int32_t Api_SetNcCompoundPathMask(uint32_t pathMask);
int32_t Api_SetNcDoubleTableControl(uint8_t enabled, uint32_t slaveAxisMask);
int32_t Api_GetNcRotaryMccStatus(NC_ROTARY_MCC_STATUS* pOutStatus);
int32_t Api_SetNcRotaryAxisRadius(uint8_t axis, int32_t radius);
int32_t Api_SetNcMccOutput(uint8_t enabled);
int32_t Api_GetNcAxisConfigStatus(NC_AXIS_CONFIG_STATUS* pOutStatus);
int32_t Api_SetNcAxisDefinition(uint8_t axis, uint8_t axisName, uint8_t axisType, uint8_t coordinateGroup);
int32_t Api_SetNcPathAxisMask(uint32_t axisMask);
int32_t Api_SetNcAxisDetachedMask(uint32_t axisMask);
int32_t Api_SetNcAxisDiameterMode(uint8_t axis, uint8_t enabled);

#endif /* CONTROL_API_H */
