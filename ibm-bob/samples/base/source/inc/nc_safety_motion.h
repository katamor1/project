/* ibm-bob/samples/base/source/inc/nc_safety_motion.h */
/* Declares RT-safe safety motion overlays for external decel, power loss, and position switch latch. */
#ifndef NC_SAFETY_MOTION_H
#define NC_SAFETY_MOTION_H

#include <stdint.h>
#include "system_shared.h"

void NcSafetyMotion_Reset(void);
void NcSafetyMotion_UpdateRt(void);
void NcSafetyMotion_ApplyBlockRt(NC_EXEC_BLOCK* pBlock);

#endif /* NC_SAFETY_MOTION_H */
