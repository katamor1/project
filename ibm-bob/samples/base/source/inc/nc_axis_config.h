/* ibm-bob/samples/base/source/inc/nc_axis_config.h */
/* Declares TS-side axis setting, path-axis allocation, detached-axis and diameter/radius command helpers. */
/* This exists so flexible axis setup is converted into fixed NC execution blocks before RT execution. */
/* RELEVANT FILES: src/nc_axis_config.c, src/nc_parser_modal.c, inc/control_api.h, inc/system_shared.h */
#ifndef NC_AXIS_CONFIG_H
#define NC_AXIS_CONFIG_H

#include <stdint.h>
#include "system_shared.h"

int32_t NcAxisConfig_SetAxisDefinition(uint8_t axis,
                                       uint8_t axisName,
                                       uint8_t axisType,
                                       uint8_t coordinateGroup);
int32_t NcAxisConfig_SetPathAxisMask(uint32_t axisMask);
int32_t NcAxisConfig_SetDetachedMask(uint32_t axisMask);
int32_t NcAxisConfig_SetDiameterMode(uint8_t axis, uint8_t enabled);
int32_t NcAxisConfig_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                  NC_ERROR_CODE* pOutError);

#endif /* NC_AXIS_CONFIG_H */
