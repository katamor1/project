/* ibm-bob/samples/base/source/inc/nc_diagnostics.h */
/* Declares RT-safe diagnostic helpers for load monitoring and tool-life tracking. */
/* This exists so operations diagnostics stay separate from NC parsing and interpolation. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_diagnostics.c, ibm-bob/samples/base/source/src/rt_control.c, ibm-bob/samples/base/source/src/nc_program.c */
#ifndef NC_DIAGNOSTICS_H
#define NC_DIAGNOSTICS_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Update axis load RT from current inputs.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcDiagnostics_UpdateAxisLoadRt(void);
/**
 * @brief Record NC diagnostics data from one executing RT block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcDiagnostics_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
/**
 * @brief Set tool life limit in NC diagnostics.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param toolNo Input value for tool no.
 * @param limit Input value for limit.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcDiagnostics_SetToolLifeLimit(uint16_t toolNo, uint32_t limit);
/**
 * @brief Build snapshot from current shared state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcDiagnostics_BuildSnapshot(void);

#endif /* NC_DIAGNOSTICS_H */
