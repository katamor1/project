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
int32_t Api_RequestNcProgramStart(void);
int32_t Api_RequestNcProgramHold(void);
int32_t Api_RequestNcProgramStop(void);
int32_t Api_RequestNcProgramResume(void);
int32_t Api_GetNcProgramStatus(NC_PROGRAM_STATUS* pOutStatus);
int32_t Api_GetNcInterpolationStatus(NC_INTERP_STATUS* pOutStatus);
int32_t Api_SetNcFeedOverride(uint16_t percent);
int32_t Api_GetNcFeedStatus(NC_FEED_STATUS* pOutStatus);
int32_t Api_GetNcCoordinateStatus(NC_COORDINATE_STATE* pOutStatus);

#endif /* CONTROL_API_H */
