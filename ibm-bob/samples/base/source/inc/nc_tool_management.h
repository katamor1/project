/* ibm-bob/samples/base/source/inc/nc_tool_management.h */
/* Declares RT-safe tool table, preparation, and exchange status helpers. */
/* This exists so T words and M06 are tracked separately from generic auxiliary M-code waiting. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_tool_management.c, ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/src/nc_program.c */
#ifndef NC_TOOL_MANAGEMENT_H
#define NC_TOOL_MANAGEMENT_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC tool management state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcToolManagement_Reset(void);

/**
 * @brief Set pocket in NC tool management.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param toolNo Input value for tool no.
 * @param pocketNo Input value for pocket no.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcToolManagement_SetPocket(uint32_t toolNo, uint32_t pocketNo);

/**
 * @brief Handle NC tool management request prepare for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param toolNo Input value for tool no.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcToolManagement_RequestPrepare(uint32_t toolNo);

/**
 * @brief Record NC tool management data from one parsed TS block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcToolManagement_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock);

/**
 * @brief Record NC tool management data from one executing RT block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcToolManagement_OnBlockRt(const NC_EXEC_BLOCK* pBlock);

/**
 * @brief Advance NC tool management service work during the RT cycle.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcToolManagement_ServiceRt(void);

#endif /* NC_TOOL_MANAGEMENT_H */
