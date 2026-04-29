/* ibm-bob/samples/base/source/src/nc_interpolation.c */
/* Prepares NC motion blocks in TS and steps them into RT axis targets. */
/* This exists so RT consumes fixed-size interpolation/cycle data instead of NC text. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_interpolation.h, ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/src/nc_program.c */
#include <math.h>
#include <string.h>
#include "nc_cycle.h"
#include "nc_feed.h"
#include "nc_motion_filter.h"
#include "nc_interp_math.h"
#include "nc_interpolation.h"
#include "nc_program.h"

#define CMD_ENABLE_SERVO  (0x00000001UL)
#define CMD_START_MOTION  (0x00000002UL)

static NC_EXEC_BLOCK s_activeBlock;
static uint32_t s_activeTick;
static uint8_t s_hasActive;
static NC_ACTIVE_SEGMENT s_segments[NC_CANNED_MAX_SEGMENTS];
static uint32_t s_segmentCount;
static uint32_t s_segmentIndex;

static uint8_t IsMotion(NC_MOTION_TYPE motion)
{
    return (uint8_t)((motion == NC_MOTION_RAPID) ||
                     (motion == NC_MOTION_LINEAR) ||
                     (motion == NC_MOTION_ARC_CW) ||
                     (motion == NC_MOTION_ARC_CCW) ||
                     (motion == NC_MOTION_CANNED_CYCLE) ||
                     (motion == NC_MOTION_THREAD) ||
                     (motion == NC_MOTION_SKIP) ||
                     (motion == NC_MOTION_REFERENCE_RETURN) ||
                     (motion == NC_MOTION_ADVANCED_INTERP));
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

static void ResetSegmentPlan(void)
{
    (void)memset(s_segments, 0, sizeof(s_segments));
    s_segmentCount = 0U;
    s_segmentIndex = 0U;
}

static void CopyAxes(int32_t* pDst, const int32_t* pSrc)
{
    uint32_t i;
    for (i = 0U; i < AXIS_MAX; ++i) {
        pDst[i] = pSrc[i];
    }
}

static uint32_t LastSegmentEndTick(void)
{
    return (s_segmentCount == 0U) ? 0U : s_segments[s_segmentCount - 1U].end_tick;
}

static int32_t AppendSegment(const int32_t* pStart,
                             const int32_t* pEnd,
                             uint32_t ticks,
                             uint8_t kind)
{
    NC_ACTIVE_SEGMENT* pSeg;

    if (s_segmentCount >= NC_CANNED_MAX_SEGMENTS) {
        return -1;
    }
    if (ticks == 0U) {
        ticks = NC_INTERP_MIN_TICKS;
    }
    pSeg = &s_segments[s_segmentCount];
    CopyAxes(pSeg->start, pStart);
    CopyAxes(pSeg->end, pEnd);
    pSeg->kind = kind;
    pSeg->end_tick = LastSegmentEndTick() + ticks;
    s_segmentCount++;
    return 0;
}

static void ApplyStagedDecelToActivePlan(void)
{
    uint16_t percent = g_ncAxisLoadStatus.decel_override_percent;
    uint32_t i;
    if ((g_ncAxisLoadStatus.decel_request == 0U) ||
        (percent == 0U) || (percent >= NC_FEED_OVERRIDE_DEFAULT_PERCENT)) {
        return;
    }
    if (s_activeBlock.interp_ticks < NC_INTERP_MAX_TICKS) {
        uint64_t scaled = ((uint64_t)s_activeBlock.interp_ticks * 100U + (percent - 1U)) / percent;
        s_activeBlock.interp_ticks = (scaled > NC_INTERP_MAX_TICKS) ? NC_INTERP_MAX_TICKS : (uint32_t)scaled;
    }
    for (i = 0U; i < s_segmentCount; ++i) {
        uint64_t scaled = ((uint64_t)s_segments[i].end_tick * 100U + (percent - 1U)) / percent;
        s_segments[i].end_tick = (scaled > NC_INTERP_MAX_TICKS) ? NC_INTERP_MAX_TICKS : (uint32_t)scaled;
    }
    if (s_activeBlock.accel_ticks != 0U) {
        s_activeBlock.accel_ticks = (s_activeBlock.accel_ticks * 100U + (percent - 1U)) / percent;
    }
    if (s_activeBlock.decel_ticks != 0U) {
        s_activeBlock.decel_ticks = (s_activeBlock.decel_ticks * 100U + (percent - 1U)) / percent;
    }
    s_activeBlock.feed_override_percent = percent;
    NcEvent_Push(NC_EVENT_CODE_AXIS_DECEL_APPLIED,
                 (int32_t)g_ncAxisLoadStatus.staged_decel_level,
                 (int32_t)percent,
                 s_activeBlock.line_no);
}

static int32_t BuildArcSegmentPlan(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t a;
    uint32_t b;
    uint32_t i;
    uint32_t seg;
    uint32_t segmentCount;
    uint32_t totalTicks;
    double sweep;
    double startAngle;
    int32_t prev[AXIS_MAX];
    int32_t next[AXIS_MAX];

    ResetSegmentPlan();
    NcInterp_PlaneAxes(pBlock->plane, &a, &b);
    sweep = NcInterp_SignedSweep(pBlock->motion_type,
                                 (double)pBlock->axis_start[a],
                                 (double)pBlock->axis_start[b],
                                 (double)pBlock->axis_target[a],
                                 (double)pBlock->axis_target[b],
                                 (double)pBlock->arc_center[a],
                                 (double)pBlock->arc_center[b]);
    startAngle = atan2((double)pBlock->axis_start[b] -
                       (double)pBlock->arc_center[b],
                       (double)pBlock->axis_start[a] -
                       (double)pBlock->arc_center[a]);
    segmentCount = (uint32_t)((fabs(sweep) * (double)pBlock->arc_radius) /
                              (double)(NC_POSITION_SCALE / 2));
    if (segmentCount < 4U) {
        segmentCount = 4U;
    }
    if (segmentCount > NC_ARC_SEGMENT_MAX) {
        segmentCount = NC_ARC_SEGMENT_MAX;
    }
    totalTicks = (pBlock->interp_ticks == 0U) ? NC_INTERP_MIN_TICKS : pBlock->interp_ticks;
    CopyAxes(prev, pBlock->axis_start);

    for (seg = 1U; seg <= segmentCount; ++seg) {
        double ratio = (double)seg / (double)segmentCount;
        double angle = startAngle + (sweep * ratio);
        uint32_t startTick = ((seg - 1U) * totalTicks) / segmentCount;
        uint32_t endTick = (seg * totalTicks) / segmentCount;
        uint32_t ticks = (endTick > startTick) ? (endTick - startTick) : 1U;

        for (i = 0U; i < AXIS_MAX; ++i) {
            double start = (double)pBlock->axis_start[i];
            double end = (double)pBlock->axis_target[i];
            next[i] = RoundFixed(start + ((end - start) * ratio));
        }
        next[a] = RoundFixed((double)pBlock->arc_center[a] +
                             ((double)pBlock->arc_radius * cos(angle)));
        next[b] = RoundFixed((double)pBlock->arc_center[b] +
                             ((double)pBlock->arc_radius * sin(angle)));
        if (AppendSegment(prev, next, ticks, NC_SEG_KIND_LINEAR) != 0) {
            return -1;
        }
        CopyAxes(prev, next);
    }

    g_ncPathControlStatus.arc_segments_generated += segmentCount;
    g_ncPathControlStatus.generation++;
    return 0;
}

void NcInterpolation_Reset(void)
{
    (void)memset(&s_activeBlock, 0, sizeof(s_activeBlock));
    s_activeTick = 0U;
    s_hasActive = 0U;
    ResetSegmentPlan();
    (void)memset((void*)&g_ncInterpStatus, 0, sizeof(g_ncInterpStatus));
    (void)memset((void*)&g_ncCycleStatus, 0, sizeof(g_ncCycleStatus));
    g_ncInterpStatus.state = NC_INTERP_IDLE;
    g_ncInterpStatus.last_error = NC_ERR_NONE;
    NcFeed_Reset();
    NcMotionFilter_ResetRt();
}

void NcInterpolation_Cancel(void)
{
    s_hasActive = 0U;
    s_activeTick = 0U;
    ResetSegmentPlan();
    g_ncInterpStatus.state = NC_INTERP_IDLE;
    g_ncInterpStatus.generation++;
    g_ncCycleStatus.active = 0U;
    g_ncCycleStatus.generation++;
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
    if ((pBlock->motion_type == NC_MOTION_CANNED_CYCLE) ||
        (pBlock->motion_type == NC_MOTION_THREAD) ||
        (pBlock->motion_type == NC_MOTION_SKIP) ||
        (pBlock->motion_type == NC_MOTION_REFERENCE_RETURN) ||
        (pBlock->motion_type == NC_MOTION_ADVANCED_INTERP)) {
        return NcFeed_PrepareMotionTs(pBlock, LinearLengthUnits(pBlock), pOutError);
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
    ResetSegmentPlan();

    if ((s_activeBlock.motion_type == NC_MOTION_ARC_CW) ||
        (s_activeBlock.motion_type == NC_MOTION_ARC_CCW)) {
        if (BuildArcSegmentPlan(&s_activeBlock) != 0) {
            g_ncInterpStatus.last_error = NC_ERR_ARC_GEOMETRY;
            return -1;
        }
        s_activeBlock.interp_ticks = LastSegmentEndTick();
    } else if (s_activeBlock.motion_type == NC_MOTION_CANNED_CYCLE) {
        if (NcCycle_BuildSegmentPlanRt(&s_activeBlock,
                                       s_segments,
                                       &s_segmentCount) != 0) {
            g_ncInterpStatus.last_error = NC_ERR_RANGE;
            return -1;
        }
        s_activeBlock.interp_ticks = LastSegmentEndTick();
        g_ncCycleStatus.active = 1U;
        g_ncCycleStatus.skip_triggered = 0U;
        g_ncCycleStatus.line_no = s_activeBlock.line_no;
        g_ncCycleStatus.cycle_code10 = s_activeBlock.cycle_code10;
        g_ncCycleStatus.phase_index = 0U;
        g_ncCycleStatus.phase_count = s_segmentCount;
        g_ncCycleStatus.cycle_repeat = s_activeBlock.cycle_repeat;
        g_ncCycleStatus.generation++;
    } else if (s_activeBlock.motion_type == NC_MOTION_SKIP) {
        g_ncCycleStatus.active = 1U;
        g_ncCycleStatus.skip_triggered = 0U;
        g_ncCycleStatus.line_no = s_activeBlock.line_no;
        g_ncCycleStatus.cycle_code10 = s_activeBlock.g_code10;
        g_ncCycleStatus.phase_index = 0U;
        g_ncCycleStatus.phase_count = 1U;
        g_ncCycleStatus.generation++;
    } else {
        g_ncCycleStatus.active = 0U;
    }

    ApplyStagedDecelToActivePlan();
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
    feedResult = NcFeed_BeginBlockRt(&s_activeBlock);
    return (feedResult == -2) ? -2 : 0;
}

static void OutputLinearBlock(double ratio)
{
    uint32_t i;

    for (i = 0U; i < AXIS_MAX; ++i) {
        double start = (double)s_activeBlock.axis_start[i];
        double end = (double)s_activeBlock.axis_target[i];
        g_ioOut.axis_target[i] = RoundFixed(start + ((end - start) * ratio));
    }
}

static void OutputSegmentAtTick(uint32_t tick)
{
    const NC_ACTIVE_SEGMENT* pSeg;
    uint32_t startTick;
    uint32_t i;
    double ratio;

    if (s_segmentCount == 0U) {
        OutputLinearBlock(1.0);
        return;
    }

    while (((s_segmentIndex + 1U) < s_segmentCount) &&
           (tick > s_segments[s_segmentIndex].end_tick)) {
        s_segmentIndex++;
    }
    pSeg = &s_segments[s_segmentIndex];
    startTick = (s_segmentIndex == 0U) ? 0U : s_segments[s_segmentIndex - 1U].end_tick;
    if (pSeg->end_tick <= startTick) {
        ratio = 1.0;
    } else {
        ratio = ((double)(tick - startTick)) /
                ((double)(pSeg->end_tick - startTick));
    }
    if (ratio > 1.0) {
        ratio = 1.0;
    }
    for (i = 0U; i < AXIS_MAX; ++i) {
        double start = (double)pSeg->start[i];
        double end = (double)pSeg->end[i];
        g_ioOut.axis_target[i] = RoundFixed(start + ((end - start) * ratio));
    }
    g_ncCycleStatus.phase_index = s_segmentIndex;
    g_ncCycleStatus.generation++;
}


static void CompleteActiveBlock(uint8_t countAsSegment)
{
    s_hasActive = 0U;
    g_ncInterpStatus.state = NC_INTERP_COMPLETED;
    if (countAsSegment != 0U) {
        g_ncInterpStatus.completed_segments++;
        g_ncInterpStatus.completed_ticks += s_activeBlock.interp_ticks;
    }
    if (s_activeBlock.motion_type == NC_MOTION_CANNED_CYCLE) {
        g_ncCycleStatus.active = 0U;
        g_ncCycleStatus.completed_cycles++;
        g_ncCycleStatus.repeat_completed +=
            (s_activeBlock.cycle_repeat == 0U) ? 1U : s_activeBlock.cycle_repeat;
        g_ncCycleStatus.generation++;
    } else if (s_activeBlock.motion_type == NC_MOTION_SKIP) {
        g_ncCycleStatus.active = 0U;
        g_ncCycleStatus.generation++;
    }
    g_ncInterpStatus.generation++;
}

static void CheckSkipInput(void)
{
    uint32_t i;

    if ((s_activeBlock.motion_type != NC_MOTION_SKIP) ||
        ((g_ioIn.seq_bits & SENSOR_SEQ_SKIP_BIT) == 0U)) {
        return;
    }

    g_ncCycleStatus.skip_triggered = 1U;
    g_ncCycleStatus.completed_skips++;
    g_ncCycleStatus.last_skip_cycle = g_machineCtx.cycle_count;
    for (i = 0U; i < AXIS_MAX; ++i) {
        g_ncCycleStatus.last_skip_position[i] = g_ioOut.axis_target[i];
        s_activeBlock.axis_target[i] = g_ioOut.axis_target[i];
    }
    g_ncCycleStatus.generation++;
    CompleteActiveBlock(1U);
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
    } else if ((s_activeBlock.motion_type == NC_MOTION_CANNED_CYCLE) ||
               (s_activeBlock.motion_type == NC_MOTION_ARC_CW) ||
               (s_activeBlock.motion_type == NC_MOTION_ARC_CCW)) {
        OutputSegmentAtTick(s_activeTick);
    } else if (s_activeTick == s_activeBlock.interp_ticks) {
        OutputLinearBlock(1.0);
    } else {
        OutputLinearBlock(ratio);
    }

    if (s_activeBlock.motion_type != NC_MOTION_DWELL) {
        NcMotionFilter_ApplyRt(g_ioOut.axis_target,
                               s_activeBlock.axis_mask,
                               (uint8_t)(s_activeTick == s_activeBlock.interp_ticks),
                               (s_activeBlock.interp_ticks > s_activeTick) ?
                                   (s_activeBlock.interp_ticks - s_activeTick) : 0U);
    }
    CheckSkipInput();
    if (s_hasActive == 0U) {
        return 0;
    }

    g_ncInterpStatus.active_tick = s_activeTick;
    NcFeed_UpdateRt(&s_activeBlock,
                    s_activeTick,
                    (uint8_t)(s_activeTick == s_activeBlock.interp_ticks));
    if (s_activeTick == s_activeBlock.interp_ticks) {
        CompleteActiveBlock((uint8_t)(s_activeBlock.motion_type != NC_MOTION_DWELL));
    }
    return 0;
}
