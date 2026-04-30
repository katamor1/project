/* ibm-bob/samples/base/source/src/nc_coordinate_transform.c */
/* Applies work mounting error, dynamic fixture, and rotary table offsets on TS side. */
/* This exists so RT receives already-corrected fixed-size NC execution blocks. */
/* RELEVANT FILES: inc/nc_coordinate_transform.h, src/nc_parser_modal.c, src/control_api.c */
#include <string.h>
#include "nc_coordinate.h"
#include "nc_coordinate_transform.h"
/**
 * @brief Return whether motion like is true for the current block or state.
 * @param motion Motion type being tested by the helper.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
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

/**
 * @brief Handle validate axis for this module.
 * @details This local helper has multiple return paths. The early returns keep validation and no-op cases explicit before the success path mutates shared state.
 * @param axis Axis index used by the helper.
 * @return 0 or a non-negative value on the accepted path; a negative value when validation fails or the requested item is absent.
 */
static int32_t ValidateAxis(uint8_t axis)
{
    if (axis >= AXIS_MAX) {
        g_ncCoordinateTransformStatus.api_rejected_calls++;
        g_ncCoordinateTransformStatus.generation++;
        return -1;
    }
    return 0;
}

/**
 * @brief Update active mask from current inputs.
 */
static void UpdateActiveMask(void)
{
    uint32_t i;
    uint32_t mask = 0U;

    for (i = 0U; i < AXIS_MAX; ++i) {
        if (((g_ncCoordinateTransformStatus.dynamic_fixture_enabled != 0U) &&
             (g_ncCoordinateTransformStatus.dynamic_fixture_offset[i] != 0)) ||
            ((g_ncCoordinateTransformStatus.work_mount_error_enabled != 0U) &&
             (g_ncCoordinateTransformStatus.work_mount_error[i] != 0)) ||
            ((g_ncCoordinateTransformStatus.rotary_fixture_enabled != 0U) &&
             (g_ncCoordinateTransformStatus.rotary_table_offset[i] != 0))) {
            mask |= (1UL << i);
        }
    }
    g_ncCoordinateTransformStatus.active_correction_mask = mask;
}

/**
 * @brief Handle nc coordinate transform reset for this module.
 */
void NcCoordinateTransform_Reset(void)
{
    uint8_t dynamicEnabled = g_ncCoordinateTransformStatus.dynamic_fixture_enabled;
    uint8_t mountEnabled = g_ncCoordinateTransformStatus.work_mount_error_enabled;
    uint8_t rotaryEnabled = g_ncCoordinateTransformStatus.rotary_fixture_enabled;

    (void)memset((void*)&g_ncCoordinateTransformStatus,
                 0,
                 sizeof(g_ncCoordinateTransformStatus));
    g_ncCoordinateTransformStatus.dynamic_fixture_enabled =
        (dynamicEnabled == 0U) ? 1U : dynamicEnabled;
    g_ncCoordinateTransformStatus.work_mount_error_enabled =
        (mountEnabled == 0U) ? 1U : mountEnabled;
    g_ncCoordinateTransformStatus.rotary_fixture_enabled =
        (rotaryEnabled == 0U) ? 1U : rotaryEnabled;
    g_ncCoordinateTransformStatus.generation++;
}

/**
 * @brief Handle nc coordinate transform set enabled for this module.
 * @param dynamicFixture Input value for dynamic fixture.
 * @param workMountError Input value for work mount error.
 * @param rotaryFixture Input value for rotary fixture.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetEnabled(uint8_t dynamicFixture,
                                         uint8_t workMountError,
                                         uint8_t rotaryFixture)
{
    g_ncCoordinateTransformStatus.dynamic_fixture_enabled =
        (uint8_t)(dynamicFixture != 0U);
    g_ncCoordinateTransformStatus.work_mount_error_enabled =
        (uint8_t)(workMountError != 0U);
    g_ncCoordinateTransformStatus.rotary_fixture_enabled =
        (uint8_t)(rotaryFixture != 0U);
    UpdateActiveMask();
    g_ncCoordinateTransformStatus.generation++;
    return 0;
}

/**
 * @brief Handle nc coordinate transform set work offset for this module.
 * @param workIndex Index identifying work.
 * @param axis Axis index used by the helper.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetWorkOffset(uint8_t workIndex,
                                            uint8_t axis,
                                            int32_t offset)
{
    if ((workIndex >= NC_WORK_COORD_SYSTEMS) || (ValidateAxis(axis) != 0)) {
        return -1;
    }
    if (NcCoordinate_SetWorkOffsetAxis(workIndex, axis, offset) != 0) {
        g_ncCoordinateTransformStatus.api_rejected_calls++;
        g_ncCoordinateTransformStatus.generation++;
        return -1;
    }
    g_ncCoordinateTransformStatus.work_offset_set_calls++;
    g_ncCoordinateTransformStatus.generation++;
    return 0;
}

/**
 * @brief Handle nc coordinate transform set local shift for this module.
 * @param axis Axis index used by the helper.
 * @param shift Input value for shift.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetLocalShift(uint8_t axis, int32_t shift)
{
    if (ValidateAxis(axis) != 0) {
        return -1;
    }
    if (NcCoordinate_SetLocalShiftAxis(axis, shift) != 0) {
        g_ncCoordinateTransformStatus.api_rejected_calls++;
        g_ncCoordinateTransformStatus.generation++;
        return -1;
    }
    g_ncCoordinateTransformStatus.local_shift_set_calls++;
    g_ncCoordinateTransformStatus.generation++;
    return 0;
}

/**
 * @brief Handle nc coordinate transform set temporary absolute for this module.
 * @param axis Axis index used by the helper.
 * @param programPosition Input value for program position.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetTemporaryAbsolute(uint8_t axis,
                                                   int32_t programPosition)
{
    if (ValidateAxis(axis) != 0) {
        return -1;
    }
    if (NcCoordinate_SetTemporaryAxis(axis, programPosition) != 0) {
        g_ncCoordinateTransformStatus.api_rejected_calls++;
        g_ncCoordinateTransformStatus.generation++;
        return -1;
    }
    g_ncCoordinateTransformStatus.temporary_absolute_set_calls++;
    g_ncCoordinateTransformStatus.generation++;
    return 0;
}

/**
 * @brief Handle nc coordinate transform set dynamic fixture offset for this module.
 * @param axis Axis index used by the helper.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetDynamicFixtureOffset(uint8_t axis,
                                                      int32_t offset)
{
    if (ValidateAxis(axis) != 0) {
        return -1;
    }
    g_ncCoordinateTransformStatus.dynamic_fixture_offset[axis] = offset;
    UpdateActiveMask();
    g_ncCoordinateTransformStatus.generation++;
    return 0;
}

/**
 * @brief Handle nc coordinate transform set work mount error for this module.
 * @param axis Axis index used by the helper.
 * @param error Following-error value recorded for diagnostics.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetWorkMountError(uint8_t axis, int32_t error)
{
    if (ValidateAxis(axis) != 0) {
        return -1;
    }
    g_ncCoordinateTransformStatus.work_mount_error[axis] = error;
    UpdateActiveMask();
    g_ncCoordinateTransformStatus.generation++;
    return 0;
}

/**
 * @brief Handle nc coordinate transform set rotary table offset for this module.
 * @param axis Axis index used by the helper.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_SetRotaryTableOffset(uint8_t axis,
                                                   int32_t offset)
{
    if (ValidateAxis(axis) != 0) {
        return -1;
    }
    g_ncCoordinateTransformStatus.rotary_table_offset[axis] = offset;
    UpdateActiveMask();
    g_ncCoordinateTransformStatus.generation++;
    return 0;
}

/**
 * @brief Handle nc coordinate transform apply block ts for this module.
 * @param pBlock NC execution block read or updated by the helper.
 * @param pOutError Output pointer that receives the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinateTransform_ApplyBlockTs(NC_EXEC_BLOCK* pBlock,
                                           NC_ERROR_CODE* pOutError)
{
    /* Prepare local state used by the following processing stage. */
    uint32_t i;
    uint32_t changedMask = 0U;

    if ((pBlock == 0) || (pOutError == 0)) {
        return -1;
    }
    if ((IsMotionLike(pBlock->motion_type) == 0U) || (pBlock->axis_mask == 0U)) {
        return 0;
    }

    /* Walk the fixed-size data set for this processing stage. */
    for (i = 0U; i < AXIS_MAX; ++i) {
        int32_t delta = 0;
        g_ncCoordinateTransformStatus.last_input_target[i] = pBlock->axis_target[i];
        if ((pBlock->axis_mask & (1UL << i)) == 0U) {
            g_ncCoordinateTransformStatus.last_corrected_target[i] = pBlock->axis_target[i];
            continue;
        }
        if (g_ncCoordinateTransformStatus.dynamic_fixture_enabled != 0U) {
            delta += g_ncCoordinateTransformStatus.dynamic_fixture_offset[i];
        }
        if (g_ncCoordinateTransformStatus.work_mount_error_enabled != 0U) {
            delta += g_ncCoordinateTransformStatus.work_mount_error[i];
        }
        if (g_ncCoordinateTransformStatus.rotary_fixture_enabled != 0U) {
            delta += g_ncCoordinateTransformStatus.rotary_table_offset[i];
        }
        if (delta != 0) {
            pBlock->axis_start[i] += delta;
            pBlock->axis_target[i] += delta;
            if ((pBlock->motion_type == NC_MOTION_ARC_CW) ||
                (pBlock->motion_type == NC_MOTION_ARC_CCW)) {
                pBlock->arc_center[i] += delta;
            }
            changedMask |= (1UL << i);
        }
        g_ncCoordinateTransformStatus.last_corrected_target[i] = pBlock->axis_target[i];
    }

    if (changedMask != 0U) {
        g_ncCoordinateTransformStatus.corrected_blocks++;
        if (g_ncCoordinateTransformStatus.dynamic_fixture_enabled != 0U) {
            g_ncCoordinateTransformStatus.dynamic_fixture_blocks++;
        }
        if (g_ncCoordinateTransformStatus.work_mount_error_enabled != 0U) {
            g_ncCoordinateTransformStatus.work_mount_error_blocks++;
        }
        if (g_ncCoordinateTransformStatus.rotary_fixture_enabled != 0U) {
            g_ncCoordinateTransformStatus.rotary_fixture_blocks++;
        }
        g_ncCoordinateTransformStatus.last_axis_mask = changedMask;
        g_ncCoordinateTransformStatus.last_line_no = pBlock->line_no;
        g_ncCoordinateTransformStatus.generation++;
    }
    (void)pOutError;
    return 0;
}
