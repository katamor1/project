#include "test_common.h"
#include "system_shared.h"
#include "nc_coordinate.h"

static int test_select_work_rejects_out_of_range_and_clears_local_temporary(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    NcCoordinate_Reset();
    memset(&b, 0, sizeof(b));

    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetLocalShiftAxis(0, 100));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetTemporaryAxis(1, 200));
    TEST_ASSERT_TRUE(g_ncCoordinateState.active_local_mask != 0U);
    TEST_ASSERT_TRUE(g_ncCoordinateState.active_temporary_mask != 0U);

    NcCoordinate_SelectWork((uint8_t)NC_WORK_COORD_SYSTEMS);
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateState.selected_work_index);
    TEST_ASSERT_TRUE(g_ncCoordinateState.active_local_mask != 0U);

    NcCoordinate_SelectWork(2U);
    TEST_ASSERT_EQ_UINT(2U, g_ncCoordinateState.selected_work_index);
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateState.active_local_mask);
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateState.active_temporary_mask);
    return 0;
}

static int test_unit_and_distance_modes_are_state_only_until_axis_command(void)
{
    NC_EXEC_BLOCK b;
    SystemShared_Initialize();
    NcCoordinate_Reset();
    memset(&b, 0, sizeof(b));

    NcCoordinate_SetUnitMode(NC_UNIT_INCH);
    NcCoordinate_SetDistanceMode(NC_DISTANCE_INCREMENTAL);
    TEST_ASSERT_EQ_UINT(NC_UNIT_INCH, g_ncCoordinateState.unit_mode);
    TEST_ASSERT_EQ_UINT(NC_DISTANCE_INCREMENTAL, g_ncCoordinateState.distance_mode);

    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetProgramAxis(0, 1000, &b));
    TEST_ASSERT_EQ_INT(1000, g_ncCoordinateState.program_position[0]);
    TEST_ASSERT_EQ_INT(1000, b.axis_target[0]);
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetProgramAxis(0, 500, &b));
    TEST_ASSERT_EQ_INT(1500, g_ncCoordinateState.program_position[0]);
    TEST_ASSERT_EQ_INT(1500, b.axis_target[0]);
    return 0;
}

static int test_fill_targets_ignores_null_and_copies_all_axes(void)
{
    NC_EXEC_BLOCK b;
    uint32_t i;
    SystemShared_Initialize();
    NcCoordinate_Reset();

    NcCoordinate_FillTargets(NULL);
    for (i = 0U; i < AXIS_MAX; ++i) {
        TEST_ASSERT_EQ_INT(0, NcCoordinate_SetMachineAxis((int32_t)i, (int32_t)(1000 + (int32_t)i), &b));
    }
    memset(&b, 0, sizeof(b));
    NcCoordinate_FillTargets(&b);
    for (i = 0U; i < AXIS_MAX; ++i) {
        TEST_ASSERT_EQ_INT((int32_t)(1000 + (int32_t)i), b.axis_target[i]);
    }
    return 0;
}

int main(void)
{
    RUN_TEST(test_select_work_rejects_out_of_range_and_clears_local_temporary);
    RUN_TEST(test_unit_and_distance_modes_are_state_only_until_axis_command);
    RUN_TEST(test_fill_targets_ignores_null_and_copies_all_axes);
    printf("test_nc_coordinate_selection_clear_edges: PASS\n");
    return 0;
}
