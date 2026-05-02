#include "test_common.h"
#include "system_shared.h"
#include "nc_coordinate_transform.h"
#include "nc_coordinate.h"

static NC_EXEC_BLOCK make_motion(uint32_t line)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = line;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x03U;
    b.axis_start[0] = 100;
    b.axis_target[0] = 1000;
    b.axis_start[1] = 200;
    b.axis_target[1] = 2000;
    return b;
}

static int test_disabled_corrections_keep_active_mask_zero(void)
{
    SystemShared_Initialize();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, 100));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetEnabled(0U, 0U, 0U));
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateTransformStatus.active_correction_mask);
    return 0;
}

static int test_invalid_axis_rejects_and_counts_api_rejected_call(void)
{
    SystemShared_Initialize();
    NcCoordinateTransform_Reset();
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_SetWorkMountError((uint8_t)AXIS_MAX, 1));
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.api_rejected_calls);
    return 0;
}

static int test_apply_combines_enabled_dynamic_mount_and_rotary_offsets(void)
{
    NC_ERROR_CODE err = NC_ERR_NONE;
    NC_EXEC_BLOCK b = make_motion(1860U);
    SystemShared_Initialize();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, 10));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetWorkMountError(0U, -3));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetRotaryTableOffset(0U, 5));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(112, b.axis_start[0]);
    TEST_ASSERT_EQ_INT(1012, b.axis_target[0]);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.corrected_blocks);
    TEST_ASSERT_EQ_UINT(0x01U, g_ncCoordinateTransformStatus.last_axis_mask);
    return 0;
}

static int test_non_motion_and_null_paths_do_not_modify_status(void)
{
    NC_ERROR_CODE err = NC_ERR_NONE;
    NC_EXEC_BLOCK b = make_motion(1861U);
    SystemShared_Initialize();
    NcCoordinateTransform_Reset();
    b.motion_type = NC_MOTION_NONE;
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateTransformStatus.corrected_blocks);
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_ApplyBlockTs(0, &err));
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_ApplyBlockTs(&b, 0));
    return 0;
}

int main(void)
{
    RUN_TEST(test_disabled_corrections_keep_active_mask_zero);
    RUN_TEST(test_invalid_axis_rejects_and_counts_api_rejected_call);
    RUN_TEST(test_apply_combines_enabled_dynamic_mount_and_rotary_offsets);
    RUN_TEST(test_non_motion_and_null_paths_do_not_modify_status);
    printf("test_nc_coordinate_transform_v18_apply_more: PASS\n");
    return 0;
}
