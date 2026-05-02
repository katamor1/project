#include "test_common.h"
#include "system_shared.h"
#include "nc_compensation.h"

static NC_EXEC_BLOCK make_motion(uint32_t line)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x07U;
    b.axis_start[0] = 0;
    b.axis_start[1] = 0;
    b.axis_start[2] = 0;
    b.axis_target[0] = 10000;
    b.axis_target[1] = 0;
    b.axis_target[2] = 3000;
    return b;
}

static int test_tool_length_negative_command_subtracts_z_offset(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcCompensation_Reset();
    b = make_motion(1911U);
    TEST_ASSERT_EQ_INT(0, NcCompensation_SetToolLengthOffset(2U, 500));
    b.tool_length_command = NC_TOOL_LENGTH_CMD_NEGATIVE;
    b.h_offset_no = 2U;
    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(2500, b.axis_target[2]);
    TEST_ASSERT_EQ_UINT(2U, g_ncCompensationStatus.tool_length_sign);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_TOOL_LENGTH) != 0U);
    return 0;
}

static int test_missing_tool_length_offset_reports_range_error(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcCompensation_Reset();
    b = make_motion(1912U);
    b.tool_length_command = NC_TOOL_LENGTH_CMD_POSITIVE;
    b.h_offset_no = NC_TOOL_OFFSET_TABLE_SIZE;
    TEST_ASSERT_EQ_INT(-1, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(NC_ERR_RANGE, err);
    TEST_ASSERT_EQ_UINT(0U, g_ncCompensationStatus.tool_length_active);
    return 0;
}

static int test_cutter_radius_right_offsets_normal_to_motion_direction(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcCompensation_Reset();
    b = make_motion(1913U);
    TEST_ASSERT_EQ_INT(0, NcCompensation_SetCutterRadiusOffset(1U, 100));
    b.cutter_comp_command = NC_CUTTER_COMP_CMD_RIGHT;
    b.d_offset_no = 1U;
    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(10000, b.axis_target[0]);
    TEST_ASSERT_EQ_INT(-100, b.axis_target[1]);
    TEST_ASSERT_EQ_UINT(2U, g_ncCompensationStatus.cutter_comp_mode);
    return 0;
}

static int test_polar_conversion_uses_rotation_center_as_polar_origin(void)
{
    NC_EXEC_BLOCK setup;
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcCompensation_Reset();
    setup = make_motion(1914U);
    setup.motion_type = NC_MOTION_NONE;
    setup.rotation_command = NC_ROTATION_CMD_ENABLE;
    setup.rotation_center[0] = 100;
    setup.rotation_center[1] = 200;
    setup.rotation_angle_deg = 0;
    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&setup, &err));
    b = make_motion(1915U);
    b.axis_mask = 0x03U;
    b.axis_target[0] = 10000;
    b.axis_target[1] = 90 * NC_POSITION_SCALE;
    b.polar_command = NC_POLAR_CMD_ENABLE;
    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(100, b.axis_target[0]);
    TEST_ASSERT_EQ_INT(10200, b.axis_target[1]);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_POLAR_COORD) != 0U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_tool_length_negative_command_subtracts_z_offset);
    RUN_TEST(test_missing_tool_length_offset_reports_range_error);
    RUN_TEST(test_cutter_radius_right_offsets_normal_to_motion_direction);
    RUN_TEST(test_polar_conversion_uses_rotation_center_as_polar_origin);
    printf("test_nc_compensation_v19_commands_more: PASS\n");
    return 0;
}
