/* ibm-bob/samples/base/source/inc/nc_reference.h */
/* Tracks reference return, one-direction positioning, reference marks, and rollover state. */
/* This exists so reference-related design docs map to deterministic RT/TS state. */
/* RELEVANT FILES: src/nc_reference.c, src/nc_program.c, inc/system_shared.h */
#ifndef NC_REFERENCE_H
#define NC_REFERENCE_H

#include <stdint.h>
#include "system_shared.h"

void NcReference_Reset(void);
void NcReference_ServiceRt(void);
void NcReference_OnParsedBlockTs(NC_EXEC_BLOCK* pBlock);
void NcReference_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
int32_t NcReference_SetAxisConfig(uint8_t axis,
                                  int32_t referencePosition,
                                  int8_t approachDirection,
                                  int32_t rolloverLimit,
                                  uint8_t distanceCodedMarker);
int32_t NcReference_SetOneDirectionApproach(uint8_t axis,
                                            int32_t approachAmount);

#endif /* NC_REFERENCE_H */
