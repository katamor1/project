/* ibm-bob/samples/base/source/inc/nc_safety_motion.h */
/* Declares RT-safe safety motion overlays for external decel, power loss, and position switch latch. */
/* This exists so safety sensors can adjust motion without adding platform calls to RT control. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_safety_motion.c, ibm-bob/samples/base/source/src/rt_control.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_SAFETY_MOTION_H
#define NC_SAFETY_MOTION_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC safety motion state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcSafetyMotion_Reset(void);

/**
 * @brief Update RT from current inputs.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcSafetyMotion_UpdateRt(void);

/**
 * @brief Apply NC safety motion RT-side settings to one executing block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcSafetyMotion_ApplyBlockRt(NC_EXEC_BLOCK* pBlock);

#endif /* NC_SAFETY_MOTION_H */
