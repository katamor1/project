/* ibm-bob/samples/base/source/src/nc_lookahead.c */
/* Implements lightweight TS-side corner slowdown and path status tracking. */
/* This exists so RT receives fixed execution blocks with conservative timing. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_lookahead.h, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/src/nc_interpolation.c */
#include <math.h>
#include <string.h>
#include "nc_lookahead.h"
#include "nc_program.h"
#include "nc_interp_math.h"

#define NC_PI (3.14159265358979323846)

static uint8_t s_hasPrevMotion;
static uint8_t s_g08Mode;
static uint8_t s_hpccMode;
static int32_t s_prevStart[AXIS_MAX];
static int32_t s_prevEnd[AXIS_MAX];

static double VectorLength(const int32_t* pA, const int32_t* pB);

static uint8_t IsMotionForLookahead(NC_MOTION_TYPE motion)
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

static uint8_t IsG08Command(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)(pBlock->g_code10 == (uint16_t)NC_G_CODE_WHOLE(8));
}

static uint8_t IsG05Command(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)(pBlock->g_code10 == (uint16_t)NC_G_CODE_WHOLE(5));
}

static uint8_t IsG08Target(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)((pBlock->motion_type == NC_MOTION_LINEAR) ||
                     (pBlock->motion_type == NC_MOTION_ARC_CW) ||
                     (pBlock->motion_type == NC_MOTION_ARC_CCW));
}

static uint8_t IsExactStopBlock(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)((pBlock->feature_flags & NC_FEATURE_FLAG_EXACT_STOP) != 0U);
}

static void ApplyG08Command(const NC_EXEC_BLOCK* pBlock)
{
    if (pBlock->p_word <= 2U) {
        s_g08Mode = (uint8_t)pBlock->p_word;
    }
}

static void ApplyG05Command(const NC_EXEC_BLOCK* pBlock)
{
    if (pBlock->p_word == 10000U) {
        s_hpccMode = 1U;
    } else if (pBlock->p_word == 0U) {
        s_hpccMode = 0U;
    }
}

static uint32_t ActiveG08Tolerance(void)
{
    return (s_g08Mode == 2U) ? (uint32_t)NC_G08_HIGH_PRECISION_TOLERANCE :
                               (uint32_t)NC_G08_STANDARD_TOLERANCE;
}

static double MinDouble(double a, double b)
{
    return (a < b) ? a : b;
}

static double CornerDeviation(const int32_t* p0,
                              const int32_t* p1,
                              const int32_t* p2,
                              uint32_t angleDeg)
{
    double lenA = VectorLength(p0, p1);
    double lenB = VectorLength(p1, p2);
    double radians = ((double)angleDeg * NC_PI) / 180.0;

    return MinDouble(lenA, lenB) * sin(radians * 0.5);
}

static uint8_t IsWithinG08Tolerance(const NC_EXEC_BLOCK* pBlock,
                                    uint32_t angleDeg)
{
    double deviation = CornerDeviation(s_prevStart,
                                       s_prevEnd,
                                       pBlock->axis_target,
                                       angleDeg);

    return (uint8_t)(deviation <= (double)ActiveG08Tolerance());
}

static uint8_t IsWithinHpccTolerance(const NC_EXEC_BLOCK* pBlock,
                                     uint32_t angleDeg)
{
    double deviation = CornerDeviation(s_prevStart,
                                       s_prevEnd,
                                       pBlock->axis_target,
                                       angleDeg);

    return (uint8_t)(deviation <= (double)NC_HPCC_CORNER_TOLERANCE);
}

static void ApplyG08Flags(NC_EXEC_BLOCK* pBlock)
{
    if (s_g08Mode == 0U) {
        return;
    }
    pBlock->feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
    if (s_g08Mode == 2U) {
        pBlock->feature_flags |= NC_FEATURE_FLAG_HIGH_PRECISION;
    }
}

static void ApplyHpccFlags(NC_EXEC_BLOCK* pBlock)
{
    if (s_hpccMode == 0U) {
        return;
    }
    pBlock->feature_flags |= (NC_FEATURE_FLAG_HPCC |
                              NC_FEATURE_FLAG_HIGH_PRECISION |
                              NC_FEATURE_FLAG_SMOOTHING);
    if (pBlock->feed_override_percent > NC_HPCC_FEED_OVERRIDE_PERCENT) {
        pBlock->feed_override_percent = NC_HPCC_FEED_OVERRIDE_PERCENT;
    }
    g_ncPathControlStatus.last_override_percent = pBlock->feed_override_percent;
}

static void CopyAxes(int32_t* pDst, const int32_t* pSrc)
{
    uint32_t i;
    for (i = 0U; i < AXIS_MAX; ++i) {
        pDst[i] = pSrc[i];
    }
}

static double VectorLength(const int32_t* pA, const int32_t* pB)
{
    uint32_t i;
    double sum = 0.0;
    for (i = 0U; i < AXIS_MAX; ++i) {
        double d = (double)pB[i] - (double)pA[i];
        sum += d * d;
    }
    return sqrt(sum);
}

static uint32_t CornerAngleDeg(const int32_t* p0,
                               const int32_t* p1,
                               const int32_t* p2)
{
    uint32_t i;
    double dot = 0.0;
    double lenA = 0.0;
    double lenB = 0.0;

    for (i = 0U; i < AXIS_MAX; ++i) {
        double a = (double)p1[i] - (double)p0[i];
        double b = (double)p2[i] - (double)p1[i];
        dot += a * b;
        lenA += a * a;
        lenB += b * b;
    }
    if ((lenA <= 0.0) || (lenB <= 0.0)) {
        return 0U;
    }
    dot /= (sqrt(lenA) * sqrt(lenB));
    if (dot > 1.0) {
        dot = 1.0;
    } else if (dot < -1.0) {
        dot = -1.0;
    }
    return (uint32_t)((acos(dot) * 180.0 / NC_PI) + 0.5);
}

static void ApplyCornerSlowdown(NC_EXEC_BLOCK* pBlock, uint32_t angleDeg)
{
    uint32_t scaledTicks;

    if ((angleDeg < NC_LOOKAHEAD_CORNER_WARN_DEG) ||
        (pBlock->interp_ticks == 0U) ||
        (pBlock->motion_type == NC_MOTION_RAPID)) {
        return;
    }

    scaledTicks = (uint32_t)(((uint64_t)pBlock->interp_ticks * 100ULL) /
                             (uint64_t)NC_LOOKAHEAD_CORNER_SLOW_PERCENT);
    if (scaledTicks < pBlock->interp_ticks) {
        scaledTicks = pBlock->interp_ticks;
    }
    if (scaledTicks > NC_INTERP_MAX_TICKS) {
        scaledTicks = NC_INTERP_MAX_TICKS;
    }
    pBlock->interp_ticks = scaledTicks;
    if (pBlock->feed_override_percent > NC_LOOKAHEAD_CORNER_SLOW_PERCENT) {
        pBlock->feed_override_percent = NC_LOOKAHEAD_CORNER_SLOW_PERCENT;
    }
    g_ncPathControlStatus.corner_slowdown_events++;
    g_ncPathControlStatus.last_override_percent = NC_LOOKAHEAD_CORNER_SLOW_PERCENT;
}

void NcLookahead_Reset(void)
{
    s_hasPrevMotion = 0U;
    s_g08Mode = 0U;
    s_hpccMode = 0U;
    (void)memset(s_prevStart, 0, sizeof(s_prevStart));
    (void)memset(s_prevEnd, 0, sizeof(s_prevEnd));
    (void)memset((void*)&g_ncPathControlStatus, 0, sizeof(g_ncPathControlStatus));
}

void NcLookahead_ApplyBlockTs(NC_EXEC_BLOCK* pBlock)
{
    uint32_t angleDeg = 0U;

    if (pBlock == 0) {
        return;
    }

    if (IsG08Command(pBlock) != 0U) {
        ApplyG08Command(pBlock);
        g_ncPathControlStatus.last_line_no = pBlock->line_no;
        g_ncPathControlStatus.generation++;
        return;
    }
    if (IsG05Command(pBlock) != 0U) {
        ApplyG05Command(pBlock);
        g_ncPathControlStatus.last_line_no = pBlock->line_no;
        g_ncPathControlStatus.generation++;
        return;
    }

    if (IsMotionForLookahead(pBlock->motion_type) == 0U) {
        return;
    }

    g_ncPathControlStatus.lookahead_blocks++;
    g_ncPathControlStatus.last_line_no = pBlock->line_no;
    if ((pBlock->motion_type == NC_MOTION_ARC_CW) ||
        (pBlock->motion_type == NC_MOTION_ARC_CCW)) {
        g_ncPathControlStatus.arc_blocks++;
        if (VectorLength(pBlock->axis_start, pBlock->axis_target) > 0.0) {
            uint32_t a;
            uint32_t b;
            NcInterp_PlaneAxes(pBlock->plane, &a, &b);
            if (pBlock->axis_start[3U - a - b] != pBlock->axis_target[3U - a - b]) {
                g_ncPathControlStatus.helical_blocks++;
            }
        }
    }

    if (s_hasPrevMotion != 0U) {
        angleDeg = CornerAngleDeg(s_prevStart, s_prevEnd, pBlock->axis_target);
        g_ncPathControlStatus.last_corner_angle_deg = angleDeg;
        if ((s_hpccMode != 0U) &&
            (IsG08Target(pBlock) != 0U) &&
            (IsExactStopBlock(pBlock) == 0U)) {
            ApplyHpccFlags(pBlock);
            if (IsWithinHpccTolerance(pBlock, angleDeg) == 0U) {
                ApplyCornerSlowdown(pBlock, angleDeg);
            }
        } else if ((s_g08Mode != 0U) &&
                   (IsG08Target(pBlock) != 0U) &&
                   (IsExactStopBlock(pBlock) == 0U)) {
            ApplyG08Flags(pBlock);
            if (IsWithinG08Tolerance(pBlock, angleDeg) == 0U) {
                ApplyCornerSlowdown(pBlock, angleDeg);
            }
        } else {
            ApplyCornerSlowdown(pBlock, angleDeg);
        }
    } else {
        if ((s_hpccMode != 0U) && (IsG08Target(pBlock) != 0U) &&
            (IsExactStopBlock(pBlock) == 0U)) {
            ApplyHpccFlags(pBlock);
        } else if ((s_g08Mode != 0U) && (IsG08Target(pBlock) != 0U) &&
            (IsExactStopBlock(pBlock) == 0U)) {
            ApplyG08Flags(pBlock);
        }
        g_ncPathControlStatus.last_override_percent = pBlock->feed_override_percent;
    }

    CopyAxes(s_prevStart, pBlock->axis_start);
    CopyAxes(s_prevEnd, pBlock->axis_target);
    s_hasPrevMotion = 1U;
    g_ncPathControlStatus.generation++;
}
