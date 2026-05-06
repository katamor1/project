#include "test_common.h"
#include "system_shared.h"
#include "nc_kinematics.h"

static NC_EXEC_BLOCK make_motion(uint32_t line, uint32_t mask)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = line;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = mask;
    b.axis_target[0] = 100;
    b.axis_target[1] = 200;
    b.axis_target[2] = 300;
    b.axis_start[0] = 10;
    b.axis_start[1] = 20;
    b.axis_start[2] = 30;
    return b;
}

static int test_axis_assignment_maps_logical_axis_to_negative_physical_axis(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcKinematics_Reset();
    b = make_motion(1931U, 0x01U);
    TEST_ASSERT_EQ_INT(0, NcKinematics_SetAxisAssignment(0U, 1U, -1, 0U));
    TEST_ASSERT_EQ_INT(0, NcKinematics_SetAxisAssignment(1U, 0U, 1, 0U));
    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0x02U, b.axis_mask);
    TEST_ASSERT_EQ_INT(-100, b.axis_target[1]);
    TEST_ASSERT_EQ_INT(-10, b.axis_start[1]);
    return 0;
}

static int test_detached_axis_assignment_removes_axis_from_motion_mask(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcKinematics_Reset();
    b = make_motion(1932U, 0x01U);
    TEST_ASSERT_EQ_INT(0, NcKinematics_SetAxisAssignment(0U, 0U, 1, 1U));
    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0x00U, b.axis_mask);
    TEST_ASSERT_EQ_INT(10, b.axis_target[0]);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_AXIS_ASSIGNMENT) != 0U);
    return 0;
}

static int test_mirror_command_uses_tilt_origin_as_center(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcKinematics_Reset();
    b = make_motion(1933U, 0x01U);
    b.mirror_command = NC_MIRROR_CMD_ENABLE;
    b.mirror_axis_mask = 0x01U;
    b.tilt_origin[0] = 1000;
    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(-100, b.axis_target[0]);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_MIRROR) != 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ncKinematicsStatus.mirrored_blocks);
    return 0;
}

static int test_tool_axis_and_tilt_commands_record_singularity_warnings(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    SystemShared_Initialize();
    NcKinematics_Reset();
    b = make_motion(1934U, 0x00U);
    b.kinematics_command = NC_KINEMATICS_CMD_TILT_ENABLE;
    b.tilt_angle_deg[1] = 86 * NC_ANGLE_SCALE;
    b.tool_axis_command = NC_TOOL_AXIS_CMD_ENABLE;
    b.tool_axis_angle_deg[1] = 86 * NC_ANGLE_SCALE;
    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(1U, g_ncKinematicsStatus.tilted_plane_active);
    TEST_ASSERT_EQ_UINT(1U, g_ncKinematicsStatus.tool_axis_active);
    TEST_ASSERT_TRUE(g_ncKinematicsStatus.singularity_warnings >= 2U);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_assignment_maps_logical_axis_to_negative_physical_axis);
    RUN_TEST(test_detached_axis_assignment_removes_axis_from_motion_mask);
    RUN_TEST(test_mirror_command_uses_tilt_origin_as_center);
    RUN_TEST(test_tool_axis_and_tilt_commands_record_singularity_warnings);
    printf("test_nc_kinematics_commands_more: PASS\n");
    return 0;
}
