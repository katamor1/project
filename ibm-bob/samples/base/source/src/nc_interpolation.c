/* ibm-bob/samples/base/source/src/nc_interpolation.c */
/* Prepares NC motion blocks in TS and steps them into RT axis targets. */
/* This exists so RT consumes fixed-size interpolation data instead of NC text. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_interpolation.h, ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/src/nc_program.c */
#include <math.h>
#include <string.h>
#include "nc_feed.h"
#include "nc_interp_math.h"
#include "nc_interpolation.h"
#include "nc_program.h"

#define CMD_ENABLE_SERVO  (0x00000001UL)
#define CMD_START_MOTION  (0x00000002UL)

static NC_EXEC_BLOCK s_activeBlock;
static uint32_t s_activeTick;
static uint8_t s_hasActive;

static uint8_t IsMotion(NC_MOTION_TYPE motion)
{
    return (uint8_t)((motion == NC_MOTION_RAPID) ||
                     (motion == NC_MOTION_LINEAR) ||
                     (motion == NC_MOTION_ARC_CW) ||
                     (motion == NC_MOTION_ARC_CCW));
}

static uint8_t IsActiveType(NC_MOTION_TYPE motion)
{
    return (uint8_t)((IsMotion(motion) != 0U) || (motion == NC_MOTION_DWELL));
}

static int32_t RoundFixed(double value)
{
    return (int32_t)(value + ((value >= 0.0) ? 0.5 : -0.5));
}

static double LinearLengthUnits(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t i;
    double sum = 0.0;

    for (i = 0U; i < AXIS_MAX; ++i) {
        double delta =
            (double)pBlock->axis_target[i] - (double)pBlock->axis_start[i];
        sum += delta * delta;
    }
    return sqrt(sum) / (double)NC_POSITION_SCALE;
}

void NcInterpolation_Reset(void)
{
    (void)memset(&s_activeBlock, 0, sizeof(s_activeBlock));
    s_activeTick = 0U;
    s_hasActive = 0U;
    (void)memset((void*)&g_ncInterpStatus, 0, sizeof(g_ncInterpStatus));
    g_ncInterpStatus.state = NC_INTERP_IDLE;
    g_ncInterpStatus.last_error = NC_ERR_NONE;
    NcFeed_Reset();
}

void NcInterpolation_Cancel(void)
{
    s_hasActive = 0U;
    s_activeTick = 0U;
    g_ncInterpStatus.state = NC_INTERP_IDLE;
    g_ncInterpStatus.generation++;
    NcFeed_CancelRt();
}

uint8_t NcInterpolation_HasActive(void)
{
    return s_hasActive;
}

int32_t NcInterpolation_PrepareBlockTs(NC_EXEC_BLOCK* pBlock,
                                       NC_ERROR_CODE* pOutError)
{
    if ((pBlock == 0) || (pOutError == 0)) {
        return -1;
    }
    *pOutError = NC_ERR_NONE;
    if (pBlock->motion_type == NC_MOTION_DWELL) {
        return NcFeed_PrepareDwellTs(pBlock, pBlock->interp_ticks, pOutError);
    }
    if (IsMotion(pBlock->motion_type) == 0U) {
        return 0;
    }
    if ((pBlock->motion_type == NC_MOTION_ARC_CW) ||
        (pBlock->motion_type == NC_MOTION_ARC_CCW)) {
        double pathLengthUnits;

        if (NcInterp_PrepareArcTs(pBlock, &pathLengthUnits, pOutError) != 0) {
            return -1;
        }
        return NcFeed_PrepareMotionTs(pBlock, pathLengthUnits, pOutError);
    }
    return NcFeed_PrepareMotionTs(pBlock, LinearLengthUnits(pBlock), pOutError);
}

int32_t NcInterpolation_BeginBlockRt(const NC_EXEC_BLOCK* pBlock)
{
    int32_t feedResult;

    if ((pBlock == 0) || (IsActiveType(pBlock->motion_type) == 0U)) {
        return -1;
    }
    (void)memcpy(&s_activeBlock, pBlock, sizeof(s_activeBlock));
    if (s_activeBlock.interp_ticks == 0U) {
        s_activeBlock.interp_ticks = NC_INTERP_MIN_TICKS;
    }
    s_activeTick = 0U;
    s_hasActive = 1U;
    g_ncInterpStatus.state = NC_INTERP_ACTIVE;
    g_ncInterpStatus.line_no = pBlock->line_no;
    g_ncInterpStatus.active_tick = 0U;
    g_ncInterpStatus.active_total_ticks = s_activeBlock.interp_ticks;
    g_ncInterpStatus.generation++;
    feedResult = NcFeed_BeginBlockRt(pBlock);
    return (feedResult == -2) ? -2 : 0;
}

static void OutputLinear(double ratio)
{
    uint32_t i;

    for (i = 0U; i < AXIS_MAX; ++i) {
        double start = (double)s_activeBlock.axis_start[i];
        double end = (double)s_activeBlock.axis_target[i];
        g_ioOut.axis_target[i] = RoundFixed(start + ((end - start) * ratio));
    }
}

static void OutputArc(double ratio)
{
    uint32_t a;
    uint32_t b;
    double sweep;
    double angle;

    OutputLinear(ratio);
    NcInterp_PlaneAxes(s_activeBlock.plane, &a, &b);
    sweep = NcInterp_SignedSweep(s_activeBlock.motion_type,
                                 (double)s_activeBlock.axis_start[a],
                                 (double)s_activeBlock.axis_start[b],
                                 (double)s_activeBlock.axis_target[a],
                                 (double)s_activeBlock.axis_target[b],
                                 (double)s_activeBlock.arc_center[a],
                                 (double)s_activeBlock.arc_center[b]);
    angle = atan2((double)s_activeBlock.axis_start[b] -
                  (double)s_activeBlock.arc_center[b],
                  (double)s_activeBlock.axis_start[a] -
                  (double)s_activeBlock.arc_center[a]) + (sweep * ratio);
    g_ioOut.axis_target[a] =
        RoundFixed((double)s_activeBlock.arc_center[a] +
                   ((double)s_activeBlock.arc_radius * cos(angle)));
    g_ioOut.axis_target[b] =
        RoundFixed((double)s_activeBlock.arc_center[b] +
                   ((double)s_activeBlock.arc_radius * sin(angle)));
}

int32_t NcInterpolation_StepRt(void)
{
    double ratio;

    if (s_hasActive == 0U) {
        return 0;
    }

    s_activeTick++;
    if (s_activeTick > s_activeBlock.interp_ticks) {
        s_activeTick = s_activeBlock.interp_ticks;
    }
    ratio = NcFeed_ProfileRatioRt(&s_activeBlock, s_activeTick);
    if (s_activeBlock.motion_type != NC_MOTION_DWELL) {
        g_ioOut.command_bits |= (CMD_ENABLE_SERVO | CMD_START_MOTION);
    }

    if (s_activeBlock.motion_type == NC_MOTION_DWELL) {
        /* Dwell keeps the current target; only the active tick advances. */
    } else if (s_activeTick == s_activeBlock.interp_ticks) {
        OutputLinear(1.0);
    } else if ((s_activeBlock.motion_type == NC_MOTION_ARC_CW) ||
               (s_activeBlock.motion_type == NC_MOTION_ARC_CCW)) {
        OutputArc(ratio);
    } else {
        OutputLinear(ratio);
    }

    g_ncInterpStatus.active_tick = s_activeTick;
    NcFeed_UpdateRt(&s_activeBlock,
                    s_activeTick,
                    (uint8_t)(s_activeTick == s_activeBlock.interp_ticks));
    if (s_activeTick == s_activeBlock.interp_ticks) {
        s_hasActive = 0U;
        g_ncInterpStatus.state = NC_INTERP_COMPLETED;
        if (s_activeBlock.motion_type != NC_MOTION_DWELL) {
            g_ncInterpStatus.completed_segments++;
            g_ncInterpStatus.completed_ticks += s_activeBlock.interp_ticks;
        }
        g_ncInterpStatus.generation++;
    }
    return 0;
}
