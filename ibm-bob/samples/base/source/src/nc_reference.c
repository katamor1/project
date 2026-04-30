/* ibm-bob/samples/base/source/src/nc_reference.c */
/* Implements reference return, one-direction positioning, reference mark, and rollover diagnostics. */
/* This exists so G28/G30/G60 and scale-reference design docs are represented in shared memory. */
/* RELEVANT FILES: inc/nc_reference.h, src/nc_program.c, src/rt_control.c */
#include <string.h>
#include "nc_reference.h"
#include "nc_program.h"

#define G(whole)       NC_G_CODE_WHOLE(whole)
#define GD(whole, dec) NC_G_CODE10((whole), (dec))

static int32_t s_referencePosition[AXIS_MAX];
static int32_t s_rolloverLimit[AXIS_MAX];
static int8_t  s_approachDirection[AXIS_MAX];
static int32_t s_oneDirectionApproach[AXIS_MAX];
static uint32_t s_distanceCodedMarkerMask;
static uint32_t s_prevReferenceSensorBits;

static uint32_t ValidAxisMask(uint32_t mask)
{
    return mask & ((1UL << AXIS_MAX) - 1UL);
}

static uint32_t ReferenceSensorMask(void)
{
    uint32_t mask = 0U;
    uint32_t axis;

    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        mask |= (SENSOR_REFERENCE_MARK_BASE_BIT << axis);
    }
    return mask;
}

static uint8_t IsReferenceReturnCode(uint32_t code10)
{
    return (uint8_t)((code10 == (uint32_t)G(28)) ||
                     (code10 == (uint32_t)G(30)) ||
                     (code10 == (uint32_t)GD(10, 6)) ||
                     (code10 == (uint32_t)GD(11, 6)));
}

static uint8_t IsOneDirectionCode(uint32_t code10)
{
    return (uint8_t)(code10 == (uint32_t)G(60));
}

static uint8_t IsMotionLike(NC_MOTION_TYPE motion)
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

void NcReference_Reset(void)
{
    uint32_t generation = g_ncReferenceStatus.generation;
    uint32_t axis;

    (void)memset((void*)&g_ncReferenceStatus, 0, sizeof(g_ncReferenceStatus));
    s_distanceCodedMarkerMask = 0U;
    s_prevReferenceSensorBits = g_ioIn.sensor_bits & ReferenceSensorMask();
    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        s_referencePosition[axis] = 0;
        s_rolloverLimit[axis] = NC_REFERENCE_DEFAULT_ROLLOVER_LIMIT;
        s_approachDirection[axis] = 1;
        s_oneDirectionApproach[axis] = NC_ONE_DIRECTION_APPROACH_DEFAULT;
        g_ncReferenceStatus.reference_position[axis] = s_referencePosition[axis];
        g_ncReferenceStatus.rollover_limit[axis] = s_rolloverLimit[axis];
        g_ncReferenceStatus.approach_direction[axis] = s_approachDirection[axis];
        g_ncReferenceStatus.one_direction_approach[axis] = s_oneDirectionApproach[axis];
    }
    g_ncReferenceStatus.generation = generation + 1U;
}

int32_t NcReference_SetAxisConfig(uint8_t axis,
                                  int32_t referencePosition,
                                  int8_t approachDirection,
                                  int32_t rolloverLimit,
                                  uint8_t distanceCodedMarker)
{
    if ((axis >= AXIS_MAX) ||
        ((approachDirection != 1) && (approachDirection != -1)) ||
        (rolloverLimit < 0)) {
        return -1;
    }

    s_referencePosition[axis] = referencePosition;
    s_approachDirection[axis] = approachDirection;
    s_rolloverLimit[axis] = rolloverLimit;
    if (distanceCodedMarker != 0U) {
        s_distanceCodedMarkerMask |= (1UL << axis);
    } else {
        s_distanceCodedMarkerMask &= ~(1UL << axis);
    }
    g_ncReferenceStatus.reference_position[axis] = referencePosition;
    g_ncReferenceStatus.approach_direction[axis] = approachDirection;
    g_ncReferenceStatus.rollover_limit[axis] = rolloverLimit;
    g_ncReferenceStatus.distance_coded_axis_mask = s_distanceCodedMarkerMask;
    g_ncReferenceStatus.generation++;
    return 0;
}

int32_t NcReference_SetOneDirectionApproach(uint8_t axis,
                                            int32_t approachAmount)
{
    if ((axis >= AXIS_MAX) || (approachAmount < 0)) {
        return -1;
    }
    s_oneDirectionApproach[axis] = approachAmount;
    g_ncReferenceStatus.one_direction_approach[axis] = approachAmount;
    g_ncReferenceStatus.generation++;
    return 0;
}

static void RecordAxisTargetExtremes(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t axis;

    if (IsMotionLike(pBlock->motion_type) == 0U) {
        return;
    }
    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        int32_t limit = s_rolloverLimit[axis];
        if (((pBlock->axis_mask & (1UL << axis)) != 0U) && (limit > 0) &&
            ((pBlock->axis_target[axis] >= limit) ||
             (pBlock->axis_target[axis] <= -limit))) {
            g_ncReferenceStatus.rollover_events++;
            g_ncReferenceStatus.last_rollover_axis = axis;
            g_ncReferenceStatus.last_rollover_position = pBlock->axis_target[axis];
            NcEvent_Push(NC_EVENT_CODE_REFERENCE_ROLLOVER,
                         (int32_t)axis,
                         pBlock->axis_target[axis],
                         pBlock->line_no);
        }
    }
}

void NcReference_OnParsedBlockTs(NC_EXEC_BLOCK* pBlock)
{
    uint32_t axisMask;
    uint32_t axis;

    if (pBlock == NULL) {
        return;
    }

    axisMask = ValidAxisMask(pBlock->axis_mask);
    if ((axisMask == 0U) && (IsReferenceReturnCode(pBlock->g_code10) != 0U)) {
        axisMask = ((1UL << AXIS_MAX) - 1UL);
    }

    if (IsReferenceReturnCode(pBlock->g_code10) != 0U) {
        g_ncReferenceStatus.planned_reference_return_blocks++;
        if (pBlock->g_code10 == (uint32_t)G(30)) {
            g_ncReferenceStatus.second_reference_return_blocks++;
        }
        g_ncReferenceStatus.requested_axis_mask |= axisMask;
        g_ncReferenceStatus.last_reference_code10 = pBlock->g_code10;
        pBlock->feature_flags |= NC_FEATURE_FLAG_REFERENCE_RETURN;
    }

    if (IsOneDirectionCode(pBlock->g_code10) != 0U) {
        g_ncReferenceStatus.one_direction_positioning_blocks++;
        g_ncReferenceStatus.last_one_direction_axis_mask = axisMask;
        for (axis = 0U; axis < AXIS_MAX; ++axis) {
            if ((axisMask & (1UL << axis)) != 0U) {
                int32_t approach = s_oneDirectionApproach[axis];
                int32_t direction = (int32_t)s_approachDirection[axis];
                g_ncReferenceStatus.last_approach_start[axis] =
                    pBlock->axis_target[axis] - (direction * approach);
            }
        }
    }

    RecordAxisTargetExtremes(pBlock);
    g_ncReferenceStatus.last_line_no = pBlock->line_no;
    g_ncReferenceStatus.generation++;
}

void NcReference_OnBlockRt(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t axisMask;
    uint32_t axis;

    if (pBlock == NULL) {
        return;
    }

    axisMask = ValidAxisMask(pBlock->axis_mask);
    if ((axisMask == 0U) && (IsReferenceReturnCode(pBlock->g_code10) != 0U)) {
        axisMask = ((1UL << AXIS_MAX) - 1UL);
    }

    if (IsReferenceReturnCode(pBlock->g_code10) != 0U) {
        g_ncReferenceStatus.active = 1U;
        g_ncReferenceStatus.executed_reference_return_blocks++;
        g_ncReferenceStatus.completed_axis_mask |= axisMask;
        g_ncReferenceStatus.homed_axis_mask |= axisMask;
        g_ncReferenceStatus.last_reference_code10 = pBlock->g_code10;
        for (axis = 0U; axis < AXIS_MAX; ++axis) {
            if ((axisMask & (1UL << axis)) != 0U) {
                g_ncReferenceStatus.last_reference_position[axis] =
                    s_referencePosition[axis];
            }
        }
        NcEvent_Push(NC_EVENT_CODE_REFERENCE_RETURN,
                     (int32_t)axisMask,
                     (int32_t)pBlock->g_code10,
                     pBlock->line_no);
        g_ncReferenceStatus.active = 0U;
    }

    if (IsOneDirectionCode(pBlock->g_code10) != 0U) {
        g_ncReferenceStatus.executed_one_direction_blocks++;
        NcEvent_Push(NC_EVENT_CODE_ONE_DIRECTION_POSITIONING,
                     (int32_t)axisMask,
                     (int32_t)pBlock->g_code10,
                     pBlock->line_no);
    }

    g_ncReferenceStatus.last_line_no = pBlock->line_no;
    g_ncReferenceStatus.generation++;
}

void NcReference_ServiceRt(void)
{
    uint32_t current = g_ioIn.sensor_bits & ReferenceSensorMask();
    uint32_t rising = current & ~s_prevReferenceSensorBits;
    uint32_t axis;

    if (rising != 0U) {
        for (axis = 0U; axis < AXIS_MAX; ++axis) {
            uint32_t bit = SENSOR_REFERENCE_MARK_BASE_BIT << axis;
            if ((rising & bit) != 0U) {
                uint32_t axisMask = (1UL << axis);
                g_ncReferenceStatus.reference_marker_axis_mask |= axisMask;
                g_ncReferenceStatus.reference_marker_captures++;
                g_ncReferenceStatus.last_marker_axis = axis;
                g_ncReferenceStatus.last_marker_position[axis] =
                    g_ioOut.axis_target[axis];
                if ((s_distanceCodedMarkerMask & axisMask) != 0U) {
                    g_ncReferenceStatus.distance_coded_marker_captures++;
                    g_ncReferenceStatus.distance_coded_captured_axis_mask |= axisMask;
                }
                NcEvent_Push(NC_EVENT_CODE_REFERENCE_MARK_CAPTURE,
                             (int32_t)axis,
                             g_ioOut.axis_target[axis],
                             g_ncProgramStatus.exec_line_no);
            }
        }
        g_ncReferenceStatus.generation++;
    }
    s_prevReferenceSensorBits = current;
}
