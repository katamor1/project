/* ibm-bob/samples/base/source/inc/nc_reference.h */
/* Tracks reference return, one-direction positioning, reference marks, and rollover state. */
/* This exists so reference-related design docs map to deterministic RT/TS state. */
/* RELEVANT FILES: src/nc_reference.c, src/nc_program.c, inc/system_shared.h */
#ifndef NC_REFERENCE_H
#define NC_REFERENCE_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC reference state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcReference_Reset(void);
/**
 * @brief Advance NC reference service work during the RT cycle.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcReference_ServiceRt(void);
/**
 * @brief Record NC reference data from one parsed TS block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcReference_OnParsedBlockTs(NC_EXEC_BLOCK* pBlock);
/**
 * @brief Record NC reference data from one executing RT block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcReference_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
/**
 * @brief Set axis config in NC reference.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param referencePosition Input value for reference position.
 * @param approachDirection Input value for approach direction.
 * @param rolloverLimit Input value for rollover limit.
 * @param distanceCodedMarker Input value for distance coded marker.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcReference_SetAxisConfig(uint8_t axis,
                                  int32_t referencePosition,
                                  int8_t approachDirection,
                                  int32_t rolloverLimit,
                                  uint8_t distanceCodedMarker);
/**
 * @brief Set one direction approach in NC reference.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param approachAmount Input value for approach amount.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcReference_SetOneDirectionApproach(uint8_t axis,
                                            int32_t approachAmount);

#endif /* NC_REFERENCE_H */
