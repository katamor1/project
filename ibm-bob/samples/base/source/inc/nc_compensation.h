/* ibm-bob/samples/base/source/inc/nc_compensation.h */
/* Declares TS-side compensation helpers for tool length, cutter radius, rotation, and polar conversion. */
/* This exists so RT consumes already-compensated fixed-size NC blocks. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_compensation.c, ibm-bob/samples/base/source/src/nc_parser_modal.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_COMPENSATION_H
#define NC_COMPENSATION_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC compensation state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcCompensation_Reset(void);
/**
 * @brief Set tool length offset in NC compensation.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param offsetNo Input value for offset no.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCompensation_SetToolLengthOffset(uint16_t offsetNo, int32_t offset);
/**
 * @brief Set cutter radius offset in NC compensation.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param offsetNo Input value for offset no.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCompensation_SetCutterRadiusOffset(uint16_t offsetNo, int32_t offset);
/**
 * @brief Apply NC compensation TS-side settings to one parsed block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCompensation_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                    NC_ERROR_CODE* pOutError);

#endif /* NC_COMPENSATION_H */
