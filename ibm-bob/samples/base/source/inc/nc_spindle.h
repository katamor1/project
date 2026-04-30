/* ibm-bob/samples/base/source/inc/nc_spindle.h */
/* Declares RT-safe spindle command monitoring and constant surface speed control. */
/* This exists so spindle design-doc items map to fixed shared-memory status and public APIs. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_spindle.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_SPINDLE_H
#define NC_SPINDLE_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC spindle state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcSpindle_Reset(void);
/**
 * @brief Record NC spindle data from one parsed TS block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcSpindle_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock);
/**
 * @brief Record NC spindle data from one executing RT block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcSpindle_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
/**
 * @brief Advance NC spindle service work during the RT cycle.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcSpindle_ServiceRt(void);
/**
 * @brief Set speed limit in NC spindle.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param minRpm Input value for min RPM.
 * @param maxRpm Input value for max RPM.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcSpindle_SetSpeedLimit(uint32_t minRpm, uint32_t maxRpm);
/**
 * @brief Set orient window in NC spindle.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param timeoutTicks Input value for timeout ticks.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcSpindle_SetOrientWindow(uint16_t timeoutTicks);

#endif /* NC_SPINDLE_H */
