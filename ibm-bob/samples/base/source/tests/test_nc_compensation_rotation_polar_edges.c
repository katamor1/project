#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_compensation.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcCompensation_Reset();
}

static NC_EXEC_BLOCK make_linear(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x03U;
    return b;
}

static int test_rotation_enable_rotates_xy_and_cancel_stops_rotation(void)
{
    NC_EXEC_BLOCK b = make_linear(201U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();

    b.axis_target[0] = 1000;
    b.axis_target[1] = 0;
    b.rotation_command = NC_ROTATION_CMD_ENABLE;
    b.rotation_center[0] = 0;
    b.rotation_center[1] = 0;
    b.rotation_angle_deg = 90 * NC_ANGLE_SCALE;

    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(1U, g_ncCompensationStatus.rotation_active);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_COORD_ROTATION) != 0U);
    TEST_ASSERT_TRUE(b.axis_target[0] >= -1 && b.axis_target[0] <= 1);
    TEST_ASSERT_TRUE(b.axis_target[1] >= 999 && b.axis_target[1] <= 1001);

    memset(&b, 0, sizeof(b));
    b.rotation_command = NC_ROTATION_CMD_CANCEL;
    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0U, g_ncCompensationStatus.rotation_active);
    return 0;
}

static int test_polar_conversion_uses_radius_angle_and_rotation_center(void)
{
    NC_EXEC_BLOCK cmd;
    NC_EXEC_BLOCK b = make_linear(202U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();

    memset(&cmd, 0, sizeof(cmd));
    cmd.rotation_command = NC_ROTATION_CMD_ENABLE;
    cmd.rotation_center[0] = 100;
    cmd.rotation_center[1] = -200;
    cmd.polar_command = NC_POLAR_CMD_ENABLE;
    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&cmd, &err));
    TEST_ASSERT_EQ_UINT(1U, g_ncCompensationStatus.polar_active);

    b.axis_target[0] = 1000; /* radius */
    b.axis_target[1] = 90 * NC_POSITION_SCALE; /* angle in degree-scale */
    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_POLAR_COORD) != 0U);
    TEST_ASSERT_TRUE(b.axis_target[0] >= 99 && b.axis_target[0] <= 101);
    TEST_ASSERT_TRUE(b.axis_target[1] >= 799 && b.axis_target[1] <= 801);
    return 0;
}

static int test_negative_tool_length_and_right_cutter_radius(void)
{
    NC_EXEC_BLOCK b = make_linear(203U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();

    TEST_ASSERT_EQ_INT(0, NcCompensation_SetToolLengthOffset(3U, 250));
    TEST_ASSERT_EQ_INT(0, NcCompensation_SetCutterRadiusOffset(4U, 100));
    b.axis_mask = 0x07U;
    b.axis_start[0] = 0;
    b.axis_start[1] = 0;
    b.axis_target[0] = 1000;
    b.axis_target[1] = 0;
    b.axis_target[2] = 500;
    b.tool_length_command = NC_TOOL_LENGTH_CMD_NEGATIVE;
    b.h_offset_no = 3U;
    b.cutter_comp_command = NC_CUTTER_COMP_CMD_RIGHT;
    b.d_offset_no = 4U;

    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(2U, g_ncCompensationStatus.tool_length_sign);
    TEST_ASSERT_EQ_UINT(2U, g_ncCompensationStatus.cutter_comp_mode);
    TEST_ASSERT_EQ_INT(250, b.axis_target[2]);
    TEST_ASSERT_EQ_INT(-100, b.axis_target[1]);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_TOOL_LENGTH) != 0U);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_CUTTER_RADIUS) != 0U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_rotation_enable_rotates_xy_and_cancel_stops_rotation);
    RUN_TEST(test_polar_conversion_uses_radius_angle_and_rotation_center);
    RUN_TEST(test_negative_tool_length_and_right_cutter_radius);
    printf("test_nc_compensation_rotation_polar_edges: PASS\n");
    return 0;
}
