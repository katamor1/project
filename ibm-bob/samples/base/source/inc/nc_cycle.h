/* ibm-bob/samples/base/source/inc/nc_cycle.h */
/* Declares fixed canned-cycle parser and RT segment planning helpers. */
/* This exists so fixed-cycle details stay out of interpolation and modal parsing. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_cycle.c, ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/src/nc_parser_modal.c */
#ifndef NC_CYCLE_H
#define NC_CYCLE_H

#include <stdint.h>
#include "system_shared.h"

typedef struct {
    int32_t  start[AXIS_MAX];
    int32_t  end[AXIS_MAX];
    uint32_t end_tick;
    uint8_t  kind;
} NC_ACTIVE_SEGMENT;

#define NC_SEG_KIND_LINEAR       (0U)
#define NC_SEG_KIND_RAPID        (1U)
#define NC_SEG_KIND_DWELL        (2U)
#define NC_SEG_KIND_RETRACT      (3U)

void NcCycle_ResetParserModal(void);
void NcCycle_InitParserBlock(NC_EXEC_BLOCK* pBlock);
void NcCycle_ApplyCycleGCode(int32_t code, NC_EXEC_BLOCK* pBlock);
void NcCycle_CancelParserModal(void);
int32_t NcCycle_SetRepeatWord(double value, NC_EXEC_BLOCK* pBlock);
int32_t NcCycle_FinalizeParserBlock(NC_EXEC_BLOCK* pBlock,
                                    NC_ERROR_CODE* pOutError);
int32_t NcCycle_BuildSegmentPlanRt(const NC_EXEC_BLOCK* pBlock,
                                   NC_ACTIVE_SEGMENT* pSegments,
                                   uint32_t* pSegmentCount);

#endif /* NC_CYCLE_H */
