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

#endif /* CONTROL_API_H */
