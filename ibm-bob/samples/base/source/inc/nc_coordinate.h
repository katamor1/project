/* ibm-bob/samples/base/source/inc/nc_coordinate.h */
/* Declares TS-side coordinate state helpers for NC program parsing. */
/* This exists so parser code converts program coordinates before RT consumption. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_coordinate.c, ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_COORDINATE_H
#define NC_COORDINATE_H

#include <stdint.h>
#include "system_shared.h"

void NcCoordinate_Reset(void);
void NcCoordinate_SetUnitMode(NC_UNIT_MODE mode);
void NcCoordinate_SetDistanceMode(NC_DISTANCE_MODE mode);
void NcCoordinate_SelectWork(uint8_t workIndex);
void NcCoordinate_ClearLocalShift(void);
int32_t NcCoordinate_SetLocalShiftAxis(int32_t axisIndex, int32_t shift);
int32_t NcCoordinate_SetWorkOffsetAxis(uint8_t workIndex,
                                       int32_t axisIndex,
                                       int32_t offset);
int32_t NcCoordinate_SetProgramAxis(int32_t axisIndex,
                                    int32_t programPosition,
                                    NC_EXEC_BLOCK* pBlock);
int32_t NcCoordinate_SetMachineAxis(int32_t axisIndex,
                                    int32_t machinePosition,
                                    NC_EXEC_BLOCK* pBlock);
int32_t NcCoordinate_SetTemporaryAxis(int32_t axisIndex,
                                      int32_t programPosition);
void NcCoordinate_FillTargets(NC_EXEC_BLOCK* pBlock);

#endif /* NC_COORDINATE_H */
