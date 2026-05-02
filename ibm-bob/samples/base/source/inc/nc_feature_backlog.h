/* ibm-bob/samples/base/source/inc/nc_feature_backlog.h */
#ifndef NC_FEATURE_BACKLOG_H
#define NC_FEATURE_BACKLOG_H
#include "system_shared.h"
void NcFeatureBacklog_Reset(void);
int32_t NcFeatureBacklog_SetFeature(uint16_t featureId, uint8_t enabled, uint16_t parameter);
int32_t NcFeatureBacklog_EnableAll(uint16_t parameter);
int32_t NcFeatureBacklog_RunSelfCheck(void);
int32_t NcFeatureBacklog_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError);
void NcFeatureBacklog_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
#endif /* NC_FEATURE_BACKLOG_H */
