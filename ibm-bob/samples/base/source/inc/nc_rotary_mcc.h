/* ibm-bob/samples/base/source/inc/nc_rotary_mcc.h */
/* Declares rotary-axis virtual circumferential speed and MCC output helpers. */
/* This exists so rotary-axis feed-related state is prepared outside RT and MCC output stays fixed-cycle. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_rotary_mcc.c, ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/src/rt_control.c */
#ifndef NC_ROTARY_MCC_H
#define NC_ROTARY_MCC_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC rotary MCC state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcRotaryMcc_Reset(void);

/**
 * @brief Set axis radius in NC rotary MCC.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param radius Input value for radius.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcRotaryMcc_SetAxisRadius(uint8_t axis, int32_t radius);

/**
 * @brief Set MCC output in NC rotary MCC.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcRotaryMcc_SetMccOutput(uint8_t enabled);

/**
 * @brief Apply NC rotary MCC TS-side settings to one parsed block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcRotaryMcc_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError);

/**
 * @brief Record NC rotary MCC data from one executing RT block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcRotaryMcc_OnBlockRt(const NC_EXEC_BLOCK* pBlock);

/**
 * @brief Advance NC rotary MCC service work during the RT cycle.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcRotaryMcc_ServiceRt(void);

#endif /* NC_ROTARY_MCC_H */
