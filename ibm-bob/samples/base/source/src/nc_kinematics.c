/* ibm-bob/samples/base/source/src/nc_kinematics.c */
/* Applies TS-side tilted work plane, tool axis, mirror, axis allocation, and retract state. */
/* This exists so RT stays deterministic and only consumes fixed execution blocks. */
/* RELEVANT FILES: inc/nc_kinematics.h, src/nc_parser_modal.c, src/nc_program.c */
#include <math.h>
#include <string.h>
#include "nc_kinematics.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

static int32_t RoundFixed(double value)
{
    return (int32_t)(value + ((value >= 0.0) ? 0.5 : -0.5));
}

static double AngleRad(int32_t scaledDegree)
{
    return (((double)scaledDegree / (double)NC_ANGLE_SCALE) * M_PI) / 180.0;
}

static void SetIdentityAxisMap(void)
{
    uint8_t i;

    g_ncKinematicsStatus.logical_axis_mask = 0U;
    g_ncKinematicsStatus.physical_axis_mask = 0U;
    for (i = 0U; i < AXIS_MAX; ++i) {
        g_ncKinematicsStatus.axis_map[i] = (int8_t)i;
        g_ncKinematicsStatus.axis_sign[i] = 1;
        g_ncKinematicsStatus.logical_axis_mask |= (1UL << i);
        g_ncKinematicsStatus.physical_axis_mask |= (1UL << i);
    }
}

void NcKinematics_Reset(void)
{
    NC_KINEMATICS_STATUS previous;
    uint32_t generation;

    (void)memcpy(&previous,
                 (const void*)&g_ncKinematicsStatus,
                 sizeof(previous));
    generation = previous.generation;
    (void)memset((void*)&g_ncKinematicsStatus, 0, sizeof(g_ncKinematicsStatus));
    if (previous.logical_axis_mask != 0U) {
        uint8_t i;
        for (i = 0U; i < AXIS_MAX; ++i) {
            g_ncKinematicsStatus.axis_map[i] = previous.axis_map[i];
            g_ncKinematicsStatus.axis_sign[i] =
                (previous.axis_sign[i] == -1) ? -1 : 1;
        }
        g_ncKinematicsStatus.logical_axis_mask = previous.logical_axis_mask;
        g_ncKinematicsStatus.physical_axis_mask = previous.physical_axis_mask;
        g_ncKinematicsStatus.detached_axis_mask = previous.detached_axis_mask;
        g_ncKinematicsStatus.mirror_axis_mask = previous.mirror_axis_mask;
    } else {
        SetIdentityAxisMap();
    }
    g_ncKinematicsStatus.generation = generation + 1U;
}

int32_t NcKinematics_SetAxisAssignment(uint8_t logicalAxis,
                                       uint8_t physicalAxis,
                                       int8_t sign,
                                       uint8_t detached)
{
    if ((logicalAxis >= AXIS_MAX) || (physicalAxis >= AXIS_MAX) ||
        ((sign != 1) && (sign != -1))) {
        return -1;
    }

    g_ncKinematicsStatus.axis_map[logicalAxis] = (int8_t)physicalAxis;
    g_ncKinematicsStatus.axis_sign[logicalAxis] = sign;
    if (detached != 0U) {
        g_ncKinematicsStatus.detached_axis_mask |= (1UL << logicalAxis);
    } else {
        g_ncKinematicsStatus.detached_axis_mask &= ~(1UL << logicalAxis);
    }
    g_ncKinematicsStatus.logical_axis_mask |= (1UL << logicalAxis);
    g_ncKinematicsStatus.physical_axis_mask |= (1UL << physicalAxis);
    g_ncKinematicsStatus.generation++;
    return 0;
}

int32_t NcKinematics_SetMirrorMask(uint32_t axisMask)
{
    if ((axisMask & ~((1UL << AXIS_MAX) - 1UL)) != 0U) {
        return -1;
    }
    g_ncKinematicsStatus.mirror_axis_mask = axisMask;
    g_ncKinematicsStatus.generation++;
    return 0;
}

static void ApplyTiltCommand(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t i;

    if (pBlock->kinematics_command == NC_KINEMATICS_CMD_TILT_CANCEL) {
        g_ncKinematicsStatus.tilted_plane_active = 0U;
        for (i = 0U; i < 3U; ++i) {
            g_ncKinematicsStatus.tilt_angle_deg[i] = 0;
        }
        g_ncKinematicsStatus.generation++;
        return;
    }
    if (pBlock->kinematics_command != NC_KINEMATICS_CMD_TILT_ENABLE) {
        return;
    }

    g_ncKinematicsStatus.tilted_plane_active = 1U;
    for (i = 0U; i < 3U; ++i) {
        g_ncKinematicsStatus.tilt_origin[i] = pBlock->tilt_origin[i];
        g_ncKinematicsStatus.tilt_angle_deg[i] = pBlock->tilt_angle_deg[i];
    }
    if ((g_ncKinematicsStatus.tilt_angle_deg[1] > (85 * NC_ANGLE_SCALE)) ||
        (g_ncKinematicsStatus.tilt_angle_deg[1] < (-85 * NC_ANGLE_SCALE))) {
        g_ncKinematicsStatus.singularity_warnings++;
    }
    g_ncKinematicsStatus.generation++;
}

static void ApplyToolAxisCommand(const NC_EXEC_BLOCK* pBlock)
{
    double a;
    double b;
    double ca;
    double sa;
    double cb;
    double sb;
    uint32_t i;

    if (pBlock->tool_axis_command != NC_TOOL_AXIS_CMD_ENABLE) {
        return;
    }

    g_ncKinematicsStatus.tool_axis_active = 1U;
    for (i = 0U; i < 3U; ++i) {
        g_ncKinematicsStatus.tool_axis_angle_deg[i] = pBlock->tool_axis_angle_deg[i];
    }

    a = AngleRad(pBlock->tool_axis_angle_deg[0]);
    b = AngleRad(pBlock->tool_axis_angle_deg[1]);
    ca = cos(a);
    sa = sin(a);
    cb = cos(b);
    sb = sin(b);

    g_ncKinematicsStatus.tool_axis_vector[0] = RoundFixed(sb * (double)NC_POSITION_SCALE);
    g_ncKinematicsStatus.tool_axis_vector[1] = RoundFixed((-sa * cb) * (double)NC_POSITION_SCALE);
    g_ncKinematicsStatus.tool_axis_vector[2] = RoundFixed((ca * cb) * (double)NC_POSITION_SCALE);
    if ((pBlock->tool_axis_angle_deg[1] > (85 * NC_ANGLE_SCALE)) ||
        (pBlock->tool_axis_angle_deg[1] < (-85 * NC_ANGLE_SCALE))) {
        g_ncKinematicsStatus.singularity_warnings++;
    }
    g_ncKinematicsStatus.generation++;
}

static void ApplyMirrorCommand(const NC_EXEC_BLOCK* pBlock)
{
    if (pBlock->mirror_command == NC_MIRROR_CMD_CANCEL) {
        g_ncKinematicsStatus.mirror_axis_mask = 0U;
        g_ncKinematicsStatus.generation++;
    } else if (pBlock->mirror_command == NC_MIRROR_CMD_ENABLE) {
        g_ncKinematicsStatus.mirror_axis_mask = pBlock->mirror_axis_mask;
        g_ncKinematicsStatus.generation++;
    }
}

static void ApplyRetractCommand(const NC_EXEC_BLOCK* pBlock)
{
    if (pBlock->axis_retract_command == NC_AXIS_RETRACT_CMD_RETRACT) {
        g_ncKinematicsStatus.retract_axis_mask |= pBlock->axis_mask;
        g_ncKinematicsStatus.retract_events++;
        g_ncKinematicsStatus.generation++;
    } else if (pBlock->axis_retract_command == NC_AXIS_RETRACT_CMD_RECOVER) {
        g_ncKinematicsStatus.retract_axis_mask &= ~pBlock->axis_mask;
        g_ncKinematicsStatus.retract_events++;
        g_ncKinematicsStatus.generation++;
    }
}

static void RotateTiltedTarget(NC_EXEC_BLOCK* pBlock)
{
    double x;
    double y;
    double z;
    double cx;
    double cy;
    double cz;
    double a;
    double b;
    double c;
    double ca;
    double cb;
    double cc;
    double sa;
    double sb;
    double sc;
    double x1;
    double y1;
    double z1;
    double x2;
    double y2;
    double z2;

    if ((g_ncKinematicsStatus.tilted_plane_active == 0U) ||
        (IsMotionLike(pBlock->motion_type) == 0U) ||
        ((pBlock->axis_mask & 0x7U) == 0U)) {
        return;
    }

    cx = (double)g_ncKinematicsStatus.tilt_origin[0];
    cy = (double)g_ncKinematicsStatus.tilt_origin[1];
    cz = (double)g_ncKinematicsStatus.tilt_origin[2];
    x = (double)pBlock->axis_target[0] - cx;
    y = (double)pBlock->axis_target[1] - cy;
    z = (double)pBlock->axis_target[2] - cz;

    a = AngleRad(g_ncKinematicsStatus.tilt_angle_deg[0]);
    b = AngleRad(g_ncKinematicsStatus.tilt_angle_deg[1]);
    c = AngleRad(g_ncKinematicsStatus.tilt_angle_deg[2]);
    ca = cos(a); sa = sin(a);
    cb = cos(b); sb = sin(b);
    cc = cos(c); sc = sin(c);

    x1 = x;
    y1 = (y * ca) - (z * sa);
    z1 = (y * sa) + (z * ca);

    x2 = (x1 * cb) + (z1 * sb);
    y2 = y1;
    z2 = (-x1 * sb) + (z1 * cb);

    pBlock->axis_target[0] = RoundFixed(cx + (x2 * cc) - (y2 * sc));
    pBlock->axis_target[1] = RoundFixed(cy + (x2 * sc) + (y2 * cc));
    pBlock->axis_target[2] = RoundFixed(cz + z2);
    pBlock->axis_mask |= 0x7U;
    pBlock->feature_flags |= NC_FEATURE_FLAG_TILTED_PLANE;
}

static void ApplyMirrorToTarget(NC_EXEC_BLOCK* pBlock)
{
    uint32_t i;
    uint32_t mask = g_ncKinematicsStatus.mirror_axis_mask;

    if ((mask == 0U) || (IsMotionLike(pBlock->motion_type) == 0U)) {
        return;
    }

    for (i = 0U; i < AXIS_MAX; ++i) {
        if (((mask & (1UL << i)) != 0U) &&
            ((pBlock->axis_mask & (1UL << i)) != 0U)) {
            int32_t center = (i < 3U) ? g_ncKinematicsStatus.tilt_origin[i] : 0;
            pBlock->axis_target[i] = (center * 2) - pBlock->axis_target[i];
            pBlock->feature_flags |= NC_FEATURE_FLAG_MIRROR;
        }
    }
}

static void ApplyDetachedAxes(NC_EXEC_BLOCK* pBlock)
{
    uint32_t i;

    if ((g_ncKinematicsStatus.detached_axis_mask == 0U) ||
        (IsMotionLike(pBlock->motion_type) == 0U)) {
        return;
    }

    for (i = 0U; i < AXIS_MAX; ++i) {
        if ((g_ncKinematicsStatus.detached_axis_mask & (1UL << i)) != 0U) {
            pBlock->axis_target[i] = pBlock->axis_start[i];
            pBlock->axis_mask &= ~(1UL << i);
            pBlock->feature_flags |= NC_FEATURE_FLAG_AXIS_ASSIGNMENT;
        }
    }
}

static void ApplyAxisMap(NC_EXEC_BLOCK* pBlock)
{
    int32_t mappedTarget[AXIS_MAX];
    int32_t mappedStart[AXIS_MAX];
    uint32_t mappedMask = 0U;
    uint32_t i;

    if (IsMotionLike(pBlock->motion_type) == 0U) {
        return;
    }

    for (i = 0U; i < AXIS_MAX; ++i) {
        mappedTarget[i] = pBlock->axis_target[i];
        mappedStart[i] = pBlock->axis_start[i];
    }

    for (i = 0U; i < AXIS_MAX; ++i) {
        uint8_t physical = (uint8_t)g_ncKinematicsStatus.axis_map[i];
        int8_t sign = g_ncKinematicsStatus.axis_sign[i];
        if (physical >= AXIS_MAX) {
            continue;
        }
        mappedTarget[physical] = (int32_t)sign * pBlock->axis_target[i];
        mappedStart[physical] = (int32_t)sign * pBlock->axis_start[i];
        if ((pBlock->axis_mask & (1UL << i)) != 0U) {
            mappedMask |= (1UL << physical);
        }
    }

    for (i = 0U; i < AXIS_MAX; ++i) {
        pBlock->axis_target[i] = mappedTarget[i];
        pBlock->axis_start[i] = mappedStart[i];
    }
    pBlock->axis_mask = mappedMask;
}

int32_t NcKinematics_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                  NC_ERROR_CODE* pOutError)
{
    int32_t before[AXIS_MAX];
    uint32_t i;
    uint8_t changed = 0U;

    if ((pBlock == 0) || (pOutError == 0)) {
        return -1;
    }

    for (i = 0U; i < AXIS_MAX; ++i) {
        before[i] = pBlock->axis_target[i];
    }

    ApplyTiltCommand(pBlock);
    ApplyToolAxisCommand(pBlock);
    ApplyMirrorCommand(pBlock);
    ApplyRetractCommand(pBlock);

    ApplyDetachedAxes(pBlock);
    RotateTiltedTarget(pBlock);
    ApplyMirrorToTarget(pBlock);
    ApplyAxisMap(pBlock);

    for (i = 0U; i < AXIS_MAX; ++i) {
        g_ncKinematicsStatus.last_transformed_target[i] = pBlock->axis_target[i];
        if (before[i] != pBlock->axis_target[i]) {
            changed = 1U;
        }
    }

    if (changed != 0U) {
        g_ncKinematicsStatus.transformed_blocks++;
        if ((pBlock->feature_flags & NC_FEATURE_FLAG_MIRROR) != 0U) {
            g_ncKinematicsStatus.mirrored_blocks++;
        }
        g_ncKinematicsStatus.last_line_no = pBlock->line_no;
        g_ncKinematicsStatus.generation++;
    }

    (void)pOutError;
    return 0;
}
