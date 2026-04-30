/* ibm-bob/samples/base/source/inc/nc_rotary_mcc.h */
/* Declares rotary-axis virtual circumferential speed and MCC output helpers. */
/* This exists so rotary-axis feed-related state is prepared outside RT and MCC output stays fixed-cycle. */
#ifndef NC_ROTARY_MCC_H
#define NC_ROTARY_MCC_H

#include <stdint.h>
#include "system_shared.h"

void NcRotaryMcc_Reset(void);
int32_t NcRotaryMcc_SetAxisRadius(uint8_t axis, int32_t radius);
int32_t NcRotaryMcc_SetMccOutput(uint8_t enabled);
int32_t NcRotaryMcc_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError);
void NcRotaryMcc_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
void NcRotaryMcc_ServiceRt(void);

#endif /* NC_ROTARY_MCC_H */
