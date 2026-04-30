/* ibm-bob/samples/base/source/inc/nc_synchronization.h */
/* Declares fixed-size synchronization, compound, overlay, and double-table status handling. */
/* This exists so G51.4/G50.4, G51.5/G50.5, and G51.6/G50.6 design items map to RT-safe shared memory. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_synchronization.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_SYNCHRONIZATION_H
#define NC_SYNCHRONIZATION_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC synchronization state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcSynchronization_Reset(void);
/**
 * @brief Record NC synchronization data from one parsed TS block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcSynchronization_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock);
/**
 * @brief Apply NC synchronization RT-side settings to one executing block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcSynchronization_ApplyBlockRt(NC_EXEC_BLOCK* pBlock);
/**
 * @brief Set master slave in NC synchronization.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param masterAxis Input value for master axis.
 * @param slaveAxisMask Bit mask for slave axis.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcSynchronization_SetMasterSlave(uint8_t masterAxis, uint32_t slaveAxisMask);
/**
 * @brief Set overlay axis in NC synchronization.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcSynchronization_SetOverlayAxis(uint8_t axis, int32_t offset);
/**
 * @brief Set compound path mask in NC synchronization.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pathMask Bit mask for path.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcSynchronization_SetCompoundPathMask(uint32_t pathMask);
/**
 * @brief Set double table in NC synchronization.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param slaveAxisMask Bit mask for slave axis.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcSynchronization_SetDoubleTable(uint8_t enabled, uint32_t slaveAxisMask);

#endif /* NC_SYNCHRONIZATION_H */
