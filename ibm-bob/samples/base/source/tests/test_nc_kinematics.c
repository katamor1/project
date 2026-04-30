#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_kinematics.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcKinematics_Reset();
}

static NC_EXEC_BLOCK make_motion(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x03U;
    b.axis_target[0] = 1000;
    b.axis_target[1] = 2000;
    return b;
}

static int test_axis_assignment_boundaries_and_sign(void)
{
    NC_EXEC_BLOCK b = make_motion(121U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(-1, NcKinematics_SetAxisAssignment(AXIS_MAX, 0U, 1, 0U));
    TEST_ASSERT_EQ_INT(-1, NcKinematics_SetAxisAssignment(0U, AXIS_MAX, 1, 0U));
    TEST_ASSERT_EQ_INT(-1, NcKinematics_SetAxisAssignment(0U, 1U, 0, 0U));
    TEST_ASSERT_EQ_INT(0, NcKinematics_SetAxisAssignment(0U, 1U, -1, 0U));
    TEST_ASSERT_EQ_INT(0, NcKinematics_SetAxisAssignment(1U, 0U, 1, 0U));

    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_INT(-1000, b.axis_target[1]);
    TEST_ASSERT_EQ_INT(2000, b.axis_target[0]);
    TEST_ASSERT_TRUE((b.axis_mask & 0x03U) == 0x03U);
    return 0;
}

static int test_mirror_and_retract_transform_block(void)
{
    NC_EXEC_BLOCK b = make_motion(122U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcKinematics_SetMirrorMask(0x01U));
    TEST_ASSERT_TRUE((g_ncKinematicsStatus.mirror_axis_mask & 0x01U) != 0U);
    b.mirror_command = NC_MIRROR_CMD_ENABLE;
    b.mirror_axis_mask = 0x01U;
    b.axis_retract_command = NC_AXIS_RETRACT_CMD_RETRACT;
    b.axis_mask = 0x03U;

    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_TRUE((g_ncKinematicsStatus.retract_axis_mask & 0x03U) == 0x03U);
    TEST_ASSERT_TRUE(g_ncKinematicsStatus.retract_events >= 1U);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_MIRROR) != 0U);
    return 0;
}

static int test_tilt_and_tool_axis_singularity_warning(void)
{
    NC_EXEC_BLOCK b = make_motion(123U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
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
    RUN_TEST(test_axis_assignment_boundaries_and_sign);
    RUN_TEST(test_mirror_and_retract_transform_block);
    RUN_TEST(test_tilt_and_tool_axis_singularity_warning);
    printf("test_nc_kinematics: PASS\n");
    return 0;
}
