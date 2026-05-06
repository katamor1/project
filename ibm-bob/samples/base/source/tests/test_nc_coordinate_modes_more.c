#include "test_common.h"
#include "system_shared.h"
#include "nc_coordinate.h"
#include "nc_program.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcCoordinate_Reset();
}

static int test_select_work_rejects_out_of_range_without_changing_selection(void)
{
    setup();
    NcCoordinate_SelectWork(2U);
    TEST_ASSERT_EQ_UINT(2U, g_ncCoordinateState.selected_work_index);
    NcCoordinate_SelectWork(NC_WORK_COORD_SYSTEMS);
    TEST_ASSERT_EQ_UINT(2U, g_ncCoordinateState.selected_work_index);
    return 0;
}

static int test_local_shift_zero_clears_active_mask_and_recomputes_machine_position(void)
{
    setup();
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetWorkOffsetAxis(0U, 0, 100));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetProgramAxis(0, 200, &(NC_EXEC_BLOCK){0}));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetLocalShiftAxis(0, 50));
    TEST_ASSERT_EQ_INT(350, g_ncCoordinateState.machine_position[0]);
    TEST_ASSERT_TRUE((g_ncCoordinateState.active_local_mask & 0x01U) != 0U);
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetLocalShiftAxis(0, 0));
    TEST_ASSERT_EQ_INT(300, g_ncCoordinateState.machine_position[0]);
    TEST_ASSERT_TRUE((g_ncCoordinateState.active_local_mask & 0x01U) == 0U);
    return 0;
}

static int test_temporary_axis_keeps_current_machine_position_and_sets_mask(void)
{
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetWorkOffsetAxis(0U, 1, 1000));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetProgramAxis(1, 500, &b));
    TEST_ASSERT_EQ_INT(1500, g_ncCoordinateState.machine_position[1]);
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetTemporaryAxis(1, 100));
    TEST_ASSERT_EQ_INT(100, g_ncCoordinateState.program_position[1]);
    TEST_ASSERT_EQ_INT(1500, g_ncCoordinateState.machine_position[1]);
    TEST_ASSERT_TRUE((g_ncCoordinateState.active_temporary_mask & 0x02U) != 0U);
    return 0;
}

static int test_incremental_program_axis_and_fill_targets(void)
{
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    NcCoordinate_SetDistanceMode(NC_DISTANCE_INCREMENTAL);
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetProgramAxis(2, 100, &b));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetProgramAxis(2, -25, &b));
    TEST_ASSERT_EQ_INT(75, g_ncCoordinateState.program_position[2]);
    memset(&b, 0, sizeof(b));
    NcCoordinate_FillTargets(&b);
    TEST_ASSERT_EQ_INT(75, b.axis_target[2]);
    NcCoordinate_FillTargets(NULL);
    return 0;
}

int main(void)
{
    RUN_TEST(test_select_work_rejects_out_of_range_without_changing_selection);
    RUN_TEST(test_local_shift_zero_clears_active_mask_and_recomputes_machine_position);
    RUN_TEST(test_temporary_axis_keeps_current_machine_position_and_sets_mask);
    RUN_TEST(test_incremental_program_axis_and_fill_targets);
    printf("test_nc_coordinate_modes_more: PASS\n");
    return 0;
}
