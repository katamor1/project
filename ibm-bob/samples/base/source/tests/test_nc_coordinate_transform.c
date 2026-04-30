#include "test_common.h"
#include "stub_external_io.h"
#include "nc_coordinate.h"
#include "nc_coordinate_transform.h"

static int test_transform_reset_enabled_and_api_validation(void)
{
    Test_ResetSystem();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();

    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.dynamic_fixture_enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.work_mount_error_enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.rotary_fixture_enabled);

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetEnabled(0U, 1U, 0U));
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateTransformStatus.dynamic_fixture_enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.work_mount_error_enabled);
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateTransformStatus.rotary_fixture_enabled);

    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_SetLocalShift(AXIS_MAX, 10));
    TEST_ASSERT_TRUE(g_ncCoordinateTransformStatus.api_rejected_calls > 0U);
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_SetWorkOffset(NC_WORK_COORD_SYSTEMS, 0U, 10));
    return 0;
}

static int test_transform_setters_update_masks_and_coordinate_state(void)
{
    Test_ResetSystem();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetWorkOffset(0U, 0U, 100));
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.work_offset_set_calls);
    TEST_ASSERT_EQ_INT(100, g_ncCoordinateState.work_offset[0][0]);

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetLocalShift(1U, 20));
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.local_shift_set_calls);
    TEST_ASSERT_EQ_INT(20, g_ncCoordinateState.local_shift[1]);

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, 5));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetWorkMountError(0U, -2));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetRotaryTableOffset(1U, 7));
    TEST_ASSERT_TRUE((g_ncCoordinateTransformStatus.active_correction_mask & 0x3U) == 0x3U);
    return 0;
}

static int test_apply_block_null_non_motion_disabled_and_correction_counts(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;

    Test_ResetSystem();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();
    block = Test_MakeMotionBlock(50U, NC_MOTION_LINEAR, 0x3U, 1000, 2000, 3000, 4000);
    block.axis_start[0] = 900;
    block.axis_start[1] = 1900;
    block.arc_center[0] = 10;

    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_ApplyBlockTs(NULL, &err));
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_ApplyBlockTs(&block, NULL));

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, 10));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetWorkMountError(0U, 2));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetRotaryTableOffset(1U, -5));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&block, &err));
    TEST_ASSERT_EQ_INT(1012, block.axis_target[0]);
    TEST_ASSERT_EQ_INT(912, block.axis_start[0]);
    TEST_ASSERT_EQ_INT(1995, block.axis_target[1]);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.corrected_blocks);
    TEST_ASSERT_EQ_UINT(0x3U, g_ncCoordinateTransformStatus.last_axis_mask);
    TEST_ASSERT_EQ_UINT(50U, g_ncCoordinateTransformStatus.last_line_no);

    block.motion_type = NC_MOTION_AUX_ONLY;
    block.axis_target[0] = 1;
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&block, &err));
    TEST_ASSERT_EQ_INT(1, block.axis_target[0]);

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetEnabled(0U, 0U, 0U));
    block.motion_type = NC_MOTION_LINEAR;
    block.axis_target[0] = 100;
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&block, &err));
    TEST_ASSERT_EQ_INT(100, block.axis_target[0]);
    return 0;
}

static int test_arc_center_is_corrected_for_arc_motion(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;

    Test_ResetSystem();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();
    block = Test_MakeMotionBlock(51U, NC_MOTION_ARC_CW, 0x1U, 100, 0, 0, 0);
    block.arc_center[0] = 20;
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, 3));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&block, &err));
    TEST_ASSERT_EQ_INT(103, block.axis_target[0]);
    TEST_ASSERT_EQ_INT(3, block.axis_start[0]);
    TEST_ASSERT_EQ_INT(23, block.arc_center[0]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_transform_reset_enabled_and_api_validation);
    RUN_TEST(test_transform_setters_update_masks_and_coordinate_state);
    RUN_TEST(test_apply_block_null_non_motion_disabled_and_correction_counts);
    RUN_TEST(test_arc_center_is_corrected_for_arc_motion);
    printf("[  PASSED  ] test_nc_coordinate_transform\n");
    return 0;
}
