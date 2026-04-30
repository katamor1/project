/* ibm-bob/samples/base/source/inc/nc_coordinate_transform.h */
/* Declares work-coordinate extension helpers for fixture and mounting-error correction. */
/* This exists so TS-side coordinate corrections are fixed before RT block consumption. */
/* RELEVANT FILES: src/nc_coordinate_transform.c, src/nc_parser_modal.c, src/control_api.c */
#ifndef NC_COORDINATE_TRANSFORM_H
#define NC_COORDINATE_TRANSFORM_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC coordinate transform state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcCoordinateTransform_Reset(void);
/**
 * @brief Set enabled in NC coordinate transform.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param dynamicFixture Input value for dynamic fixture.
 * @param workMountError Input value for work mount error.
 * @param rotaryFixture Input value for rotary fixture.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetEnabled(uint8_t dynamicFixture,
                                         uint8_t workMountError,
                                         uint8_t rotaryFixture);
/**
 * @brief Set work offset in NC coordinate transform.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param workIndex Index selecting work.
 * @param axis Axis index to read or update.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetWorkOffset(uint8_t workIndex,
                                            uint8_t axis,
                                            int32_t offset);
/**
 * @brief Set local shift in NC coordinate transform.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param shift Input value for shift.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetLocalShift(uint8_t axis, int32_t shift);
/**
 * @brief Set temporary absolute in NC coordinate transform.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param programPosition Input value for program position.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetTemporaryAbsolute(uint8_t axis,
                                                   int32_t programPosition);
/**
 * @brief Set dynamic fixture offset in NC coordinate transform.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetDynamicFixtureOffset(uint8_t axis,
                                                      int32_t offset);
/**
 * @brief Set work mount error in NC coordinate transform.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param error Input value for error.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetWorkMountError(uint8_t axis, int32_t error);
/**
 * @brief Set rotary table offset in NC coordinate transform.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetRotaryTableOffset(uint8_t axis,
                                                   int32_t offset);
/**
 * @brief Apply NC coordinate transform TS-side settings to one parsed block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                           NC_ERROR_CODE* pOutError);

#endif /* NC_COORDINATE_TRANSFORM_H */
