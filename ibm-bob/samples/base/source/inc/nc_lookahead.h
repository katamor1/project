/* ibm-bob/samples/base/source/inc/nc_lookahead.h */
/* Declares TS-side lightweight lookahead and path control adjustment helpers. */
/* This exists so corner deceleration decisions are made before RT consumes blocks. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_lookahead.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_LOOKAHEAD_H
#define NC_LOOKAHEAD_H

#include "system_shared.h"

void NcLookahead_Reset(void);
void NcLookahead_ApplyBlockTs(NC_EXEC_BLOCK* pBlock);

#endif /* NC_LOOKAHEAD_H */
