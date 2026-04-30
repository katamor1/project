/* ibm-bob/samples/base/source/inc/nc_synchronization.h */
/* Declares fixed-size synchronization, compound, overlay, and double-table status handling. */
/* This exists so G51.4/G50.4, G51.5/G50.5, and G51.6/G50.6 design items map to RT-safe shared memory. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_synchronization.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_SYNCHRONIZATION_H
#define NC_SYNCHRONIZATION_H

#include <stdint.h>
#include "system_shared.h"

void NcSynchronization_Reset(void);
void NcSynchronization_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock);
void NcSynchronization_ApplyBlockRt(NC_EXEC_BLOCK* pBlock);
int32_t NcSynchronization_SetMasterSlave(uint8_t masterAxis, uint32_t slaveAxisMask);
int32_t NcSynchronization_SetOverlayAxis(uint8_t axis, int32_t offset);
int32_t NcSynchronization_SetCompoundPathMask(uint32_t pathMask);
int32_t NcSynchronization_SetDoubleTable(uint8_t enabled, uint32_t slaveAxisMask);

#endif /* NC_SYNCHRONIZATION_H */
