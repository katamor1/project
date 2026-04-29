/* ibm-bob/samples/base/source/inc/nc_kinematics.h */
/* Declares TS-side 5-axis, mirror, axis allocation, and retract helpers. */
/* This exists so RT receives fixed, already-transformed NC execution blocks. */
/* RELEVANT FILES: src/nc_kinematics.c, src/nc_parser_modal.c, src/nc_program.c */
#ifndef NC_KINEMATICS_H
#define NC_KINEMATICS_H

#include <stdint.h>
#include "system_shared.h"

void NcKinematics_Reset(void);
int32_t NcKinematics_SetAxisAssignment(uint8_t logicalAxis,
                                       uint8_t physicalAxis,
                                       int8_t sign,
                                       uint8_t detached);
int32_t NcKinematics_SetMirrorMask(uint32_t axisMask);
int32_t NcKinematics_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                  NC_ERROR_CODE* pOutError);

#endif /* NC_KINEMATICS_H */
