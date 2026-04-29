/* ibm-bob/samples/base/source/inc/nc_compensation.h */
/* Declares TS-side compensation helpers for tool length, cutter radius, rotation, and polar conversion. */
/* This exists so RT consumes already-compensated fixed-size NC blocks. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_compensation.c, ibm-bob/samples/base/source/src/nc_parser_modal.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_COMPENSATION_H
#define NC_COMPENSATION_H

#include <stdint.h>
#include "system_shared.h"

void NcCompensation_Reset(void);
int32_t NcCompensation_SetToolLengthOffset(uint16_t offsetNo, int32_t offset);
int32_t NcCompensation_SetCutterRadiusOffset(uint16_t offsetNo, int32_t offset);
int32_t NcCompensation_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                    NC_ERROR_CODE* pOutError);

#endif /* NC_COMPENSATION_H */
