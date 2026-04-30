/* ibm-bob/samples/base/source/inc/nc_precision.h */
/* Declares high-precision learning, vibration, and preview-control helpers. */
/* This exists so advanced contour-control design items have fixed shared state. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_precision.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/src/rt_control.c */
#ifndef NC_PRECISION_H
#define NC_PRECISION_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC precision state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcPrecision_Reset(void);
/**
 * @brief Record NC precision data from one parsed TS block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcPrecision_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock);
/**
 * @brief Record NC precision data from one executing RT block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcPrecision_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
/**
 * @brief Advance NC precision service work during the RT cycle.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcPrecision_ServiceRt(void);
/**
 * @brief Set learning control in NC precision.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param gainPercent Input value for gain percent.
 * @param memoryWindow Input value for memory window.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcPrecision_SetLearningControl(uint8_t enabled,
                                       int32_t gainPercent,
                                       uint8_t memoryWindow);
/**
 * @brief Set vibration control in NC precision.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param notchFreqHz Input value for notch freq hz.
 * @param dampingPercent Input value for damping percent.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcPrecision_SetVibrationControl(uint8_t enabled,
                                        uint16_t notchFreqHz,
                                        uint16_t dampingPercent);
/**
 * @brief Set preview control in NC precision.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param lookaheadBlocks Input value for lookahead blocks.
 * @param cornerTolerance Input value for corner tolerance.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcPrecision_SetPreviewControl(uint8_t enabled,
                                      uint16_t lookaheadBlocks,
                                      uint16_t cornerTolerance);

#endif /* NC_PRECISION_H */
