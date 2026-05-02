#include "test_common.h"
#include "system_shared.h"
#include "nc_coordinate.h"
#include "nc_coordinate_transform.h"

static NC_EXEC_BLOCK make_motion(uint32_t line, uint32_t mask)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = mask;
    for (uint32_t i = 0U; i < AXIS_MAX; ++i) {
        b.axis_start[i] = (int32_t)(10 * ((int32_t)i + 1));
        b.axis_target[i] = (int32_t)(100 * ((int32_t)i + 1));
        b.arc_center[i] = (int32_t)(1000 * ((int32_t)i + 1));
    }
    return b;
}

static int test_all_enabled_offsets_are_added_to_motion_and_arc_center(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();
    b = make_motion(1921U, 0x07U);
    b.motion_type = NC_MOTION_ARC_CW;
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, 10));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetWorkMountError(1U, 20));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetRotaryTableOffset(2U, 30));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(110, b.axis_target[0]);
    TEST_ASSERT_EQ_INT(220, b.axis_target[1]);
    TEST_ASSERT_EQ_INT(330, b.axis_target[2]);
    TEST_ASSERT_EQ_INT(1010, b.arc_center[0]);
    TEST_ASSERT_EQ_UINT(0x07U, g_ncCoordinateTransformStatus.last_axis_mask);
    return 0;
}

static int test_disabled_dynamic_fixture_is_excluded_from_correction_sum(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();
    b = make_motion(1922U, 0x01U);
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, 10));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetWorkMountError(0U, 20));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetRotaryTableOffset(0U, 30));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetEnabled(0U, 1U, 1U));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(150, b.axis_target[0]);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.corrected_blocks);
    return 0;
}

static int test_invalid_axis_and_work_index_are_counted_as_api_rejections(void)
{
    uint32_t before;
    SystemShared_Initialize();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();
    before = g_ncCoordinateTransformStatus.api_rejected_calls;
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_SetDynamicFixtureOffset(AXIS_MAX, 1));
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_SetWorkOffset(0U, AXIS_MAX, 1));
    TEST_ASSERT_EQ_UINT(before + 2U, g_ncCoordinateTransformStatus.api_rejected_calls);
    return 0;
}

static int test_non_motion_or_zero_axis_mask_block_is_noop(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();
    b = make_motion(1923U, 0x00U);
    b.motion_type = NC_MOTION_AUX_ONLY;
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, 1000));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(100, b.axis_target[0]);
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateTransformStatus.corrected_blocks);
    return 0;
}

int main(void)
{
    RUN_TEST(test_all_enabled_offsets_are_added_to_motion_and_arc_center);
    RUN_TEST(test_disabled_dynamic_fixture_is_excluded_from_correction_sum);
    RUN_TEST(test_invalid_axis_and_work_index_are_counted_as_api_rejections);
    RUN_TEST(test_non_motion_or_zero_axis_mask_block_is_noop);
    printf("test_nc_coordinate_transform_v19_modes_more: PASS\n");
    return 0;
}
