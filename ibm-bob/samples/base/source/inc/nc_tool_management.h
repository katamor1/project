/* ibm-bob/samples/base/source/inc/nc_tool_management.h */
/* Declares RT-safe tool table, preparation, and exchange status helpers. */
/* This exists so T words and M06 are tracked separately from generic auxiliary M-code waiting. */
#ifndef NC_TOOL_MANAGEMENT_H
#define NC_TOOL_MANAGEMENT_H

#include <stdint.h>
#include "system_shared.h"

void NcToolManagement_Reset(void);
int32_t NcToolManagement_SetPocket(uint32_t toolNo, uint32_t pocketNo);
int32_t NcToolManagement_RequestPrepare(uint32_t toolNo);
void NcToolManagement_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock);
void NcToolManagement_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
void NcToolManagement_ServiceRt(void);

#endif /* NC_TOOL_MANAGEMENT_H */
