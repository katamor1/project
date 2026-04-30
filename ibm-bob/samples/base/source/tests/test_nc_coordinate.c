#include "test_common.h"
#include "stub_external_io.h"
#include "nc_coordinate.h"

extern int32_t NcCoordinate_SetWorkOffsetAxis(uint8_t workIndex, int32_t axisIndex, int32_t offset);
extern int32_t NcCoordinate_SetProgramAxis(int32_t axisIndex, int32_t programPosition, NC_EXEC_BLOCK* pBlock);
extern int32_t NcCoordinate_SetMachineAxis(int32_t axisIndex, int32_t machinePosition, NC_EXEC_BLOCK* pBlock);
extern int32_t NcCoordinate_SetTemporaryAxis(int32_t axisIndex, int32_t programPosition);

static int test_coordinate_reset_modes_and_fill_targets_null_safe(void)
{
    NC_EXEC_BLOCK block;

    Test_ResetSystem();
    NcCoordinate_Reset();
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateState.selected_work_index);
    TEST_ASSERT_EQ_UINT(NC_UNIT_MM, g_ncCoordinateState.unit_mode);
    TEST_ASSERT_EQ_UINT(NC_DISTANCE_ABSOLUTE, g_ncCoordinateState.distance_mode);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateState.generation);

    NcCoordinate_SetUnitMode(NC_UNIT_INCH);
    NcCoordinate_SetDistanceMode(NC_DISTANCE_INCREMENTAL);
    TEST_ASSERT_EQ_UINT(NC_UNIT_INCH, g_ncCoordinateState.unit_mode);
    TEST_ASSERT_EQ_UINT(NC_DISTANCE_INCREMENTAL, g_ncCoordinateState.distance_mode);

    block = Test_MakeMotionBlock(1U, NC_MOTION_LINEAR, 0U, 111, 222, 333, 444);
    NcCoordinate_FillTargets(NULL);
    NcCoordinate_FillTargets(&block);
    TEST_ASSERT_EQ_INT(0, block.axis_target[0]);
    TEST_ASSERT_EQ_INT(0, block.axis_target[1]);
    return 0;
}

static int test_work_local_and_program_axis_absolute_path(void)
{
    NC_EXEC_BLOCK block;

    Test_ResetSystem();
    NcCoordinate_Reset();
    block = Test_MakeMotionBlock(2U, NC_MOTION_LINEAR, 0U, 0, 0, 0, 0);

    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetWorkOffsetAxis(NC_WORK_COORD_SYSTEMS, 0, 1));
    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetWorkOffsetAxis(0U, AXIS_MAX, 1));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetWorkOffsetAxis(0U, 0, 100));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetLocalShiftAxis(0, 20));
    TEST_ASSERT_EQ_UINT(0x1U, g_ncCoordinateState.active_local_mask);

    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetProgramAxis(-1, 10, &block));
    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetProgramAxis(0, 10, NULL));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetProgramAxis(0, 500, &block));
    TEST_ASSERT_EQ_INT(500, g_ncCoordinateState.program_position[0]);
    TEST_ASSERT_EQ_INT(620, g_ncCoordinateState.machine_position[0]);
    TEST_ASSERT_EQ_INT(620, block.axis_target[0]);
    TEST_ASSERT_EQ_UINT(0x1U, block.axis_mask);
    return 0;
}

static int test_incremental_machine_temporary_and_work_select_side_effects(void)
{
    NC_EXEC_BLOCK block;

    Test_ResetSystem();
    NcCoordinate_Reset();
    block = Test_MakeMotionBlock(3U, NC_MOTION_LINEAR, 0U, 0, 0, 0, 0);

    NcCoordinate_SetDistanceMode(NC_DISTANCE_INCREMENTAL);
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetProgramAxis(1, 100, &block));
    TEST_ASSERT_EQ_INT(100, g_ncCoordinateState.program_position[1]);
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetProgramAxis(1, -25, &block));
    TEST_ASSERT_EQ_INT(75, g_ncCoordinateState.program_position[1]);
    TEST_ASSERT_EQ_INT(75, block.axis_target[1]);

    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetMachineAxis(AXIS_MAX, 1000, &block));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetWorkOffsetAxis(0U, 2, 30));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetMachineAxis(2, 1000, &block));
    TEST_ASSERT_EQ_INT(970, g_ncCoordinateState.program_position[2]);
    TEST_ASSERT_EQ_INT(1000, block.axis_target[2]);

    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetTemporaryAxis(-1, 0));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetTemporaryAxis(2, 900));
    TEST_ASSERT_EQ_INT(900, g_ncCoordinateState.program_position[2]);
    TEST_ASSERT_EQ_UINT(0x4U, g_ncCoordinateState.active_temporary_mask);
    TEST_ASSERT_EQ_INT(1000, g_ncCoordinateState.machine_position[2]);

    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetLocalShiftAxis(0, 10));
    NcCoordinate_SelectWork((uint8_t)(NC_WORK_COORD_SYSTEMS + 1U));
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateState.selected_work_index);
    NcCoordinate_SelectWork(1U);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateState.selected_work_index);
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateState.active_local_mask);
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateState.active_temporary_mask);
    return 0;
}

int main(void)
{
    RUN_TEST(test_coordinate_reset_modes_and_fill_targets_null_safe);
    RUN_TEST(test_work_local_and_program_axis_absolute_path);
    RUN_TEST(test_incremental_machine_temporary_and_work_select_side_effects);
    printf("[  PASSED  ] test_nc_coordinate\n");
    return 0;
}
