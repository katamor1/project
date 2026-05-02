#include "test_common.h"
#include "system_shared.h"
#include "nc_compensation.h"
#include "nc_program.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcCompensation_Reset();
}

static NC_EXEC_BLOCK make_motion(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.feed_override_percent = 100U;
    return b;
}

static int test_negative_tool_length_offset_subtracts_from_z_target(void)
{
    NC_EXEC_BLOCK b = make_motion(1601U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcCompensation_SetToolLengthOffset(3U, 250));
    b.tool_length_command = NC_TOOL_LENGTH_CMD_NEGATIVE;
    b.h_offset_no = 3U;
    b.axis_target[2] = 1000;

    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));

    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_EQ_UINT(1U, g_ncCompensationStatus.tool_length_active);
    TEST_ASSERT_EQ_UINT(2U, g_ncCompensationStatus.tool_length_sign);
    TEST_ASSERT_EQ_INT(750, b.axis_target[2]);
    TEST_ASSERT_TRUE((b.axis_mask & (1UL << 2U)) != 0U);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_TOOL_LENGTH) != 0U);
    return 0;
}

static int test_polar_conversion_uses_rotation_center_without_extra_rotation(void)
{
    NC_EXEC_BLOCK b = make_motion(1602U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();

    b.rotation_command = NC_ROTATION_CMD_ENABLE;
    b.rotation_center[0] = 100;
    b.rotation_center[1] = -200;
    b.rotation_angle_deg = 0;
    b.polar_command = NC_POLAR_CMD_ENABLE;
    b.axis_mask = 0x03U;
    b.axis_target[0] = 10000;
    b.axis_target[1] = 900000;

    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));

    TEST_ASSERT_EQ_UINT(NC_ERR_NONE, err);
    TEST_ASSERT_EQ_UINT(1U, g_ncCompensationStatus.polar_active);
    TEST_ASSERT_EQ_INT(100, b.axis_target[0]);
    TEST_ASSERT_EQ_INT(9800, b.axis_target[1]);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_POLAR_COORD) != 0U);
    return 0;
}

static int test_rotation_cancel_clears_active_and_angle_but_keeps_center_for_diagnostics(void)
{
    NC_EXEC_BLOCK b = make_motion(1603U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    b.rotation_command = NC_ROTATION_CMD_ENABLE;
    b.rotation_center[0] = 111;
    b.rotation_center[1] = 222;
    b.rotation_angle_deg = 450000;
    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(1U, g_ncCompensationStatus.rotation_active);

    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = 1604U;
    b.rotation_command = NC_ROTATION_CMD_CANCEL;
    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));

    TEST_ASSERT_EQ_UINT(0U, g_ncCompensationStatus.rotation_active);
    TEST_ASSERT_EQ_INT(0, g_ncCompensationStatus.rotation_angle_deg);
    TEST_ASSERT_EQ_INT(111, g_ncCompensationStatus.rotation_center[0]);
    return 0;
}

static int test_zero_length_cutter_comp_does_not_adjust_target(void)
{
    NC_EXEC_BLOCK b = make_motion(1605U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcCompensation_SetCutterRadiusOffset(4U, 500));
    b.cutter_comp_command = NC_CUTTER_COMP_CMD_LEFT;
    b.d_offset_no = 4U;
    b.axis_mask = 0x03U;
    b.axis_start[0] = 1000;
    b.axis_start[1] = 2000;
    b.axis_target[0] = 1000;
    b.axis_target[1] = 2000;

    TEST_ASSERT_EQ_INT(0, NcCompensation_ApplyBlockTs(&b, &err));

    TEST_ASSERT_EQ_UINT(1U, g_ncCompensationStatus.cutter_comp_mode);
    TEST_ASSERT_EQ_INT(1000, b.axis_target[0]);
    TEST_ASSERT_EQ_INT(2000, b.axis_target[1]);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_CUTTER_RADIUS) == 0U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_negative_tool_length_offset_subtracts_from_z_target);
    RUN_TEST(test_polar_conversion_uses_rotation_center_without_extra_rotation);
    RUN_TEST(test_rotation_cancel_clears_active_and_angle_but_keeps_center_for_diagnostics);
    RUN_TEST(test_zero_length_cutter_comp_does_not_adjust_target);
    printf("test_nc_compensation_v16_transform_more: PASS\n");
    return 0;
}
