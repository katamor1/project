/* ibm-bob/samples/base/source/inc/nc_axis_config.h */
/* Declares TS-side axis setting, path-axis allocation, detached-axis and diameter/radius command helpers. */
/* This exists so flexible axis setup is converted into fixed NC execution blocks before RT execution. */
/* RELEVANT FILES: src/nc_axis_config.c, src/nc_parser_modal.c, inc/control_api.h, inc/system_shared.h */
#ifndef NC_AXIS_CONFIG_H
#define NC_AXIS_CONFIG_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Set axis definition in NC axis config.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param axisName Input value for axis name.
 * @param axisType Input value for axis type.
 * @param coordinateGroup Input value for coordinate group.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcAxisConfig_SetAxisDefinition(uint8_t axis,
                                       uint8_t axisName,
                                       uint8_t axisType,
                                       uint8_t coordinateGroup);
/**
 * @brief Set path axis mask in NC axis config.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axisMask Bit mask of axes affected by the request.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcAxisConfig_SetPathAxisMask(uint32_t axisMask);
/**
 * @brief Set detached mask in NC axis config.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axisMask Bit mask of axes affected by the request.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcAxisConfig_SetDetachedMask(uint32_t axisMask);
/**
 * @brief Set diameter mode in NC axis config.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcAxisConfig_SetDiameterMode(uint8_t axis, uint8_t enabled);
/**
 * @brief Apply NC axis config TS-side settings to one parsed block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcAxisConfig_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                  NC_ERROR_CODE* pOutError);

#endif /* NC_AXIS_CONFIG_H */
