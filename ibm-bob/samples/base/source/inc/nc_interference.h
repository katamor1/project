/* ibm-bob/samples/base/source/inc/nc_interference.h */
/* Declares lightweight path/3D interference screening for TS-side block generation. */
/* This exists so risky blocks can be slowed or flagged before RT execution consumes them. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_interference.c, ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/src/nc_program.c */
#ifndef NC_INTERFERENCE_H
#define NC_INTERFERENCE_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC interference state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcInterference_Reset(void);

/**
 * @brief Set enabled in NC interference.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcInterference_SetEnabled(uint8_t enabled);

/**
 * @brief Check block TS against current input state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcInterference_CheckBlockTs(NC_EXEC_BLOCK* pBlock);

#endif /* NC_INTERFERENCE_H */
