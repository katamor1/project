/* ibm-bob/samples/base/source/inc/nc_capability.h */
/* Tracks table-driven G/M/address coverage and parser usage diagnostics. */
#ifndef NC_CAPABILITY_H
#define NC_CAPABILITY_H

#include <stdint.h>
#include "system_shared.h"

#define NC_CAPABILITY_KIND_G       (1U)
#define NC_CAPABILITY_KIND_M       (2U)
#define NC_CAPABILITY_KIND_ADDRESS (3U)

void NcCapability_Reset(void);
void NcCapability_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock);
void NcCapability_RecordUnsupported(uint8_t kind, uint32_t code);

#endif /* NC_CAPABILITY_H */
