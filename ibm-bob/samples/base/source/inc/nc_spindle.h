/* ibm-bob/samples/base/source/inc/nc_spindle.h */
/* Declares RT-safe spindle command monitoring and constant surface speed control. */
/* This exists so spindle design-doc items map to fixed shared-memory status and public APIs. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_spindle.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_SPINDLE_H
#define NC_SPINDLE_H

#include <stdint.h>
#include "system_shared.h"

void NcSpindle_Reset(void);
void NcSpindle_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock);
void NcSpindle_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
void NcSpindle_ServiceRt(void);
int32_t NcSpindle_SetSpeedLimit(uint32_t minRpm, uint32_t maxRpm);
int32_t NcSpindle_SetOrientWindow(uint16_t timeoutTicks);

#endif /* NC_SPINDLE_H */
