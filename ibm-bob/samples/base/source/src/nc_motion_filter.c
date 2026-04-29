/* ibm-bob/samples/base/source/src/nc_motion_filter.c */
/* Implements RT-safe two-stage smoothing and axis-limit guarding for motion commands. */
/* This exists so interpolated position targets are smoothed before motion-controller output. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_motion_filter.h, ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/src/control_api.c */
#include <string.h>
#include "nc_motion_filter.h"

static uint8_t s_initialized[AXIS_MAX];
static int32_t s_lastRawTarget[AXIS_MAX];
static int32_t s_filteredTarget[AXIS_MAX];
static int32_t s_lastOutputVelocity[AXIS_MAX];
static int32_t s_velocityRing[AXIS_MAX][NC_MOTION_FILTER_VELOCITY_WINDOW_MAX];
static int32_t s_accelRing[AXIS_MAX][NC_MOTION_FILTER_ACCEL_WINDOW_MAX];
static int32_t s_firHistory[AXIS_MAX][NC_MOTION_FILTER_FIR_TAPS];
static int64_t s_velocitySum[AXIS_MAX];
static int64_t s_accelSum[AXIS_MAX];
static uint8_t s_velocityIndex[AXIS_MAX];
static uint8_t s_accelIndex[AXIS_MAX];
static uint8_t s_firIndex[AXIS_MAX];
static uint8_t s_velocityCount[AXIS_MAX];
static uint8_t s_accelCount[AXIS_MAX];
static uint8_t s_firCount[AXIS_MAX];

static int32_t Abs32Local(int32_t value)
{
    return (value < 0) ? -value : value;
}

static uint8_t ClampWindow(uint8_t value, uint8_t maxValue)
{
    if (value == 0U) {
        return 1U;
    }
    return (value > maxValue) ? maxValue : value;
}

static int32_t NormalizeLimit(int32_t value, int32_t fallback)
{
    if (value < 0) {
        value = -value;
    }
    if (value == 0) {
        value = fallback;
    }
    if (value > NC_MOTION_FILTER_AXIS_LIMIT_MAX) {
        value = NC_MOTION_FILTER_AXIS_LIMIT_MAX;
    }
    return value;
}

static int32_t DivRound64(int64_t value, int64_t denom)
{
    if (denom <= 0) {
        return 0;
    }
    if (value >= 0) {
        return (int32_t)((value + (denom / 2)) / denom);
    }
    return (int32_t)((value - (denom / 2)) / denom);
}

static int32_t ClampSymmetric(int32_t value, int32_t limit, uint8_t* pLimited)
{
    int32_t normalized = NormalizeLimit(limit, NC_MOTION_FILTER_AXIS_LIMIT_MAX);
    if (value > normalized) {
        if (pLimited != 0) {
            *pLimited = 1U;
        }
        return normalized;
    }
    if (value < -normalized) {
        if (pLimited != 0) {
            *pLimited = 1U;
        }
        return -normalized;
    }
    return value;
}

static void InitializeDefaultLimits(void)
{
    uint32_t i;
    for (i = 0U; i < AXIS_MAX; ++i) {
        g_ncMotionFilterStatus.max_velocity_per_tick[i] = NC_MOTION_FILTER_DEFAULT_MAX_VELOCITY;
        g_ncMotionFilterStatus.max_accel_per_tick[i] = NC_MOTION_FILTER_DEFAULT_MAX_ACCEL;
    }
    g_ncMotionFilterStatus.endpoint_correction_window = NC_MOTION_FILTER_ENDPOINT_WINDOW;
}

void NcMotionFilter_ResetRt(void)
{
    uint8_t enabled = g_ncMotionFilterStatus.enabled;
    uint8_t secondStageMode = g_ncMotionFilterStatus.second_stage_mode;
    uint8_t velocityWindow = g_ncMotionFilterStatus.velocity_window;
    uint8_t accelWindow = g_ncMotionFilterStatus.accel_window;
    int32_t maxVelocity[AXIS_MAX];
    int32_t maxAccel[AXIS_MAX];
    uint8_t correctionWindow = g_ncMotionFilterStatus.endpoint_correction_window;
    uint32_t i;

    for (i = 0U; i < AXIS_MAX; ++i) {
        maxVelocity[i] = g_ncMotionFilterStatus.max_velocity_per_tick[i];
        maxAccel[i] = g_ncMotionFilterStatus.max_accel_per_tick[i];
    }

    (void)memset((void*)&g_ncMotionFilterStatus, 0, sizeof(g_ncMotionFilterStatus));
    (void)memset(s_initialized, 0, sizeof(s_initialized));
    (void)memset(s_lastRawTarget, 0, sizeof(s_lastRawTarget));
    (void)memset(s_filteredTarget, 0, sizeof(s_filteredTarget));
    (void)memset(s_lastOutputVelocity, 0, sizeof(s_lastOutputVelocity));
    (void)memset(s_velocityRing, 0, sizeof(s_velocityRing));
    (void)memset(s_accelRing, 0, sizeof(s_accelRing));
    (void)memset(s_firHistory, 0, sizeof(s_firHistory));
    (void)memset(s_velocitySum, 0, sizeof(s_velocitySum));
    (void)memset(s_accelSum, 0, sizeof(s_accelSum));
    (void)memset(s_velocityIndex, 0, sizeof(s_velocityIndex));
    (void)memset(s_accelIndex, 0, sizeof(s_accelIndex));
    (void)memset(s_firIndex, 0, sizeof(s_firIndex));
    (void)memset(s_velocityCount, 0, sizeof(s_velocityCount));
    (void)memset(s_accelCount, 0, sizeof(s_accelCount));
    (void)memset(s_firCount, 0, sizeof(s_firCount));

    g_ncMotionFilterStatus.enabled = (enabled == 0U) ? 1U : enabled;
    g_ncMotionFilterStatus.second_stage_mode =
        ((secondStageMode == NC_MOTION_FILTER_SECOND_STAGE_MOVING_AVERAGE) ||
         (secondStageMode == NC_MOTION_FILTER_SECOND_STAGE_FIR)) ?
        secondStageMode : NC_MOTION_FILTER_SECOND_STAGE_FIR;
    g_ncMotionFilterStatus.velocity_window =
        ClampWindow((velocityWindow == 0U) ? NC_MOTION_FILTER_DEFAULT_VELOCITY_WINDOW : velocityWindow,
                    NC_MOTION_FILTER_VELOCITY_WINDOW_MAX);
    g_ncMotionFilterStatus.accel_window =
        ClampWindow((accelWindow == 0U) ? NC_MOTION_FILTER_DEFAULT_ACCEL_WINDOW : accelWindow,
                    NC_MOTION_FILTER_ACCEL_WINDOW_MAX);
    InitializeDefaultLimits();
    for (i = 0U; i < AXIS_MAX; ++i) {
        if (maxVelocity[i] != 0) {
            g_ncMotionFilterStatus.max_velocity_per_tick[i] =
                NormalizeLimit(maxVelocity[i], NC_MOTION_FILTER_DEFAULT_MAX_VELOCITY);
        }
        if (maxAccel[i] != 0) {
            g_ncMotionFilterStatus.max_accel_per_tick[i] =
                NormalizeLimit(maxAccel[i], NC_MOTION_FILTER_DEFAULT_MAX_ACCEL);
        }
    }
    if (correctionWindow != 0U) {
        g_ncMotionFilterStatus.endpoint_correction_window =
            ClampWindow(correctionWindow, NC_MOTION_FILTER_ENDPOINT_WINDOW);
    }
    g_ncMotionFilterStatus.generation++;
}

int32_t NcMotionFilter_SetConfigRt(uint8_t enabled,
                                   uint8_t secondStageMode,
                                   uint8_t velocityWindow,
                                   uint8_t accelWindow)
{
    if ((secondStageMode != NC_MOTION_FILTER_SECOND_STAGE_MOVING_AVERAGE) &&
        (secondStageMode != NC_MOTION_FILTER_SECOND_STAGE_FIR)) {
        return -1;
    }

    g_ncMotionFilterStatus.enabled = (uint8_t)(enabled != 0U);
    g_ncMotionFilterStatus.second_stage_mode = secondStageMode;
    g_ncMotionFilterStatus.velocity_window =
        ClampWindow(velocityWindow, NC_MOTION_FILTER_VELOCITY_WINDOW_MAX);
    g_ncMotionFilterStatus.accel_window =
        ClampWindow(accelWindow, NC_MOTION_FILTER_ACCEL_WINDOW_MAX);
    if (g_ncMotionFilterStatus.endpoint_correction_window == 0U) {
        g_ncMotionFilterStatus.endpoint_correction_window = NC_MOTION_FILTER_ENDPOINT_WINDOW;
    }
    NcMotionFilter_ResetRt();
    g_ncMotionFilterStatus.enabled = (uint8_t)(enabled != 0U);
    g_ncMotionFilterStatus.second_stage_mode = secondStageMode;
    g_ncMotionFilterStatus.velocity_window =
        ClampWindow(velocityWindow, NC_MOTION_FILTER_VELOCITY_WINDOW_MAX);
    g_ncMotionFilterStatus.accel_window =
        ClampWindow(accelWindow, NC_MOTION_FILTER_ACCEL_WINDOW_MAX);
    return 0;
}

int32_t NcMotionFilter_SetAxisLimitRt(uint8_t axis,
                                      int32_t maxVelocityPerTick,
                                      int32_t maxAccelPerTick)
{
    if (axis >= AXIS_MAX) {
        return -1;
    }
    g_ncMotionFilterStatus.max_velocity_per_tick[axis] =
        NormalizeLimit(maxVelocityPerTick, NC_MOTION_FILTER_DEFAULT_MAX_VELOCITY);
    g_ncMotionFilterStatus.max_accel_per_tick[axis] =
        NormalizeLimit(maxAccelPerTick, NC_MOTION_FILTER_DEFAULT_MAX_ACCEL);
    g_ncMotionFilterStatus.generation++;
    return 0;
}

static int32_t Stage1VelocityAverage(uint32_t axis, int32_t rawVelocity)
{
    uint8_t window = ClampWindow(g_ncMotionFilterStatus.velocity_window,
                                 NC_MOTION_FILTER_VELOCITY_WINDOW_MAX);
    uint8_t index = s_velocityIndex[axis];

    s_velocitySum[axis] -= s_velocityRing[axis][index];
    s_velocityRing[axis][index] = rawVelocity;
    s_velocitySum[axis] += rawVelocity;
    s_velocityIndex[axis] = (uint8_t)((index + 1U) % window);
    if (s_velocityCount[axis] < window) {
        s_velocityCount[axis]++;
    }
    return DivRound64(s_velocitySum[axis], s_velocityCount[axis]);
}

static int32_t Stage2MovingAverage(uint32_t axis, int32_t stage1Velocity)
{
    uint8_t window = ClampWindow(g_ncMotionFilterStatus.accel_window,
                                 NC_MOTION_FILTER_ACCEL_WINDOW_MAX);
    uint8_t index = s_accelIndex[axis];

    s_accelSum[axis] -= s_accelRing[axis][index];
    s_accelRing[axis][index] = stage1Velocity;
    s_accelSum[axis] += stage1Velocity;
    s_accelIndex[axis] = (uint8_t)((index + 1U) % window);
    if (s_accelCount[axis] < window) {
        s_accelCount[axis]++;
    }
    return DivRound64(s_accelSum[axis], s_accelCount[axis]);
}

static int32_t Stage2Fir(uint32_t axis, int32_t stage1Velocity)
{
    static const int32_t kCoeff[NC_MOTION_FILTER_FIR_TAPS] = {1, 2, 3, 2, 1};
    uint8_t index = s_firIndex[axis];
    uint8_t count;
    int64_t sum = 0;
    int64_t denom = 0;
    uint8_t i;

    s_firHistory[axis][index] = stage1Velocity;
    s_firIndex[axis] = (uint8_t)((index + 1U) % NC_MOTION_FILTER_FIR_TAPS);
    if (s_firCount[axis] < NC_MOTION_FILTER_FIR_TAPS) {
        s_firCount[axis]++;
    }

    count = s_firCount[axis];
    for (i = 0U; i < count; ++i) {
        uint8_t histIndex = (uint8_t)((s_firIndex[axis] + NC_MOTION_FILTER_FIR_TAPS - 1U - i) % NC_MOTION_FILTER_FIR_TAPS);
        int32_t coeff = kCoeff[i];
        sum += ((int64_t)s_firHistory[axis][histIndex] * coeff);
        denom += coeff;
    }
    return DivRound64(sum, denom);
}

static int32_t ApplyEndpointResidualCorrection(uint32_t axis,
                                               int32_t rawTarget,
                                               int32_t filtered,
                                               uint32_t remainingTicks)
{
    int32_t residual = rawTarget - filtered;
    uint8_t window = g_ncMotionFilterStatus.endpoint_correction_window;

    if (window == 0U) {
        window = NC_MOTION_FILTER_ENDPOINT_WINDOW;
    }
    g_ncMotionFilterStatus.endpoint_residual[axis] = residual;
    if ((remainingTicks > 0U) && (remainingTicks <= window)) {
        int32_t correction = DivRound64(residual, (int64_t)remainingTicks + 1);
        filtered += correction;
        g_ncMotionFilterStatus.residual_correction_samples++;
    }
    return filtered;
}

void NcMotionFilter_ApplyRt(volatile int32_t axisTarget[AXIS_MAX],
                            uint32_t axisMask,
                            uint8_t forceEndpoint,
                            uint32_t remainingTicks)
{
    uint32_t i;
    uint8_t pushedLimitEvent = 0U;

    if ((axisTarget == 0) || (g_ncMotionFilterStatus.enabled == 0U)) {
        return;
    }

    for (i = 0U; i < AXIS_MAX; ++i) {
        int32_t rawTarget = axisTarget[i];
        int32_t rawVelocity;
        int32_t stage1Velocity;
        int32_t desiredVelocity;
        int32_t accelLimitedVelocity;
        int32_t stage2Velocity;
        int32_t filtered;
        int32_t lag;
        uint8_t velocityLimited = 0U;
        uint8_t accelLimited = 0U;

        if ((axisMask & (1UL << i)) == 0U) {
            s_lastRawTarget[i] = rawTarget;
            s_filteredTarget[i] = rawTarget;
            s_lastOutputVelocity[i] = 0;
            s_initialized[i] = 1U;
            continue;
        }
        if (s_initialized[i] == 0U) {
            s_lastRawTarget[i] = rawTarget;
            s_filteredTarget[i] = rawTarget;
            s_lastOutputVelocity[i] = 0;
            s_initialized[i] = 1U;
            axisTarget[i] = rawTarget;
            continue;
        }

        rawVelocity = rawTarget - s_lastRawTarget[i];
        s_lastRawTarget[i] = rawTarget;
        stage1Velocity = Stage1VelocityAverage(i, rawVelocity);
        if (g_ncMotionFilterStatus.second_stage_mode == NC_MOTION_FILTER_SECOND_STAGE_MOVING_AVERAGE) {
            desiredVelocity = Stage2MovingAverage(i, stage1Velocity);
        } else {
            desiredVelocity = Stage2Fir(i, stage1Velocity);
        }

        accelLimitedVelocity = s_lastOutputVelocity[i] +
            ClampSymmetric(desiredVelocity - s_lastOutputVelocity[i],
                           g_ncMotionFilterStatus.max_accel_per_tick[i],
                           &accelLimited);
        stage2Velocity = ClampSymmetric(accelLimitedVelocity,
                                        g_ncMotionFilterStatus.max_velocity_per_tick[i],
                                        &velocityLimited);
        if (velocityLimited != 0U) {
            g_ncMotionFilterStatus.velocity_limit_events++;
        }
        if (accelLimited != 0U) {
            g_ncMotionFilterStatus.accel_limit_events++;
        }
        if (((velocityLimited != 0U) || (accelLimited != 0U)) &&
            (pushedLimitEvent == 0U)) {
            NcEvent_Push(NC_EVENT_CODE_MOTION_FILTER_SATURATED,
                         (int32_t)i,
                         stage2Velocity,
                         g_ncProgramStatus.exec_line_no);
            pushedLimitEvent = 1U;
        }

        filtered = s_filteredTarget[i] + stage2Velocity;
        filtered = ApplyEndpointResidualCorrection(i, rawTarget, filtered, remainingTicks);
        if (forceEndpoint != 0U) {
            int32_t residual = rawTarget - filtered;
            g_ncMotionFilterStatus.endpoint_residual[i] = residual;
            if (residual != 0) {
                g_ncMotionFilterStatus.endpoint_corrections++;
                NcEvent_Push(NC_EVENT_CODE_MOTION_FILTER_ENDPOINT_CORRECT,
                             (int32_t)i,
                             residual,
                             g_ncProgramStatus.exec_line_no);
            }
            filtered = rawTarget;
            stage2Velocity = filtered - s_filteredTarget[i];
        }

        s_lastOutputVelocity[i] = stage2Velocity;
        s_filteredTarget[i] = filtered;
        axisTarget[i] = filtered;

        lag = rawTarget - filtered;
        g_ncMotionFilterStatus.last_raw_velocity[i] = rawVelocity;
        g_ncMotionFilterStatus.last_velocity_stage1[i] = stage1Velocity;
        g_ncMotionFilterStatus.last_velocity_stage2[i] = stage2Velocity;
        g_ncMotionFilterStatus.last_filtered_target[i] = filtered;
        g_ncMotionFilterStatus.last_lag_error[i] = lag;
        if (Abs32Local(rawVelocity) > g_ncMotionFilterStatus.max_raw_velocity[i]) {
            g_ncMotionFilterStatus.max_raw_velocity[i] = Abs32Local(rawVelocity);
        }
        if (Abs32Local(stage2Velocity) > g_ncMotionFilterStatus.max_filtered_velocity[i]) {
            g_ncMotionFilterStatus.max_filtered_velocity[i] = Abs32Local(stage2Velocity);
        }
        if (Abs32Local(lag) > g_ncMotionFilterStatus.max_lag_error[i]) {
            g_ncMotionFilterStatus.max_lag_error[i] = Abs32Local(lag);
        }
        g_ncMotionFilterStatus.filtered_axis_mask |= (1UL << i);
    }

    if (g_ncMotionFilterStatus.applied_samples == 0U) {
        NcEvent_Push(NC_EVENT_CODE_MOTION_FILTER_APPLIED,
                     (int32_t)g_ncMotionFilterStatus.second_stage_mode,
                     (int32_t)g_ncMotionFilterStatus.velocity_window,
                     0U);
    }
    g_ncMotionFilterStatus.applied_samples++;
    g_ncMotionFilterStatus.generation++;
}
