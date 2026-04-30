/* ibm-bob/samples/base/source/inc/nc_precision.h */
/* Declares high-precision learning, vibration, and preview-control helpers. */
/* This exists so advanced contour-control design items have fixed shared state. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_precision.c, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/src/rt_control.c */
#ifndef NC_PRECISION_H
#define NC_PRECISION_H

#include <stdint.h>
#include "system_shared.h"

void NcPrecision_Reset(void);
void NcPrecision_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock);
void NcPrecision_OnBlockRt(const NC_EXEC_BLOCK* pBlock);
void NcPrecision_ServiceRt(void);
int32_t NcPrecision_SetLearningControl(uint8_t enabled,
                                       int32_t gainPercent,
                                       uint8_t memoryWindow);
int32_t NcPrecision_SetVibrationControl(uint8_t enabled,
                                        uint16_t notchFreqHz,
                                        uint16_t dampingPercent);
int32_t NcPrecision_SetPreviewControl(uint8_t enabled,
                                      uint16_t lookaheadBlocks,
                                      uint16_t cornerTolerance);

#endif /* NC_PRECISION_H */
