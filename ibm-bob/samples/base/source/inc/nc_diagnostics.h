/* ibm-bob/samples/base/source/inc/nc_diagnostics.h */
/* Declares RT-safe diagnostic helpers for load monitoring and tool-life tracking. */
/* This exists so operations diagnostics stay separate from NC parsing and interpolation. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_diagnostics.c, ibm-bob/samples/base/source/src/rt_control.c, ibm-bob/samples/base/source/src/nc_program.c */
#ifndef NC_DIAGNOSTICS_H
#define NC_DIAGNOSTICS_H

#include <stdint.h>
#include "system_shared.h"

void NcDiagnostics_UpdateAxisLoadRt(void);
void NcDiagnostics_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
int32_t NcDiagnostics_SetToolLifeLimit(uint16_t toolNo, uint32_t limit);
void NcDiagnostics_BuildSnapshot(void);

#endif /* NC_DIAGNOSTICS_H */
