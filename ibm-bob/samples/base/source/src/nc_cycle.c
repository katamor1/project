/* ibm-bob/samples/base/source/src/nc_cycle.c */
/* Builds fixed canned-cycle segment plans and keeps parser modal cycle state. */
/* This exists so fixed-cycle G-code behavior can grow without bloating interpolation. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_cycle.h, ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/src/nc_parser_modal.c */
#include <string.h>
#include "nc_cycle.h"
#include "nc_program.h"

#define G(whole)       NC_G_CODE_WHOLE(whole)

typedef struct {
    NC_ACTIVE_SEGMENT* pSegments;
    uint32_t count;
} NC_SEGMENT_WRITER;

static NC_EXEC_BLOCK s_modalCycle;
static uint8_t s_hasModalCycle;

static uint8_t IsTap(uint16_t code) { return (uint8_t)((code == G(74)) || (code == G(84))); }
static uint8_t IsPeck(uint16_t code) { return (uint8_t)((code == G(73)) || (code == G(83))); }
static uint8_t IsDwell(uint16_t code) { return (uint8_t)((code == G(82)) || (code == G(89))); }
static uint8_t IsBoring(uint16_t code) { return (uint8_t)((code >= G(85)) && (code <= G(89))); }
static uint8_t IsV7Cycle(uint16_t code) { return (uint8_t)((code == G(73)) || (code == G(74)) || ((code >= G(81)) && (code <= G(89)))); }

static uint16_t FlagsForCycle(uint16_t code)
{
    uint16_t flags = 0U;
    if (IsTap(code) != 0U) { flags |= NC_CYCLE_FLAG_TAP; }
    if (code == G(74)) { flags |= NC_CYCLE_FLAG_LEFT_TAP; }
    if (IsBoring(code) != 0U) { flags |= NC_CYCLE_FLAG_BORING; }
    if (code == G(88)) { flags |= NC_CYCLE_FLAG_MANUAL_RETURN; }
    return flags;
}

static void CopyAxes(int32_t* pDst, const int32_t* pSrc)
{
    uint32_t i;
    for (i = 0U; i < AXIS_MAX; ++i) { pDst[i] = pSrc[i]; }
}

static uint32_t AbsDiff32(int32_t a, int32_t b)
{
    int64_t d = (int64_t)a - (int64_t)b;
    if (d < 0) { d = -d; }
    return (d > 0xFFFFFFFFLL) ? 0xFFFFFFFFUL : (uint32_t)d;
}

static uint32_t EstimateTicks(const int32_t* pStart, const int32_t* pEnd, uint8_t rapid)
{
    uint32_t i;
    uint64_t maxDelta = 0U;
    for (i = 0U; i < AXIS_MAX; ++i) {
        uint32_t d = AbsDiff32(pStart[i], pEnd[i]);
        if ((uint64_t)d > maxDelta) { maxDelta = (uint64_t)d; }
    }
    return (uint32_t)((maxDelta / (uint64_t)(NC_POSITION_SCALE *
        ((rapid != 0U) ? 20 : 5))) + 1U);
}

static int32_t AppendSegment(NC_SEGMENT_WRITER* pWriter, const int32_t* pStart,
                             const int32_t* pEnd, uint32_t ticks, uint8_t kind)
{
    NC_ACTIVE_SEGMENT* pSeg;
    if (pWriter->count >= NC_CANNED_MAX_SEGMENTS) { return -1; }
    pSeg = &pWriter->pSegments[pWriter->count];
    CopyAxes(pSeg->start, pStart);
    CopyAxes(pSeg->end, pEnd);
    pSeg->kind = kind;
    pSeg->end_tick = (pWriter->count == 0U) ? ticks :
        (pWriter->pSegments[pWriter->count - 1U].end_tick + ticks);
    pWriter->count++;
    return 0;
}

static int32_t AppendLinear(NC_SEGMENT_WRITER* pWriter, const int32_t* pStart,
                            const int32_t* pEnd, uint8_t rapid, uint8_t kind)
{
    return AppendSegment(pWriter, pStart, pEnd,
                         EstimateTicks(pStart, pEnd, rapid), kind);
}

void NcCycle_ResetParserModal(void)
{
    (void)memset(&s_modalCycle, 0, sizeof(s_modalCycle));
    s_hasModalCycle = 0U;
}

void NcCycle_InitParserBlock(NC_EXEC_BLOCK* pBlock)
{
    if ((pBlock == 0) || (s_hasModalCycle == 0U) ||
        (pBlock->motion_type != NC_MOTION_CANNED_CYCLE)) {
        return;
    }
    pBlock->cycle_code10 = s_modalCycle.cycle_code10;
    pBlock->feature_flags |= NC_FEATURE_FLAG_CANNED_CYCLE |
        (s_modalCycle.feature_flags & (NC_FEATURE_FLAG_PARAM_R |
         NC_FEATURE_FLAG_PARAM_Q | NC_FEATURE_FLAG_PARAM_P));
    pBlock->r_value = s_modalCycle.r_value;
    pBlock->q_value = s_modalCycle.q_value;
    pBlock->p_word = s_modalCycle.p_word;
    pBlock->cycle_repeat = s_modalCycle.cycle_repeat;
    pBlock->cycle_flags = s_modalCycle.cycle_flags | NC_CYCLE_FLAG_MODAL_REPLAY;
}

void NcCycle_ApplyCycleGCode(int32_t code, NC_EXEC_BLOCK* pBlock)
{
    pBlock->feature_flags |= NC_FEATURE_FLAG_CANNED_CYCLE;
    pBlock->cycle_code10 = (uint16_t)code;
    pBlock->cycle_repeat = 1U;
    pBlock->cycle_flags = FlagsForCycle((uint16_t)code);
}

void NcCycle_CancelParserModal(void)
{
    s_hasModalCycle = 0U;
}

int32_t NcCycle_SetRepeatWord(double value, NC_EXEC_BLOCK* pBlock)
{
    uint32_t word;
    if ((pBlock == 0) || (value < 1.0) ||
        (value > (double)NC_CYCLE_REPEAT_MAX)) {
        return -2;
    }
    word = (uint32_t)(value + 0.5);
    if ((word < 1U) || (word > NC_CYCLE_REPEAT_MAX)) {
        return -2;
    }
    pBlock->cycle_repeat = (uint16_t)word;
    pBlock->cycle_flags |= NC_CYCLE_FLAG_HAS_K;
    return 0;
}

int32_t NcCycle_FinalizeParserBlock(NC_EXEC_BLOCK* pBlock,
                                    NC_ERROR_CODE* pOutError)
{
    uint8_t replay;
    if ((pBlock == 0) || (pBlock->motion_type != NC_MOTION_CANNED_CYCLE)) {
        return 0;
    }
    replay = (uint8_t)((pBlock->cycle_flags & NC_CYCLE_FLAG_MODAL_REPLAY) != 0U);
    if (pBlock->cycle_repeat == 0U) {
        pBlock->cycle_repeat = 1U;
    }
    if (pBlock->cycle_repeat > NC_CYCLE_REPEAT_MAX) {
        *pOutError = NC_ERR_RANGE;
        return -1;
    }
    if ((IsV7Cycle(pBlock->cycle_code10) != 0U) &&
        (pBlock->feed <= 0)) {
        *pOutError = NC_ERR_FEED;
        return -1;
    }
    if ((replay == 0U) && (IsV7Cycle(pBlock->cycle_code10) != 0U) &&
        (((pBlock->axis_mask & (1UL << 2U)) == 0U) ||
         ((pBlock->feature_flags & NC_FEATURE_FLAG_PARAM_R) == 0U))) {
        *pOutError = NC_ERR_RANGE;
        return -1;
    }
    if ((IsPeck(pBlock->cycle_code10) != 0U) &&
        (((pBlock->feature_flags & NC_FEATURE_FLAG_PARAM_Q) == 0U) ||
         (pBlock->q_value <= 0))) {
        *pOutError = NC_ERR_RANGE;
        return -1;
    }
    if ((IsTap(pBlock->cycle_code10) != 0U) &&
        (pBlock->spindle_speed < NC_TAP_MIN_SPINDLE_RPM)) {
        *pOutError = NC_ERR_FEED;
        return -1;
    }
    s_modalCycle = *pBlock;
    s_modalCycle.cycle_flags &= (uint16_t)~NC_CYCLE_FLAG_MODAL_REPLAY;
    s_hasModalCycle = 1U;
    return 0;
}

static int32_t AppendPeck(NC_SEGMENT_WRITER* pWriter,
                          const NC_EXEC_BLOCK* pBlock,
                          int32_t* pCurrent,
                          int32_t targetZ,
                          int32_t rZ,
                          int32_t direction)
{
    int32_t next[AXIS_MAX];
    int32_t retract[AXIS_MAX];
    uint32_t step = AbsDiff32(0, pBlock->q_value);
    while (pCurrent[2] != targetZ) {
        uint32_t remain = AbsDiff32(pCurrent[2], targetZ);
        uint32_t move = (remain < step) ? remain : step;
        CopyAxes(next, pCurrent);
        next[2] += (direction > 0) ? (int32_t)move : -(int32_t)move;
        if (((direction > 0) && (next[2] > targetZ)) ||
            ((direction < 0) && (next[2] < targetZ))) {
            next[2] = targetZ;
        }
        if (AppendLinear(pWriter, pCurrent, next, 0U, NC_SEG_KIND_LINEAR) != 0) {
            return -1;
        }
        g_ncCycleStatus.peck_index++;
        CopyAxes(pCurrent, next);
        if (pCurrent[2] != targetZ) {
            CopyAxes(retract, pCurrent);
            retract[2] = (pBlock->cycle_code10 == G(73)) ?
                (pCurrent[2] - (direction * NC_CYCLE_CHIP_BREAK_RETRACT)) : rZ;
            if (AppendLinear(pWriter, pCurrent, retract, 1U, NC_SEG_KIND_RETRACT) != 0 ||
                AppendLinear(pWriter, retract, pCurrent, 1U, NC_SEG_KIND_RAPID) != 0) {
                return -1;
            }
        }
    }
    return 0;
}

static int32_t AppendOneCycle(NC_SEGMENT_WRITER* pWriter,
                              const NC_EXEC_BLOCK* pBlock,
                              const int32_t* pStart,
                              int32_t* pOutEnd)
{
    int32_t atR[AXIS_MAX], current[AXIS_MAX], retract[AXIS_MAX];
    int32_t rZ = pBlock->r_value;
    int32_t direction = (pBlock->axis_target[2] >= rZ) ? 1 : -1;
    CopyAxes(atR, pBlock->axis_target);
    atR[2] = rZ;
    if (AppendLinear(pWriter, pStart, atR, 1U, NC_SEG_KIND_RAPID) != 0) {
        return -1;
    }
    CopyAxes(current, atR);
    if (IsPeck(pBlock->cycle_code10) != 0U) {
        if (AppendPeck(pWriter, pBlock, current, pBlock->axis_target[2],
                       rZ, direction) != 0) {
            return -1;
        }
    } else if (AppendLinear(pWriter, atR, pBlock->axis_target, 0U,
                            NC_SEG_KIND_LINEAR) != 0) {
        return -1;
    }
    if ((IsDwell(pBlock->cycle_code10) != 0U) && (pBlock->p_word != 0U) &&
        (AppendSegment(pWriter, pBlock->axis_target, pBlock->axis_target,
                       pBlock->p_word, NC_SEG_KIND_DWELL) != 0)) {
        return -1;
    }
    CopyAxes(retract, pBlock->axis_target);
    retract[2] = (pBlock->return_mode == 1U) ? pBlock->axis_start[2] : rZ;
    if (AppendLinear(pWriter, pBlock->axis_target, retract,
                     (uint8_t)(IsTap(pBlock->cycle_code10) == 0U),
                     NC_SEG_KIND_RETRACT) != 0) {
        return -1;
    }
    CopyAxes(pOutEnd, retract);
    return 0;
}

int32_t NcCycle_BuildSegmentPlanRt(const NC_EXEC_BLOCK* pBlock,
                                   NC_ACTIVE_SEGMENT* pSegments,
                                   uint32_t* pSegmentCount)
{
    NC_SEGMENT_WRITER writer;
    int32_t start[AXIS_MAX];
    uint32_t repeat;
    if ((pBlock == 0) || (pSegments == 0) || (pSegmentCount == 0)) {
        return -1;
    }
    writer.pSegments = pSegments;
    writer.count = 0U;
    g_ncCycleStatus.peck_index = 0U;
    CopyAxes(start, pBlock->axis_start);
    repeat = (pBlock->cycle_repeat == 0U) ? 1U : pBlock->cycle_repeat;
    while (repeat-- != 0U) {
        if (AppendOneCycle(&writer, pBlock, start, start) != 0) {
            return -1;
        }
    }
    g_ncCycleStatus.last_cycle_code10 = pBlock->cycle_code10;
    g_ncCycleStatus.cycle_repeat = pBlock->cycle_repeat;
    g_ncCycleStatus.peck_blocks += (IsPeck(pBlock->cycle_code10) != 0U);
    g_ncCycleStatus.tap_blocks += (IsTap(pBlock->cycle_code10) != 0U);
    g_ncCycleStatus.boring_blocks += (IsBoring(pBlock->cycle_code10) != 0U);
    g_ncCycleStatus.spindle_sync_blocks += (IsTap(pBlock->cycle_code10) != 0U);
    g_ncCycleStatus.spindle_reverse_events += (IsTap(pBlock->cycle_code10) != 0U);
    g_ncCycleStatus.manual_return_blocks +=
        ((pBlock->cycle_flags & NC_CYCLE_FLAG_MANUAL_RETURN) != 0U);
    *pSegmentCount = writer.count;
    return 0;
}
