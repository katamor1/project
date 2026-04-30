/* ibm-bob/samples/base/source/inc/nc_lookahead.h */
/* Declares TS-side lightweight lookahead and path control adjustment helpers. */
/* This exists so corner deceleration decisions are made before RT consumes blocks. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_lookahead.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_LOOKAHEAD_H
#define NC_LOOKAHEAD_H

#include "system_shared.h"

/**
 * @brief Reset NC lookahead state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcLookahead_Reset(void);
/**
 * @brief Apply NC lookahead TS-side settings to one parsed block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcLookahead_ApplyBlockTs(NC_EXEC_BLOCK* pBlock);

#endif /* NC_LOOKAHEAD_H */
