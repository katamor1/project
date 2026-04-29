/* ibm-bob/samples/base/source/inc/nc_interpolation.h */
/* Declares NC interpolation helpers for TS preparation and RT stepping. */
/* This exists so NC blocks become fixed-period axis targets without RT parsing. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/src/nc_parser.c */
#ifndef NC_INTERPOLATION_H
#define NC_INTERPOLATION_H

#include <stdint.h>
#include "system_shared.h"

void NcInterpolation_Reset(void);
void NcInterpolation_Cancel(void);
uint8_t NcInterpolation_HasActive(void);
int32_t NcInterpolation_PrepareBlockTs(NC_EXEC_BLOCK* pBlock,
                                       NC_ERROR_CODE* pOutError);
int32_t NcInterpolation_BeginBlockRt(const NC_EXEC_BLOCK* pBlock);
int32_t NcInterpolation_StepRt(void);

#endif /* NC_INTERPOLATION_H */
