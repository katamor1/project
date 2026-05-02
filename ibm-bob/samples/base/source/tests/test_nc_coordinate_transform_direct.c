#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_coordinate.h"
#include "nc_coordinate_transform.h"

extern int32_t NcCoordinateTransform_SetEnabled(uint8_t dynamicFixture, uint8_t workMountError, uint8_t rotaryFixture);
extern int32_t NcCoordinateTransform_SetWorkOffset(uint8_t workIndex, uint8_t axis, int32_t offset);
extern int32_t NcCoordinateTransform_SetTemporaryAbsolute(uint8_t axis, int32_t programPosition);
extern int32_t NcCoordinateTransform_SetDynamicFixtureOffset(uint8_t axis, int32_t offset);
extern int32_t NcCoordinateTransform_SetRotaryTableOffset(uint8_t axis, int32_t offset);
extern int32_t NcCoordinateTransform_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError);

static void setup(void)
{
    SystemShared_Initialize();
    NcCoordinate_Reset();
    NcCoordinateTransform_Reset();
}

static NC_EXEC_BLOCK make_linear(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x01U;
    b.axis_target[0] = 1000;
    b.feed_override_percent = 100U;
    return b;
}

static int test_setters_validate_axis_and_update_coordinate_state(void)
{
    setup();

    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_SetWorkOffset(NC_WORK_COORD_SYSTEMS, 0U, 10));
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_SetWorkOffset(0U, AXIS_MAX, 10));
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_SetTemporaryAbsolute(AXIS_MAX, 0));
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_SetRotaryTableOffset(AXIS_MAX, 1));
    TEST_ASSERT_TRUE(g_ncCoordinateTransformStatus.api_rejected_calls >= 3U);

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetWorkOffset(0U, 0U, 100));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetLocalShift(0U, 20));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetTemporaryAbsolute(0U, 500));
    TEST_ASSERT_EQ_INT(500, g_ncCoordinateState.program_position[0]);
    TEST_ASSERT_EQ_INT(-500, g_ncCoordinateState.temporary_shift[0]);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.work_offset_set_calls);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.local_shift_set_calls);
    TEST_ASSERT_EQ_UINT(1U, g_ncCoordinateTransformStatus.temporary_absolute_set_calls);
    return 0;
}

static int test_enabled_mask_and_apply_combined_corrections(void)
{
    NC_EXEC_BLOCK b = make_linear(701U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetDynamicFixtureOffset(0U, 10));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetWorkMountError(0U, -5));
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetRotaryTableOffset(0U, 7));
    TEST_ASSERT_EQ_UINT(0x01U, g_ncCoordinateTransformStatus.active_correction_mask);

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetEnabled(0U, 0U, 0U));
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateTransformStatus.active_correction_mask);
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(1000, b.axis_target[0]);
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateTransformStatus.corrected_blocks);
    TEST_ASSERT_EQ_UINT(0U, g_ncCoordinateTransformStatus.last_axis_mask);

    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_SetEnabled(1U, 1U, 1U));
    TEST_ASSERT_EQ_UINT(0x01U, g_ncCoordinateTransformStatus.active_correction_mask);
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(1012, b.axis_target[0]);
    TEST_ASSERT_EQ_INT(1000, g_ncCoordinateTransformStatus.last_input_target[0]);
    TEST_ASSERT_EQ_INT(1012, g_ncCoordinateTransformStatus.last_corrected_target[0]);
    TEST_ASSERT_EQ_UINT(0x01U, g_ncCoordinateTransformStatus.last_axis_mask);
    return 0;
}

static int test_apply_rejects_null_error_pointer_and_noop_blocks(void)
{
    NC_EXEC_BLOCK b = make_linear(702U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();

    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_ApplyBlockTs(NULL, &err));
    TEST_ASSERT_EQ_INT(-1, NcCoordinateTransform_ApplyBlockTs(&b, NULL));

    b.motion_type = NC_MOTION_AUX_ONLY;
    b.axis_target[0] = 2000;
    TEST_ASSERT_EQ_INT(0, NcCoordinateTransform_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(2000, b.axis_target[0]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_setters_validate_axis_and_update_coordinate_state);
    RUN_TEST(test_enabled_mask_and_apply_combined_corrections);
    RUN_TEST(test_apply_rejects_null_error_pointer_and_noop_blocks);
    printf("test_nc_coordinate_transform_direct: PASS\n");
    return 0;
}
