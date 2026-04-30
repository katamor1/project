/* ibm-bob/samples/base/source/inc/nc_feed.h */
/* Declares feed override, dwell, and simple profile helpers for NC motion. */
/* This exists so interpolation can use feed behavior without owning feed state. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_feed.c, ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_FEED_H
#define NC_FEED_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC feed state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcFeed_Reset(void);
/**
 * @brief Cancel active NC feed state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcFeed_CancelRt(void);
/**
 * @brief Set override in NC feed.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param percent Override or percentage value to apply.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcFeed_SetOverride(uint16_t percent);
/**
 * @brief Prepare motion TS for later execution.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param pathLengthUnits Input value for path length units.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcFeed_PrepareMotionTs(NC_EXEC_BLOCK* pBlock,
                               double pathLengthUnits,
                               NC_ERROR_CODE* pOutError);
/**
 * @brief Prepare dwell TS for later execution.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param dwellTicks Input value for dwell ticks.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcFeed_PrepareDwellTs(NC_EXEC_BLOCK* pBlock,
                              uint32_t dwellTicks,
                              NC_ERROR_CODE* pOutError);
/**
 * @brief Start NC feed RT state for one block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcFeed_BeginBlockRt(const NC_EXEC_BLOCK* pBlock);
/**
 * @brief Update RT from current inputs.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param tick Input value for tick.
 * @param done Input value for done.
 */
void NcFeed_UpdateRt(const NC_EXEC_BLOCK* pBlock, uint32_t tick, uint8_t done);
/**
 * @brief Handle NC feed profile ratio RT for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param tick Input value for tick.
 * @return Computed floating-point result for the current operation.
 */
double NcFeed_ProfileRatioRt(const NC_EXEC_BLOCK* pBlock, uint32_t tick);

#endif /* NC_FEED_H */
