/* ibm-bob/samples/base/source/inc/nc_design_features.h */
#ifndef NC_DESIGN_FEATURES_H
#define NC_DESIGN_FEATURES_H
#include "system_shared.h"
void NcDesignFeatures_Reset(void);
int32_t NcDesignFeatures_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError);
void NcDesignFeatures_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
int32_t NcDesignFeatures_SetAutoCornerOverride(uint8_t enabled, uint16_t overridePercent);
#endif /* NC_DESIGN_FEATURES_H */
