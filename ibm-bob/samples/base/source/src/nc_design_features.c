/* ibm-bob/samples/base/source/src/nc_design_features.c */
/* Completes design-document features that can be implemented as TS fixed-block conversion plus RT-safe status publication. */
#include <string.h>
#include "nc_design_features.h"
#include "nc_codes.h"
#include "nc_program.h"

#define G(whole)       NC_G_CODE_WHOLE(whole)
#define GD(whole, dec) NC_G_CODE10((whole), (dec))

static uint16_t s_autoCornerOverride = NC_DESIGN_AUTO_CORNER_OVERRIDE_PERCENT;

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

static void CopyAxes(int32_t* pDst, const int32_t* pSrc)
{
    uint32_t axis;
    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        pDst[axis] = pSrc[axis];
    }
}

static int32_t Abs32(int32_t value)
{
    if (value == (int32_t)0x80000000UL) {
        return 0x7fffffff;
    }
    return (value < 0) ? -value : value;
}

static uint32_t BitCount(uint32_t mask)
{
    uint32_t count = 0U;
    while (mask != 0U) {
        count += (mask & 1U);
        mask >>= 1U;
    }
    return count;
}

static uint8_t IsGrindingCycle(uint16_t code10)
{
    return (uint8_t)(((code10 >= (uint16_t)GD(70, 7)) &&
                      (code10 <= (uint16_t)GD(76, 7))) ||
                     ((code10 >= (uint16_t)G(77)) &&
                      (code10 <= (uint16_t)G(79))));
}

static uint8_t IsHelicalB(const NC_EXEC_BLOCK* pBlock)
{
    uint8_t arc = (uint8_t)((pBlock->motion_type == NC_MOTION_ARC_CW) ||
                            (pBlock->motion_type == NC_MOTION_ARC_CCW) ||
                            (pBlock->g_code10 == (uint16_t)G(2)) ||
                            (pBlock->g_code10 == (uint16_t)G(3)));
    return (uint8_t)((arc != 0U) &&
                     ((pBlock->axis_mask & ((1UL << 2U) | (1UL << 3U))) != 0U));
}

static uint8_t IsThreeDInterference(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)((BitCount(pBlock->axis_mask) >= 3U) ||
                     ((pBlock->feature_flags &
                       (NC_FEATURE_FLAG_TILTED_PLANE | NC_FEATURE_FLAG_TOOL_AXIS_DIR)) != 0U));
}

static uint8_t IsFollowupRollover(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t axis;
    if (pBlock->motion_type != NC_MOTION_REFERENCE_RETURN) {
        return 0U;
    }
    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        if ((pBlock->axis_mask & (1UL << axis)) == 0U) {
            continue;
        }
        if (Abs32(pBlock->axis_target[axis]) > g_ncReferenceStatus.rollover_limit[axis]) {
            return 1U;
        }
    }
    return 0U;
}

static uint8_t IsInvolute(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)((pBlock->g_code10 == (uint16_t)GD(2, 2)) ||
                     (pBlock->g_code10 == (uint16_t)GD(3, 2)));
}

static uint8_t IsHighPrecisionContour(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)((pBlock->g_code10 == (uint16_t)G(8)) ||
                     (pBlock->g_code10 == (uint16_t)G(5)) ||
                     (pBlock->g_code10 == (uint16_t)GD(5, 1)) ||
                     (pBlock->g_code10 == (uint16_t)GD(5, 2)) ||
                     ((pBlock->feature_flags &
                       (NC_FEATURE_FLAG_HPCC | NC_FEATURE_FLAG_HIGH_PRECISION |
                        NC_FEATURE_FLAG_SMOOTHING)) != 0U));
}

static uint8_t IsExactStop(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)((pBlock->g_code10 == (uint16_t)G(9)) ||
                     (pBlock->g_code10 == (uint16_t)G(61)) ||
                     (pBlock->g_code10 == (uint16_t)G(63)) ||
                     ((pBlock->feature_flags & NC_FEATURE_FLAG_EXACT_STOP) != 0U));
}

static uint8_t IsRigidTap(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)(((pBlock->cycle_code10 == (uint16_t)G(84)) ||
                      (pBlock->cycle_code10 == (uint16_t)G(74))) &&
                     (pBlock->motion_type == NC_MOTION_CANNED_CYCLE));
}

static uint8_t IsThreadChamfer(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)((pBlock->motion_type == NC_MOTION_THREAD) ||
                     (pBlock->cycle_code10 == (uint16_t)GD(76, 7)) ||
                     (pBlock->g_code10 == (uint16_t)G(76)) ||
                     (pBlock->g_code10 == (uint16_t)G(78)) ||
                     (pBlock->g_code10 == (uint16_t)G(63)));
}

static uint8_t IsCoordinateShift(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)((pBlock->g_code10 == (uint16_t)G(10)) ||
                     (pBlock->g_code10 == (uint16_t)G(52)) ||
                     (pBlock->g_code10 == (uint16_t)G(92)));
}

static void StoreTargets(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t axis;
    g_ncDesignFeatureStatus.last_g_code10 = pBlock->g_code10;
    g_ncDesignFeatureStatus.last_line_no = pBlock->line_no;
    g_ncDesignFeatureStatus.last_axis_mask = pBlock->axis_mask;
    g_ncDesignFeatureStatus.last_effective_feed = pBlock->feed;
    g_ncDesignFeatureStatus.last_spindle_speed = pBlock->spindle_speed;
    CopyAxes((int32_t*)g_ncDesignFeatureStatus.last_output_target,
             pBlock->axis_target);
    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        g_ncDesignFeatureStatus.last_following_error[axis] =
            pBlock->axis_target[axis] - pBlock->axis_start[axis];
        g_ncDesignFeatureStatus.last_clearance[axis] =
            NC_INTERFERENCE_AXIS_LIMIT - Abs32(pBlock->axis_target[axis]);
    }
}

static void MarkTs(uint32_t modeMask, NC_EXEC_BLOCK* pBlock)
{
    g_ncDesignFeatureStatus.active_mode_mask |= modeMask;
    g_ncDesignFeatureStatus.ts_adjusted_blocks++;
    StoreTargets(pBlock);
    g_ncDesignFeatureStatus.generation++;
}

static void ApplyFeedSlowdown(NC_EXEC_BLOCK* pBlock, uint16_t percent)
{
    if ((pBlock->feed > 0) && (percent > 0U) && (percent < 100U)) {
        pBlock->feed = (int32_t)(((int64_t)pBlock->feed * (int64_t)percent) / 100LL);
        if (pBlock->feed <= 0) {
            pBlock->feed = 1;
        }
        g_ncDesignFeatureStatus.slowdown_applied_blocks++;
        g_ncDesignFeatureStatus.active_override_percent = percent;
    }
}

static void ApplyCylindricalAxisHint(NC_EXEC_BLOCK* pBlock)
{
    if ((pBlock->axis_mask & (1UL << 3U)) != 0U) {
        int32_t virtualLinear = pBlock->axis_target[3] / 360;
        pBlock->axis_target[1] += virtualLinear;
        pBlock->axis_mask |= (1UL << 1U);
    }
}

static void ApplyHighPrecisionPreparation(NC_EXEC_BLOCK* pBlock)
{
    if (IsMotion(pBlock->motion_type) != 0U) {
        if (pBlock->interp_ticks < NC_DESIGN_ADVANCED_MIN_TICKS) {
            pBlock->interp_ticks = NC_DESIGN_ADVANCED_MIN_TICKS;
        }
        pBlock->accel_ticks += (pBlock->accel_ticks < 2U) ? 1U : 0U;
        pBlock->decel_ticks += (pBlock->decel_ticks < 2U) ? 1U : 0U;
        ApplyFeedSlowdown(pBlock, NC_DESIGN_HP_CONTOUR_PERCENT);
    }
}

static void ApplyRigidTapPreparation(NC_EXEC_BLOCK* pBlock)
{
    uint32_t rpm = pBlock->spindle_speed;
    if (rpm == 0U) {
        rpm = NC_TAP_MIN_SPINDLE_RPM;
        pBlock->spindle_speed = rpm;
    }
    if (pBlock->interp_ticks < NC_DESIGN_RIGID_TAP_MIN_TICKS) {
        pBlock->interp_ticks = NC_DESIGN_RIGID_TAP_MIN_TICKS;
    }
    g_ncDesignFeatureStatus.last_rigid_tap_pitch =
        (rpm == 0U) ? 0 : (int32_t)(pBlock->feed / (int32_t)rpm);
}

void NcDesignFeatures_Reset(void)
{
    (void)memset((void*)&g_ncDesignFeatureStatus, 0, sizeof(g_ncDesignFeatureStatus));
    s_autoCornerOverride = NC_DESIGN_AUTO_CORNER_OVERRIDE_PERCENT;
    g_ncDesignFeatureStatus.active_override_percent = s_autoCornerOverride;
}

int32_t NcDesignFeatures_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                      NC_ERROR_CODE* pOutError)
{
    uint32_t modeMask = 0U;
    (void)pOutError;

    if ((pBlock == 0) || (pOutError == 0)) {
        return -1;
    }
    CopyAxes((int32_t*)g_ncDesignFeatureStatus.last_input_target,
             pBlock->axis_target);

    if (pBlock->g_code10 == (uint16_t)GD(6, 2)) {
        g_ncDesignFeatureStatus.nurbs_blocks++;
        pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP;
        pBlock->motion_type = NC_MOTION_ADVANCED_INTERP;
        if (pBlock->interp_ticks < NC_DESIGN_ADVANCED_MIN_TICKS) {
            pBlock->interp_ticks = NC_DESIGN_ADVANCED_MIN_TICKS;
        }
        modeMask |= NC_DESIGN_MODE_NURBS;
    }
    if ((pBlock->g_code10 == (uint16_t)GD(2, 3)) ||
        (pBlock->g_code10 == (uint16_t)GD(3, 3))) {
        g_ncDesignFeatureStatus.exponential_blocks++;
        pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP;
        pBlock->motion_type = NC_MOTION_ADVANCED_INTERP;
        ApplyFeedSlowdown(pBlock, 90U);
        modeMask |= NC_DESIGN_MODE_EXPONENTIAL;
    }
    if (IsInvolute(pBlock) != 0U) {
        g_ncDesignFeatureStatus.involute_blocks++;
        pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP;
        pBlock->motion_type = NC_MOTION_ADVANCED_INTERP;
        if (pBlock->interp_ticks < NC_DESIGN_INVOLUTE_MIN_TICKS) {
            pBlock->interp_ticks = NC_DESIGN_INVOLUTE_MIN_TICKS;
        }
        ApplyFeedSlowdown(pBlock, 88U);
        modeMask |= NC_DESIGN_MODE_INVOLUTE;
    }
    if ((pBlock->g_code10 == (uint16_t)GD(7, 1)) ||
        (pBlock->g_code10 == (uint16_t)GD(12, 1))) {
        g_ncDesignFeatureStatus.cylindrical_active = 1U;
        g_ncDesignFeatureStatus.cylindrical_blocks++;
        ApplyCylindricalAxisHint(pBlock);
        modeMask |= NC_DESIGN_MODE_CYLINDRICAL;
    }
    if (pBlock->g_code10 == (uint16_t)GD(13, 1)) {
        g_ncDesignFeatureStatus.cylindrical_active = 0U;
        g_ncDesignFeatureStatus.cylindrical_blocks++;
        modeMask |= NC_DESIGN_MODE_CYLINDRICAL;
    }
    if (pBlock->g_code10 == (uint16_t)G(16)) {
        g_ncDesignFeatureStatus.polar_active = 1U;
        g_ncDesignFeatureStatus.polar_blocks++;
        modeMask |= NC_DESIGN_MODE_POLAR;
    } else if (pBlock->g_code10 == (uint16_t)G(15)) {
        g_ncDesignFeatureStatus.polar_active = 0U;
        g_ncDesignFeatureStatus.polar_blocks++;
        modeMask |= NC_DESIGN_MODE_POLAR;
    } else if ((g_ncDesignFeatureStatus.polar_active != 0U) &&
               (IsMotion(pBlock->motion_type) != 0U)) {
        g_ncDesignFeatureStatus.polar_blocks++;
        ApplyFeedSlowdown(pBlock, 95U);
        modeMask |= NC_DESIGN_MODE_POLAR;
    }
    if (IsHelicalB(pBlock) != 0U) {
        g_ncDesignFeatureStatus.helical_b_blocks++;
        pBlock->feature_flags |= NC_FEATURE_FLAG_ADVANCED_INTERP;
        if (pBlock->interp_ticks < NC_DESIGN_ADVANCED_MIN_TICKS) {
            pBlock->interp_ticks = NC_DESIGN_ADVANCED_MIN_TICKS;
        }
        modeMask |= NC_DESIGN_MODE_HELICAL_B;
    }
    if (pBlock->g_code10 == (uint16_t)G(62)) {
        g_ncDesignFeatureStatus.auto_corner_active = 1U;
        g_ncDesignFeatureStatus.auto_corner_override_blocks++;
        modeMask |= NC_DESIGN_MODE_AUTO_CORNER;
    } else if (pBlock->g_code10 == (uint16_t)G(64)) {
        g_ncDesignFeatureStatus.auto_corner_active = 0U;
        modeMask |= NC_DESIGN_MODE_AUTO_CORNER;
    } else if ((g_ncDesignFeatureStatus.auto_corner_active != 0U) &&
               (IsMotion(pBlock->motion_type) != 0U)) {
        g_ncDesignFeatureStatus.auto_corner_override_blocks++;
        ApplyFeedSlowdown(pBlock, s_autoCornerOverride);
        modeMask |= NC_DESIGN_MODE_AUTO_CORNER;
    }
    if (IsGrindingCycle(pBlock->cycle_code10) != 0U) {
        g_ncDesignFeatureStatus.grinding_cycle_blocks++;
        pBlock->feature_flags |= NC_FEATURE_FLAG_CANNED_CYCLE;
        ApplyFeedSlowdown(pBlock, 70U);
        modeMask |= NC_DESIGN_MODE_GRINDING;
    }
    if ((pBlock->motion_type == NC_MOTION_SKIP) &&
        ((pBlock->axis_mask & (1UL << 3U)) != 0U)) {
        g_ncDesignFeatureStatus.egb_skip_armed = 1U;
        g_ncDesignFeatureStatus.egb_skip_blocks++;
        modeMask |= NC_DESIGN_MODE_EGB_SKIP;
    }
    if (IsThreeDInterference(pBlock) != 0U) {
        g_ncDesignFeatureStatus.three_d_interference_blocks++;
        ApplyFeedSlowdown(pBlock, 85U);
        modeMask |= NC_DESIGN_MODE_3D_INTERFERENCE;
    }
    if (IsFollowupRollover(pBlock) != 0U) {
        g_ncDesignFeatureStatus.followup_rollover_blocks++;
        pBlock->feature_flags |= NC_FEATURE_FLAG_REFERENCE_RETURN;
        modeMask |= NC_DESIGN_MODE_FOLLOWUP_ROLLOVER;
    }
    if (IsHighPrecisionContour(pBlock) != 0U) {
        g_ncDesignFeatureStatus.high_precision_contour_blocks++;
        ApplyHighPrecisionPreparation(pBlock);
        modeMask |= NC_DESIGN_MODE_HP_CONTOUR;
    }
    if (IsExactStop(pBlock) != 0U) {
        g_ncDesignFeatureStatus.exact_stop_blocks++;
        if (IsMotion(pBlock->motion_type) != 0U) {
            ApplyFeedSlowdown(pBlock, NC_DESIGN_EXACT_STOP_PERCENT);
        }
        modeMask |= NC_DESIGN_MODE_EXACT_STOP;
    }
    if (IsRigidTap(pBlock) != 0U) {
        g_ncDesignFeatureStatus.rigid_tap_blocks++;
        ApplyRigidTapPreparation(pBlock);
        modeMask |= NC_DESIGN_MODE_RIGID_TAP;
    }
    if (IsThreadChamfer(pBlock) != 0U) {
        g_ncDesignFeatureStatus.thread_chamfer_blocks++;
        if (pBlock->decel_ticks < 2U) {
            pBlock->decel_ticks = 2U;
        }
        modeMask |= NC_DESIGN_MODE_THREAD_CHAMFER;
    }
    if (pBlock->g_code10 == (uint16_t)G(30)) {
        g_ncDesignFeatureStatus.second_reference_blocks++;
        pBlock->feature_flags |= NC_FEATURE_FLAG_REFERENCE_RETURN;
        modeMask |= NC_DESIGN_MODE_SECOND_REFERENCE;
    }
    if ((pBlock->modal_flags & NC_MODAL_FLAG_G53) != 0U) {
        g_ncDesignFeatureStatus.machine_direct_blocks++;
        modeMask |= NC_DESIGN_MODE_MACHINE_DIRECT;
    }
    if (IsCoordinateShift(pBlock) != 0U) {
        g_ncDesignFeatureStatus.coordinate_shift_blocks++;
        modeMask |= NC_DESIGN_MODE_COORD_SHIFT;
    }
    if ((pBlock->g_code10 == (uint16_t)G(96)) ||
        (pBlock->g_code10 == (uint16_t)G(97))) {
        g_ncDesignFeatureStatus.constant_surface_speed_blocks++;
        g_ncDesignFeatureStatus.last_css_diameter =
            (pBlock->axis_target[0] != 0) ? Abs32(pBlock->axis_target[0]) : NC_SPINDLE_CSS_MIN_DIAMETER;
        modeMask |= NC_DESIGN_MODE_CSS_SPINDLE;
    }
    if ((pBlock->feed_mode == NC_FEED_MODE_PER_REV) &&
        (IsMotion(pBlock->motion_type) != 0U)) {
        g_ncDesignFeatureStatus.feed_per_rev_blocks++;
        if ((pBlock->spindle_speed == 0U) && (g_ncSpindleStatus.output_rpm != 0U)) {
            pBlock->spindle_speed = g_ncSpindleStatus.output_rpm;
        }
        modeMask |= NC_DESIGN_MODE_FEED_PER_REV;
    }

    if (modeMask != 0U) {
        MarkTs(modeMask, pBlock);
    }
    return 0;
}

void NcDesignFeatures_OnBlockRt(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t modeMask = 0U;

    if (pBlock == 0) {
        return;
    }
    if (pBlock->g_code10 == (uint16_t)GD(6, 2)) {
        modeMask |= NC_DESIGN_MODE_NURBS;
    }
    if ((pBlock->g_code10 == (uint16_t)GD(2, 3)) ||
        (pBlock->g_code10 == (uint16_t)GD(3, 3))) {
        modeMask |= NC_DESIGN_MODE_EXPONENTIAL;
    }
    if (IsInvolute(pBlock) != 0U) {
        modeMask |= NC_DESIGN_MODE_INVOLUTE;
    }
    if ((pBlock->g_code10 == (uint16_t)GD(7, 1)) ||
        (pBlock->g_code10 == (uint16_t)GD(12, 1)) ||
        (pBlock->g_code10 == (uint16_t)GD(13, 1))) {
        modeMask |= NC_DESIGN_MODE_CYLINDRICAL;
    }
    if ((pBlock->g_code10 == (uint16_t)G(15)) ||
        (pBlock->g_code10 == (uint16_t)G(16)) ||
        (g_ncDesignFeatureStatus.polar_active != 0U)) {
        modeMask |= NC_DESIGN_MODE_POLAR;
    }
    if (IsHelicalB(pBlock) != 0U) {
        modeMask |= NC_DESIGN_MODE_HELICAL_B;
    }
    if ((pBlock->g_code10 == (uint16_t)G(62)) ||
        (g_ncDesignFeatureStatus.auto_corner_active != 0U)) {
        modeMask |= NC_DESIGN_MODE_AUTO_CORNER;
    }
    if (IsGrindingCycle(pBlock->cycle_code10) != 0U) {
        modeMask |= NC_DESIGN_MODE_GRINDING;
    }
    if ((pBlock->motion_type == NC_MOTION_SKIP) &&
        ((pBlock->axis_mask & (1UL << 3U)) != 0U)) {
        modeMask |= NC_DESIGN_MODE_EGB_SKIP;
    }
    if (IsThreeDInterference(pBlock) != 0U) {
        modeMask |= NC_DESIGN_MODE_3D_INTERFERENCE;
    }
    if (IsFollowupRollover(pBlock) != 0U) {
        modeMask |= NC_DESIGN_MODE_FOLLOWUP_ROLLOVER;
    }
    if (IsHighPrecisionContour(pBlock) != 0U) {
        modeMask |= NC_DESIGN_MODE_HP_CONTOUR;
    }
    if (IsExactStop(pBlock) != 0U) {
        modeMask |= NC_DESIGN_MODE_EXACT_STOP;
    }
    if (IsRigidTap(pBlock) != 0U) {
        modeMask |= NC_DESIGN_MODE_RIGID_TAP;
    }
    if (IsThreadChamfer(pBlock) != 0U) {
        modeMask |= NC_DESIGN_MODE_THREAD_CHAMFER;
    }
    if (pBlock->g_code10 == (uint16_t)G(30)) {
        modeMask |= NC_DESIGN_MODE_SECOND_REFERENCE;
    }
    if ((pBlock->modal_flags & NC_MODAL_FLAG_G53) != 0U) {
        modeMask |= NC_DESIGN_MODE_MACHINE_DIRECT;
    }
    if (IsCoordinateShift(pBlock) != 0U) {
        modeMask |= NC_DESIGN_MODE_COORD_SHIFT;
    }
    if ((pBlock->g_code10 == (uint16_t)G(96)) ||
        (pBlock->g_code10 == (uint16_t)G(97))) {
        modeMask |= NC_DESIGN_MODE_CSS_SPINDLE;
    }
    if ((pBlock->feed_mode == NC_FEED_MODE_PER_REV) &&
        (IsMotion(pBlock->motion_type) != 0U)) {
        modeMask |= NC_DESIGN_MODE_FEED_PER_REV;
    }
    if (modeMask != 0U) {
        g_ncDesignFeatureStatus.rt_executed_blocks++;
        StoreTargets(pBlock);
        g_ncDesignFeatureStatus.generation++;
        NcEvent_Push(NC_EVENT_CODE_DESIGN_FEATURE_APPLIED,
                     (int32_t)modeMask,
                     (int32_t)pBlock->g_code10,
                     pBlock->line_no);
    }
}

int32_t NcDesignFeatures_SetAutoCornerOverride(uint8_t enabled,
                                               uint16_t overridePercent)
{
    if (overridePercent > NC_FEED_OVERRIDE_MAX_PERCENT) {
        g_ncDesignFeatureStatus.generation++;
        return -1;
    }
    s_autoCornerOverride = overridePercent;
    g_ncDesignFeatureStatus.auto_corner_active = (enabled != 0U) ? 1U : 0U;
    g_ncDesignFeatureStatus.active_override_percent = overridePercent;
    g_ncDesignFeatureStatus.generation++;
    NcEvent_Push(NC_EVENT_CODE_DESIGN_FEATURE_APPLIED,
                 (int32_t)NC_DESIGN_MODE_AUTO_CORNER,
                 (int32_t)overridePercent,
                 0U);
    return 0;
}
