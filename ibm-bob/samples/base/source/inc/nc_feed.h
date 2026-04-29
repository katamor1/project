/* ibm-bob/samples/base/source/inc/nc_feed.h */
/* Declares feed override, dwell, and simple profile helpers for NC motion. */
/* This exists so interpolation can use feed behavior without owning feed state. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_feed.c, ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_FEED_H
#define NC_FEED_H

#include <stdint.h>
#include "system_shared.h"

void NcFeed_Reset(void);
void NcFeed_CancelRt(void);
int32_t NcFeed_SetOverride(uint16_t percent);
int32_t NcFeed_PrepareMotionTs(NC_EXEC_BLOCK* pBlock,
                               double pathLengthUnits,
                               NC_ERROR_CODE* pOutError);
int32_t NcFeed_PrepareDwellTs(NC_EXEC_BLOCK* pBlock,
                              uint32_t dwellTicks,
                              NC_ERROR_CODE* pOutError);
int32_t NcFeed_BeginBlockRt(const NC_EXEC_BLOCK* pBlock);
void NcFeed_UpdateRt(const NC_EXEC_BLOCK* pBlock, uint32_t tick, uint8_t done);
double NcFeed_ProfileRatioRt(const NC_EXEC_BLOCK* pBlock, uint32_t tick);

#endif /* NC_FEED_H */
