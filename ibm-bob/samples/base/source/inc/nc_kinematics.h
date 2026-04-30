/* ibm-bob/samples/base/source/inc/nc_kinematics.h */
/* Declares TS-side 5-axis, mirror, axis allocation, and retract helpers. */
/* This exists so RT receives fixed, already-transformed NC execution blocks. */
/* RELEVANT FILES: src/nc_kinematics.c, src/nc_parser_modal.c, src/nc_program.c */
#ifndef NC_KINEMATICS_H
#define NC_KINEMATICS_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC kinematics state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcKinematics_Reset(void);
/**
 * @brief Set axis assignment in NC kinematics.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param logicalAxis Input value for logical axis.
 * @param physicalAxis Input value for physical axis.
 * @param sign Input value for sign.
 * @param detached Input value for detached.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcKinematics_SetAxisAssignment(uint8_t logicalAxis,
                                       uint8_t physicalAxis,
                                       int8_t sign,
                                       uint8_t detached);
/**
 * @brief Set mirror mask in NC kinematics.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axisMask Bit mask of axes affected by the request.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcKinematics_SetMirrorMask(uint32_t axisMask);
/**
 * @brief Apply NC kinematics TS-side settings to one parsed block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcKinematics_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                  NC_ERROR_CODE* pOutError);

#endif /* NC_KINEMATICS_H */
