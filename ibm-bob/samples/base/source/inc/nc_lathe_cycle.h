/* ibm-bob/samples/base/source/inc/nc_lathe_cycle.h */
/* Tracks turning, grinding, and threading cycles as fixed-size TS/RT diagnostics. */
/* This exists so FANUC-style cycle design docs map to observable implementation state. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_lathe_cycle.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_LATHE_CYCLE_H
#define NC_LATHE_CYCLE_H

#include "system_shared.h"

/**
 * @brief Reset NC lathe cycle state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcLatheCycle_Reset(void);
/**
 * @brief Record NC lathe cycle data from one parsed TS block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcLatheCycle_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock);
/**
 * @brief Record NC lathe cycle data from one executing RT block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcLatheCycle_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
/**
 * @brief Set diameter mode in NC lathe cycle.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcLatheCycle_SetDiameterMode(uint8_t enabled);

#endif /* NC_LATHE_CYCLE_H */
