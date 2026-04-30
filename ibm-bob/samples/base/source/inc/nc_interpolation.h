/* ibm-bob/samples/base/source/inc/nc_interpolation.h */
/* Declares NC interpolation helpers for TS preparation and RT stepping. */
/* This exists so NC blocks become fixed-period axis targets without RT parsing. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/src/nc_parser.c */
#ifndef NC_INTERPOLATION_H
#define NC_INTERPOLATION_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC interpolation state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcInterpolation_Reset(void);
/**
 * @brief Cancel active NC interpolation state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcInterpolation_Cancel(void);
/**
 * @brief Return whether NC interpolation currently has active.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @return Non-zero when true or active; zero when false or inactive.
 */
uint8_t NcInterpolation_HasActive(void);
/**
 * @brief Prepare NC interpolation state for one TS block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcInterpolation_PrepareBlockTs(NC_EXEC_BLOCK* pBlock,
                                       NC_ERROR_CODE* pOutError);
/**
 * @brief Start NC interpolation RT state for one block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcInterpolation_BeginBlockRt(const NC_EXEC_BLOCK* pBlock);
/**
 * @brief Advance NC interpolation by one RT interpolation step.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcInterpolation_StepRt(void);

#endif /* NC_INTERPOLATION_H */
