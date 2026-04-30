/* ibm-bob/samples/base/source/inc/nc_lathe_cycle.h */
/* Tracks turning, grinding, and threading cycles as fixed-size TS/RT diagnostics. */
/* This exists so FANUC-style cycle design docs map to observable implementation state. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_lathe_cycle.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_LATHE_CYCLE_H
#define NC_LATHE_CYCLE_H

#include "system_shared.h"

void NcLatheCycle_Reset(void);
void NcLatheCycle_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock);
void NcLatheCycle_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
int32_t NcLatheCycle_SetDiameterMode(uint8_t enabled);

#endif /* NC_LATHE_CYCLE_H */
