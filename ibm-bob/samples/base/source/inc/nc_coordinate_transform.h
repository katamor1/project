/* ibm-bob/samples/base/source/inc/nc_coordinate_transform.h */
/* Declares work-coordinate extension helpers for fixture and mounting-error correction. */
/* This exists so TS-side coordinate corrections are fixed before RT block consumption. */
/* RELEVANT FILES: src/nc_coordinate_transform.c, src/nc_parser_modal.c, src/control_api.c */
#ifndef NC_COORDINATE_TRANSFORM_H
#define NC_COORDINATE_TRANSFORM_H

#include <stdint.h>
#include "system_shared.h"

void NcCoordinateTransform_Reset(void);
int32_t NcCoordinateTransform_SetEnabled(uint8_t dynamicFixture,
                                         uint8_t workMountError,
                                         uint8_t rotaryFixture);
int32_t NcCoordinateTransform_SetWorkOffset(uint8_t workIndex,
                                            uint8_t axis,
                                            int32_t offset);
int32_t NcCoordinateTransform_SetLocalShift(uint8_t axis, int32_t shift);
int32_t NcCoordinateTransform_SetTemporaryAbsolute(uint8_t axis,
                                                   int32_t programPosition);
int32_t NcCoordinateTransform_SetDynamicFixtureOffset(uint8_t axis,
                                                      int32_t offset);
int32_t NcCoordinateTransform_SetWorkMountError(uint8_t axis, int32_t error);
int32_t NcCoordinateTransform_SetRotaryTableOffset(uint8_t axis,
                                                   int32_t offset);
int32_t NcCoordinateTransform_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                           NC_ERROR_CODE* pOutError);

#endif /* NC_COORDINATE_TRANSFORM_H */
