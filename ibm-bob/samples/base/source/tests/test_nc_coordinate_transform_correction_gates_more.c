#include "test_common.h"
#include "system_shared.h"
#include "nc_coordinate.h"
#include "nc_coordinate_transform.h"
#include "nc_program.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();
}

static NC_EXEC_BLOCK make_motion(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x07U;
    b.axis_target[0] = 1000;
    b.axis_target[1] = 2000;
    b.axis_target[2] = 3000;
    return b;
}

static int test_enabled_flags_gate_each_correction_family(void)
{
    NC_EXEC_BLOCK b = make_motion(1611U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, 100));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetWorkMountError(1U, 200));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetRotaryTableOffset(2U, 300));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetEnabled(0U, 1U, 0U));

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&b, &err));

    TEST_ASSERT_EQ_INT(1000, b.axis_target[0]);
    TEST_ASSERT_EQ_INT(2200, b.axis_target[1]);
    TEST_ASSERT_EQ_INT(3000, b.axis_target[2]);
    TEST_ASSERT_EQ_UINT(0x02U, g_ncCoordinateTransformStatus.last_axis_mask);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.work_mount_error_blocks);
    return 0;
}

static int test_invalid_work_offset_index_is_rejected_and_counted(void)
{
    setup();
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_SetWorkOffset(0U, (uint8_t)AXIS_MAX, 1));
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.api_rejected_calls);
    TEST_ASSERT_TRUE(g_ncCoordinateTransformStatus.generation > 0U);
    return 0;
}

static int test_arc_center_is_shifted_on_corrected_arc_axes(void)
{
    NC_EXEC_BLOCK b = make_motion(1612U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    b.motion_type = NC_MOTION_ARC_CCW;
    b.axis_mask = 0x03U;
    b.arc_center[0] = 500;
    b.arc_center[1] = 600;
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, -50));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(1U, 75));

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&b, &err));

    TEST_ASSERT_EQ_INT(950, b.axis_target[0]);
    TEST_ASSERT_EQ_INT(2075, b.axis_target[1]);
    TEST_ASSERT_EQ_INT(450, b.arc_center[0]);
    TEST_ASSERT_EQ_INT(675, b.arc_center[1]);
    TEST_ASSERT_EQ_UINT(0x03U, g_ncCoordinateTransformStatus.last_axis_mask);
    return 0;
}

static int test_non_motion_block_records_no_correction_even_with_active_offsets(void)
{
    NC_EXEC_BLOCK b = make_motion(1613U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    b.motion_type = NC_MOTION_AUX_ONLY;
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, 999));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(1000, b.axis_target[0]);
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateTransformStatus.corrected_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_enabled_flags_gate_each_correction_family);
    RUN_TEST(test_invalid_work_offset_index_is_rejected_and_counted);
    RUN_TEST(test_arc_center_is_shifted_on_corrected_arc_axes);
    RUN_TEST(test_non_motion_block_records_no_correction_even_with_active_offsets);
    printf("test_nc_coordinate_transform_correction_gates_more: PASS\n");
    return 0;
}
