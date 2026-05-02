#include "test_common.h"
#include "nc_coordinate.h"

/*
 * Direct coordinate-state tests for program/machine/temporary axes.
 * These cover branches that parser-level tests usually hide: NULL block,
 * invalid axis, absolute/incremental mode, and temporary coordinate side effects.
 */

static int test_program_axis_absolute_incremental_and_offsets(void)
{
    NC_EXEC_BLOCK block;
    SystemShared_Initialize();
    NcCoordinate_Reset();
    memset(&block, 0, sizeof(block));

    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetWorkOffsetAxis(0U, 0, 1000));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetLocalShiftAxis(0, 200));

    /* Absolute command: machine target = program + work offset + local shift. */
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetProgramAxis(0, 3000, &block));
    TEST_ASSERT_EQ_INT(3000, g_ncCoordinateState.program_position[0]);
    TEST_ASSERT_EQ_INT(4200, g_ncCoordinateState.machine_position[0]);
    TEST_ASSERT_EQ_INT(4200, block.axis_target[0]);
    TEST_ASSERT_TRUE((block.axis_mask & 0x01U) != 0U);

    /* Incremental command adds to the previous program position. */
    NcCoordinate_SetDistanceMode(NC_DISTANCE_INCREMENTAL);
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetProgramAxis(0, 500, &block));
    TEST_ASSERT_EQ_INT(3500, g_ncCoordinateState.program_position[0]);
    TEST_ASSERT_EQ_INT(4700, block.axis_target[0]);
    return 0;
}

static int test_machine_and_temporary_axis_side_effects(void)
{
    NC_EXEC_BLOCK block;
    SystemShared_Initialize();
    NcCoordinate_Reset();
    memset(&block, 0, sizeof(block));

    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetWorkOffsetAxis(0U, 0, 1000));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetLocalShiftAxis(0, 200));

    /* Machine coordinate command back-calculates the stored program position. */
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetMachineAxis(0, 8000, &block));
    TEST_ASSERT_EQ_INT(8000, g_ncCoordinateState.machine_position[0]);
    TEST_ASSERT_EQ_INT(6800, g_ncCoordinateState.program_position[0]);
    TEST_ASSERT_EQ_INT(8000, block.axis_target[0]);

    /* Temporary absolute keeps the current machine point but changes program origin. */
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetTemporaryAxis(0, 100));
    TEST_ASSERT_EQ_INT(100, g_ncCoordinateState.program_position[0]);
    TEST_ASSERT_EQ_INT(6700, g_ncCoordinateState.temporary_shift[0]);
    TEST_ASSERT_TRUE((g_ncCoordinateState.active_temporary_mask & 0x01U) != 0U);
    TEST_ASSERT_EQ_INT(8000, g_ncCoordinateState.machine_position[0]);
    return 0;
}

static int test_invalid_axis_null_and_fill_targets_noop(void)
{
    NC_EXEC_BLOCK block;
    uint32_t generation;
    SystemShared_Initialize();
    NcCoordinate_Reset();
    memset(&block, 0, sizeof(block));
    generation = g_ncCoordinateState.generation;

    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetProgramAxis(-1, 1, &block));
    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetProgramAxis(AXIS_MAX, 1, &block));
    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetProgramAxis(0, 1, NULL));
    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetMachineAxis(AXIS_MAX, 1, &block));
    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetTemporaryAxis(AXIS_MAX, 1));
    TEST_ASSERT_EQ_UINT(generation, g_ncCoordinateState.generation);

    g_ncCoordinateState.machine_position[0] = 123;
    g_ncCoordinateState.machine_position[1] = -456;
    NcCoordinate_FillTargets(NULL); /* Must be a safe no-op. */
    NcCoordinate_FillTargets(&block);
    TEST_ASSERT_EQ_INT(123, block.axis_target[0]);
    TEST_ASSERT_EQ_INT(-456, block.axis_target[1]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_program_axis_absolute_incremental_and_offsets);
    RUN_TEST(test_machine_and_temporary_axis_side_effects);
    RUN_TEST(test_invalid_axis_null_and_fill_targets_noop);
    printf("test_nc_coordinate_program_machine_edges: PASS\n");
    return 0;
}
