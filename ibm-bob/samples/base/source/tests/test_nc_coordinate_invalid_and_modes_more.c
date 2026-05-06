#include <string.h>
#include "test_common.h"
#include "nc_coordinate.h"

static int test_invalid_axis_setters_reject_without_mutating_masks(void)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    SystemShared_Initialize();
    NcCoordinate_Reset();
    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetLocalShiftAxis(-1, 1));
    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetProgramAxis(AXIS_MAX, 10, &b));
    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetMachineAxis(-1, 10, &b));
    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetTemporaryAxis(AXIS_MAX, 10));
    TEST_ASSERT_EQ_UINT(0U, b.axis_mask);
    return 0;
}

static int test_invalid_null_block_program_and_machine_axis_rejected(void)
{
    SystemShared_Initialize();
    NcCoordinate_Reset();
    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetProgramAxis(0, 100, NULL));
    TEST_ASSERT_EQ_INT(-1, NcCoordinate_SetMachineAxis(0, 100, NULL));
    TEST_ASSERT_EQ_INT(0, g_ncCoordinateState.program_position[0]);
    TEST_ASSERT_EQ_INT(0, g_ncCoordinateState.machine_position[0]);
    return 0;
}

static int test_unit_and_distance_modes_are_stored_even_without_motion(void)
{
    uint32_t before;
    SystemShared_Initialize();
    NcCoordinate_Reset();
    before = g_ncCoordinateState.generation;
    NcCoordinate_SetUnitMode(NC_UNIT_INCH);
    NcCoordinate_SetDistanceMode(NC_DISTANCE_INCREMENTAL);
    TEST_ASSERT_EQ_UINT(NC_UNIT_INCH, g_ncCoordinateState.unit_mode);
    TEST_ASSERT_EQ_UINT(NC_DISTANCE_INCREMENTAL, g_ncCoordinateState.distance_mode);
    TEST_ASSERT_TRUE(g_ncCoordinateState.generation > before);
    return 0;
}

static int test_local_shift_zero_clears_active_bit_and_refreshes_machine_position(void)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    SystemShared_Initialize();
    NcCoordinate_Reset();
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetProgramAxis(0, 1000, &b));
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetLocalShiftAxis(0, 200));
    TEST_ASSERT_TRUE((g_ncCoordinateState.active_local_mask & 1U) != 0U);
    TEST_ASSERT_EQ_INT(1200, g_ncCoordinateState.machine_position[0]);
    TEST_ASSERT_EQ_INT(0, NcCoordinate_SetLocalShiftAxis(0, 0));
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateState.active_local_mask & 1U);
    TEST_ASSERT_EQ_INT(1000, g_ncCoordinateState.machine_position[0]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_invalid_axis_setters_reject_without_mutating_masks);
    RUN_TEST(test_invalid_null_block_program_and_machine_axis_rejected);
    RUN_TEST(test_unit_and_distance_modes_are_stored_even_without_motion);
    RUN_TEST(test_local_shift_zero_clears_active_bit_and_refreshes_machine_position);
    printf("test_nc_coordinate_invalid_and_modes_more: PASS\n");
    return 0;
}
