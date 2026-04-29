/* ibm-bob/samples/base/source/inc/nc_motion_filter.h */
/* Declares RT-safe smoothing for interpolated motion-controller axis commands. */
/* This exists so velocity and acceleration smoothing stay fixed-size and inspectable. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_motion_filter.c, ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_MOTION_FILTER_H
#define NC_MOTION_FILTER_H

#include <stdint.h>
#include "system_shared.h"

void NcMotionFilter_ResetRt(void);
int32_t NcMotionFilter_SetConfigRt(uint8_t enabled,
                                   uint8_t secondStageMode,
                                   uint8_t velocityWindow,
                                   uint8_t accelWindow);
void NcMotionFilter_ApplyRt(volatile int32_t axisTarget[AXIS_MAX],
                            uint32_t axisMask,
                            uint8_t forceEndpoint,
                            uint32_t remainingTicks);
int32_t NcMotionFilter_SetAxisLimitRt(uint8_t axis,
                                      int32_t maxVelocityPerTick,
                                      int32_t maxAccelPerTick);

#endif /* NC_MOTION_FILTER_H */
