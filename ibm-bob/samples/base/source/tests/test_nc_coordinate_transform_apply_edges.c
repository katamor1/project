#include "test_common.h"
#include "nc_coordinate.h"
#include "nc_coordinate_transform.h"

/*
 * Covers coordinate transform branches for disabled correction layers,
 * arc-center correction, no-motion no-op and NULL pointer rejection.
 */

static int test_apply_motion_block_corrects_selected_enabled_layers(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();
    memset(&block, 0, sizeof(block));

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetEnabled(1U, 0U, 1U));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, 100));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetWorkMountError(0U, 1000));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetRotaryTableOffset(0U, -25));

    block.line_no = 12U;
    block.motion_type = NC_MOTION_ARC_CW;
    block.axis_mask = 0x01U;
    block.axis_start[0] = 10;
    block.axis_target[0] = 20;
    block.arc_center[0] = 30;

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&block, &err));
    /* Work-mount layer is disabled, so only +100 and -25 are applied. */
    TEST_ASSERT_EQ_INT(85, block.axis_start[0]);
    TEST_ASSERT_EQ_INT(95, block.axis_target[0]);
    TEST_ASSERT_EQ_INT(105, block.arc_center[0]);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.corrected_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.dynamic_fixture_blocks);
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateTransformStatus.work_mount_error_blocks);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.rotary_fixture_blocks);
    TEST_ASSERT_EQ_UINT(0x01U, g_ncCoordinateTransformStatus.last_axis_mask);
    TEST_ASSERT_EQ_UINT(12U, g_ncCoordinateTransformStatus.last_line_no);
    return 0;
}

static int test_apply_noop_and_reject_paths(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;
    uint32_t corrected;
    SystemShared_Initialize();
    NcCoordinateTransform_Reset();
    memset(&block, 0, sizeof(block));

    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_ApplyBlockTs(NULL, &err));
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_ApplyBlockTs(&block, NULL));
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_SetDynamicFixtureOffset(AXIS_MAX, 1));
    TEST_ASSERT_TRUE(g_ncCoordinateTransformStatus.api_rejected_calls > 0U);

    block.motion_type = NC_MOTION_AUX_ONLY;
    block.axis_mask = 0x01U;
    block.axis_target[0] = 123;
    corrected = g_ncCoordinateTransformStatus.corrected_blocks;
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&block, &err));
    TEST_ASSERT_EQ_UINT(corrected, g_ncCoordinateTransformStatus.corrected_blocks);
    TEST_ASSERT_EQ_INT(123, block.axis_target[0]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_apply_motion_block_corrects_selected_enabled_layers);
    RUN_TEST(test_apply_noop_and_reject_paths);
    printf("test_nc_coordinate_transform_apply_edges: PASS\n");
    return 0;
}
