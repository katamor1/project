/* ibm-bob/samples/base/source/src/nc_coordinate.c */
/* Converts NC program, work, local, temporary, and machine coordinates. */
/* This exists so TS parsing normalizes coordinates before RT consumes blocks. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_coordinate.h, ibm-bob/samples/base/source/src/nc_parser_modal.c, ibm-bob/samples/base/source/inc/system_shared.h */
#include <string.h>
#include "nc_coordinate.h"
/**
 * @brief Handle active offset axis for this module.
 * @param axis Axis index used by the helper.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
static int32_t ActiveOffsetAxis(uint32_t axis)
{
    uint8_t work = g_ncCoordinateState.selected_work_index;

    return (g_ncCoordinateState.work_offset[work][axis] +
            g_ncCoordinateState.local_shift[axis] +
            g_ncCoordinateState.temporary_shift[axis]);
}

/**
 * @brief Build machine axis from current shared state.
 * @param axis Axis index used by the helper.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
static int32_t BuildMachineAxis(uint32_t axis)
{
    return g_ncCoordinateState.program_position[axis] +
           ActiveOffsetAxis(axis);
}

/**
 * @brief Handle refresh machine positions for this module.
 */
static void RefreshMachinePositions(void)
{
    uint32_t i;

    for (i = 0U; i < AXIS_MAX; ++i) {
        g_ncCoordinateState.machine_position[i] = BuildMachineAxis(i);
    }
    g_ncCoordinateState.generation++;
}

/**
 * @brief Handle clear temporary shift for this module.
 */
static void ClearTemporaryShift(void)
{
    (void)memset((void*)g_ncCoordinateState.temporary_shift,
                 0,
                 sizeof(g_ncCoordinateState.temporary_shift));
    g_ncCoordinateState.active_temporary_mask = 0U;
}

/**
 * @brief Handle nc coordinate reset for this module.
 */
void NcCoordinate_Reset(void)
{
    (void)memset((void*)&g_ncCoordinateState, 0, sizeof(g_ncCoordinateState));
    g_ncCoordinateState.unit_mode = NC_UNIT_MM;
    g_ncCoordinateState.distance_mode = NC_DISTANCE_ABSOLUTE;
    g_ncCoordinateState.selected_work_index = 0U;
    RefreshMachinePositions();
}

/**
 * @brief Handle nc coordinate set unit mode for this module.
 * @param mode Input value for mode.
 */
void NcCoordinate_SetUnitMode(NC_UNIT_MODE mode)
{
    g_ncCoordinateState.unit_mode = mode;
    g_ncCoordinateState.generation++;
}

/**
 * @brief Handle nc coordinate set distance mode for this module.
 * @param mode Input value for mode.
 */
void NcCoordinate_SetDistanceMode(NC_DISTANCE_MODE mode)
{
    g_ncCoordinateState.distance_mode = mode;
    g_ncCoordinateState.generation++;
}

/**
 * @brief Handle nc coordinate select work for this module.
 * @param workIndex Index identifying work.
 */
void NcCoordinate_SelectWork(uint8_t workIndex)
{
    if (workIndex >= NC_WORK_COORD_SYSTEMS) {
        return;
    }
    g_ncCoordinateState.selected_work_index = workIndex;
    NcCoordinate_ClearLocalShift();
    ClearTemporaryShift();
    RefreshMachinePositions();
}

/**
 * @brief Handle nc coordinate clear local shift for this module.
 */
void NcCoordinate_ClearLocalShift(void)
{
    (void)memset((void*)g_ncCoordinateState.local_shift,
                 0,
                 sizeof(g_ncCoordinateState.local_shift));
    g_ncCoordinateState.active_local_mask = 0U;
    RefreshMachinePositions();
}

/**
 * @brief Handle nc coordinate set local shift axis for this module.
 * @param axisIndex Axis index selected by the parsed address word.
 * @param shift Input value for shift.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinate_SetLocalShiftAxis(int32_t axisIndex, int32_t shift)
{
    if ((axisIndex < 0) || (axisIndex >= AXIS_MAX)) {
        return -1;
    }
    g_ncCoordinateState.local_shift[axisIndex] = shift;
    if (shift == 0) {
        g_ncCoordinateState.active_local_mask &= ~(1UL << (uint32_t)axisIndex);
    } else {
        g_ncCoordinateState.active_local_mask |= (1UL << (uint32_t)axisIndex);
    }
    RefreshMachinePositions();
    return 0;
}

/**
 * @brief Handle nc coordinate set work offset axis for this module.
 * @param workIndex Index identifying work.
 * @param axisIndex Axis index selected by the parsed address word.
 * @param offset Input value for offset.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinate_SetWorkOffsetAxis(uint8_t workIndex,
                                       int32_t axisIndex,
                                       int32_t offset)
{
    if ((workIndex >= NC_WORK_COORD_SYSTEMS) ||
        (axisIndex < 0) || (axisIndex >= AXIS_MAX)) {
        return -1;
    }
    g_ncCoordinateState.work_offset[workIndex][axisIndex] = offset;
    RefreshMachinePositions();
    return 0;
}

/**
 * @brief Handle nc coordinate set program axis for this module.
 * @param axisIndex Axis index selected by the parsed address word.
 * @param programPosition Input value for program position.
 * @param pBlock NC execution block read or updated by the helper.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinate_SetProgramAxis(int32_t axisIndex,
                                    int32_t programPosition,
                                    NC_EXEC_BLOCK* pBlock)
{
    if ((axisIndex < 0) || (axisIndex >= AXIS_MAX) || (pBlock == 0)) {
        return -1;
    }
    if (g_ncCoordinateState.distance_mode == NC_DISTANCE_INCREMENTAL) {
        programPosition += g_ncCoordinateState.program_position[axisIndex];
    }
    g_ncCoordinateState.program_position[axisIndex] = programPosition;
    RefreshMachinePositions();
    pBlock->axis_target[axisIndex] =
        g_ncCoordinateState.machine_position[axisIndex];
    pBlock->axis_mask |= (1UL << (uint32_t)axisIndex);
    return 0;
}

/**
 * @brief Handle nc coordinate set machine axis for this module.
 * @param axisIndex Axis index selected by the parsed address word.
 * @param machinePosition Input value for machine position.
 * @param pBlock NC execution block read or updated by the helper.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinate_SetMachineAxis(int32_t axisIndex,
                                    int32_t machinePosition,
                                    NC_EXEC_BLOCK* pBlock)
{
    if ((axisIndex < 0) || (axisIndex >= AXIS_MAX) || (pBlock == 0)) {
        return -1;
    }
    g_ncCoordinateState.machine_position[axisIndex] = machinePosition;
    g_ncCoordinateState.program_position[axisIndex] =
        machinePosition - ActiveOffsetAxis((uint32_t)axisIndex);
    g_ncCoordinateState.generation++;
    pBlock->axis_target[axisIndex] = machinePosition;
    pBlock->axis_mask |= (1UL << (uint32_t)axisIndex);
    return 0;
}

/**
 * @brief Handle nc coordinate set temporary axis for this module.
 * @param axisIndex Axis index selected by the parsed address word.
 * @param programPosition Input value for program position.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCoordinate_SetTemporaryAxis(int32_t axisIndex,
                                      int32_t programPosition)
{
    int32_t currentMachine;

    if ((axisIndex < 0) || (axisIndex >= AXIS_MAX)) {
        return -1;
    }
    currentMachine = g_ncCoordinateState.machine_position[axisIndex];
    g_ncCoordinateState.program_position[axisIndex] = programPosition;
    g_ncCoordinateState.temporary_shift[axisIndex] =
        currentMachine - programPosition -
        g_ncCoordinateState.work_offset[g_ncCoordinateState.selected_work_index][axisIndex] -
        g_ncCoordinateState.local_shift[axisIndex];
    if (g_ncCoordinateState.temporary_shift[axisIndex] == 0) {
        g_ncCoordinateState.active_temporary_mask &= ~(1UL << (uint32_t)axisIndex);
    } else {
        g_ncCoordinateState.active_temporary_mask |= (1UL << (uint32_t)axisIndex);
    }
    RefreshMachinePositions();
    return 0;
}

/**
 * @brief Handle nc coordinate fill targets for this module.
 * @param pBlock NC execution block read or updated by the helper.
 */
void NcCoordinate_FillTargets(NC_EXEC_BLOCK* pBlock)
{
    uint32_t i;

    if (pBlock == 0) {
        return;
    }
    for (i = 0U; i < AXIS_MAX; ++i) {
        pBlock->axis_target[i] = g_ncCoordinateState.machine_position[i];
    }
}
