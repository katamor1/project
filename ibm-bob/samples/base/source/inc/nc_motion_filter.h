/* ibm-bob/samples/base/source/inc/nc_motion_filter.h */
/* Declares RT-safe smoothing for interpolated motion-controller axis commands. */
/* This exists so velocity and acceleration smoothing stay fixed-size and inspectable. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_motion_filter.c, ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_MOTION_FILTER_H
#define NC_MOTION_FILTER_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Reset NC motion filter RT state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcMotionFilter_ResetRt(void);
/**
 * @brief Set config RT in NC motion filter.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param enabled Non-zero to enable the feature, zero to disable it.
 * @param secondStageMode Input value for second stage mode.
 * @param velocityWindow Input value for velocity window.
 * @param accelWindow Input value for accel window.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcMotionFilter_SetConfigRt(uint8_t enabled,
                                   uint8_t secondStageMode,
                                   uint8_t velocityWindow,
                                   uint8_t accelWindow);
/**
 * @brief Apply RT to the current block or state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axisTarget Input value for axis target.
 * @param axisMask Bit mask of axes affected by the request.
 * @param forceEndpoint Input value for force endpoint.
 * @param remainingTicks Input value for remaining ticks.
 */
void NcMotionFilter_ApplyRt(volatile int32_t axisTarget[AXIS_MAX],
                            uint32_t axisMask,
                            uint8_t forceEndpoint,
                            uint32_t remainingTicks);
/**
 * @brief Set axis limit RT in NC motion filter.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param axis Axis index to read or update.
 * @param maxVelocityPerTick Input value for max velocity per tick.
 * @param maxAccelPerTick Input value for max accel per tick.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcMotionFilter_SetAxisLimitRt(uint8_t axis,
                                      int32_t maxVelocityPerTick,
                                      int32_t maxAccelPerTick);

#endif /* NC_MOTION_FILTER_H */
