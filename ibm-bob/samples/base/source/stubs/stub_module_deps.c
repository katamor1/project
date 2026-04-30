#include <string.h>
#include "system_shared.h"
#include "nc_program.h"

/*
 * Link-time stubs for modules that are not the focus of control_api unit tests.
 * The real modules are tested separately or at integration level. These stubs
 * keep public API tests isolated from unrelated heavy TS/NC behavior.
 */
int32_t NcProgram_RequestBinaryLoad(const NC_EXEC_BLOCK* pBlocks, uint32_t count)
{
    if ((pBlocks == NULL) || (count == 0U) || (count > NC_BINARY_PROGRAM_MAX_BLOCKS)) {
        return -1;
    }
    g_ncBinaryProgramStatus.loaded_blocks = count;
    
    g_ncBinaryProgramStatus.generation++;
    return 0;
}

int32_t NcFeed_SetOverride(uint16_t percent)
{
    if (percent > NC_FEED_OVERRIDE_MAX_PERCENT) {
        return -1;
    }
    g_ncFeedStatus.feed_override_percent = percent;
    g_ncFeedStatus.generation++;
    return 0;
}

int32_t NcCoordinateTransform_SetEnabled(uint8_t dynamicFixture, uint8_t workMountError, uint8_t rotaryFixture)
{
    g_ncCoordinateTransformStatus.dynamic_fixture_enabled = (uint8_t)(dynamicFixture != 0U);
    g_ncCoordinateTransformStatus.work_mount_error_enabled = (uint8_t)(workMountError != 0U);
    g_ncCoordinateTransformStatus.rotary_fixture_enabled = (uint8_t)(rotaryFixture != 0U);
    g_ncCoordinateTransformStatus.generation++;
    return 0;
}

int32_t NcCoordinateTransform_SetWorkOffset(uint8_t workIndex, uint8_t axis, int32_t offset)
{
    (void)offset;
    return ((workIndex < NC_WORK_COORD_SYSTEMS) && (axis < AXIS_MAX)) ? 0 : -1;
}
int32_t NcCoordinateTransform_SetLocalShift(uint8_t axis, int32_t shift)
{
    (void)shift;
    return (axis < AXIS_MAX) ? 0 : -1;
}
int32_t NcCoordinateTransform_SetTemporaryAbsolute(uint8_t axis, int32_t programPosition)
{
    (void)programPosition;
    return (axis < AXIS_MAX) ? 0 : -1;
}
int32_t NcCoordinateTransform_SetDynamicFixtureOffset(uint8_t axis, int32_t offset)
{
    (void)offset;
    return (axis < AXIS_MAX) ? 0 : -1;
}
int32_t NcCoordinateTransform_SetWorkMountError(uint8_t axis, int32_t error)
{
    (void)error;
    return (axis < AXIS_MAX) ? 0 : -1;
}
int32_t NcCoordinateTransform_SetRotaryTableOffset(uint8_t axis, int32_t offset)
{
    (void)offset;
    return (axis < AXIS_MAX) ? 0 : -1;
}

int32_t NcCompensation_SetToolLengthOffset(uint16_t offsetNo, int32_t offset)
{
    (void)offset;
    return (offsetNo == 0U) ? -1 : 0;
}
int32_t NcCompensation_SetCutterRadiusOffset(uint16_t offsetNo, int32_t offset)
{
    (void)offset;
    return (offsetNo == 0U) ? -1 : 0;
}

int32_t NcKinematics_SetAxisAssignment(uint8_t logicalAxis, uint8_t physicalAxis, int8_t sign, uint8_t detached)
{
    (void)sign;
    (void)detached;
    return ((logicalAxis < AXIS_MAX) && (physicalAxis < AXIS_MAX)) ? 0 : -1;
}
int32_t NcKinematics_SetMirrorMask(uint32_t axisMask)
{
    g_ncKinematicsStatus.mirror_axis_mask = axisMask;
    return 0;
}

int32_t NcInterference_SetEnabled(uint8_t enabled)
{
    g_ncInterferenceStatus.enabled = (uint8_t)(enabled != 0U);
    return 0;
}

int32_t NcLatheCycle_SetDiameterMode(uint8_t enabled)
{
    g_ncTurningCycleStatus.radius_mode_active = (uint8_t)(enabled == 0U);
    return 0;
}

int32_t NcReference_SetAxisConfig(uint8_t axis,
                                  int32_t referencePosition,
                                  int8_t approachDirection,
                                  int32_t rolloverLimit,
                                  uint8_t distanceCodedMarker)
{
    (void)referencePosition;
    (void)approachDirection;
    (void)rolloverLimit;
    (void)distanceCodedMarker;
    return (axis < AXIS_MAX) ? 0 : -1;
}
int32_t NcReference_SetOneDirectionApproach(uint8_t axis, int32_t approachAmount)
{
    (void)approachAmount;
    return (axis < AXIS_MAX) ? 0 : -1;
}

int32_t NcPrecision_SetLearningControl(uint8_t enabled, int32_t gainPercent, uint8_t memoryWindow)
{
    (void)enabled;
    (void)gainPercent;
    (void)memoryWindow;
    return 0;
}
int32_t NcPrecision_SetVibrationControl(uint8_t enabled, uint16_t notchFreqHz, uint16_t dampingPercent)
{
    (void)enabled;
    (void)notchFreqHz;
    (void)dampingPercent;
    return 0;
}
int32_t NcPrecision_SetPreviewControl(uint8_t enabled, uint16_t lookaheadBlocks, uint16_t cornerTolerance)
{
    (void)enabled;
    (void)lookaheadBlocks;
    (void)cornerTolerance;
    return 0;
}

int32_t NcSpindle_SetSpeedLimit(uint32_t minRpm, uint32_t maxRpm)
{
    if (minRpm > maxRpm) {
        return -1;
    }
    g_ncSpindleStatus.min_rpm = minRpm;
    g_ncSpindleStatus.max_rpm = maxRpm;
    return 0;
}
int32_t NcSpindle_SetOrientWindow(uint16_t timeoutTicks)
{
    g_ncSpindleStatus.orient_timeout_ticks = timeoutTicks;
    return 0;
}

int32_t NcToolManagement_SetPocket(uint32_t toolNo, uint32_t pocketNo)
{
    (void)toolNo;
    (void)pocketNo;
    return 0;
}
int32_t NcToolManagement_RequestPrepare(uint32_t toolNo)
{
    g_ncToolManagementStatus.requested_tool_no = toolNo;
    return 0;
}

int32_t NcSynchronization_SetMasterSlave(uint8_t masterAxis, uint32_t slaveAxisMask)
{
    g_ncSynchronizationStatus.master_axis = masterAxis;
    g_ncSynchronizationStatus.slave_axis_mask = slaveAxisMask;
    return (masterAxis < AXIS_MAX) ? 0 : -1;
}
int32_t NcSynchronization_SetOverlayAxis(uint8_t axis, int32_t offset)
{
    (void)offset;
    return (axis < AXIS_MAX) ? 0 : -1;
}
int32_t NcSynchronization_SetCompoundPathMask(uint32_t pathMask)
{
    g_ncSynchronizationStatus.compound_path_mask = pathMask;
    return 0;
}
int32_t NcSynchronization_SetDoubleTable(uint8_t enabled, uint32_t slaveAxisMask)
{
    g_ncSynchronizationStatus.double_table_enabled = (uint8_t)(enabled != 0U);
    g_ncSynchronizationStatus.slave_axis_mask = slaveAxisMask;
    return 0;
}

int32_t NcRotaryMcc_SetAxisRadius(uint8_t axis, int32_t radius)
{
    if (axis >= AXIS_MAX) {
        return -1;
    }
    g_ncRotaryMccStatus.radius[axis] = radius;
    return 0;
}
int32_t NcRotaryMcc_SetMccOutput(uint8_t enabled)
{
    g_ncRotaryMccStatus.mcc_output_enabled = (uint8_t)(enabled != 0U);
    return 0;
}

int32_t NcAxisConfig_SetAxisDefinition(uint8_t axis, uint8_t axisName, uint8_t axisType, uint8_t coordinateGroup)
{
    (void)axisName;
    (void)axisType;
    (void)coordinateGroup;
    return (axis < AXIS_MAX) ? 0 : -1;
}
int32_t NcAxisConfig_SetPathAxisMask(uint32_t axisMask)
{
    g_ncAxisConfigStatus.active_path_axis_mask = axisMask;
    return (axisMask == 0U) ? -1 : 0;
}
int32_t NcAxisConfig_SetDetachedMask(uint32_t axisMask)
{
    g_ncAxisConfigStatus.detached_axis_mask = axisMask;
    return 0;
}
int32_t NcAxisConfig_SetDiameterMode(uint8_t axis, uint8_t enabled)
{
    uint32_t bit;
    if (axis >= AXIS_MAX) {
        return -1;
    }
    bit = (1UL << axis);
    if (enabled != 0U) {
        g_ncAxisConfigStatus.diameter_axis_mask |= bit;
    } else {
        g_ncAxisConfigStatus.diameter_axis_mask &= ~bit;
    }
    return 0;
}

int32_t NcDiagnostics_SetToolLifeLimit(uint16_t toolNo, uint32_t limit)
{
    if ((toolNo == 0U) || (toolNo >= NC_TOOL_LIFE_TABLE_SIZE) || (limit == 0U)) {
        return -1;
    }
    g_ncToolLifeStatus.tool_life_limit[toolNo] = limit;
    return 0;
}
