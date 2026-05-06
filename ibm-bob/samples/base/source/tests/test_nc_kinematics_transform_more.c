#include "test_common.h"
#include "system_shared.h"
#include "nc_kinematics.h"
#include "nc_program.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcKinematics_Reset();
}

static NC_EXEC_BLOCK make_motion(uint32_t lineNo)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x07U;
    b.axis_target[0] = 100;
    b.axis_target[1] = 200;
    b.axis_target[2] = 300;
    return b;
}

static int test_axis_assignment_maps_logical_axis_to_physical_axis_with_sign(void)
{
    NC_EXEC_BLOCK b = make_motion(1631U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcKinematics_SetAxisAssignment(0U, 1U, -1, 0U));
    TEST_ASSERT_EQ_INT(0, NcKinematics_SetAxisAssignment(1U, 0U, 1, 0U));
    b.axis_mask = 0x01U;
    b.axis_target[0] = 1234;
    b.axis_target[1] = 0;

    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));

    TEST_ASSERT_TRUE((b.axis_mask & 0x02U) != 0U);
    TEST_ASSERT_EQ_INT(-1234, b.axis_target[1]);
    TEST_ASSERT_EQ_UINT(1U, g_ncKinematicsStatus.transformed_blocks);
    return 0;
}

static int test_mirror_uses_tilt_origin_as_center_for_linear_axis(void)
{
    NC_EXEC_BLOCK b = make_motion(1632U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    g_ncKinematicsStatus.tilt_origin[0] = 1000;
    TEST_ASSERT_EQ_INT(0, NcKinematics_SetMirrorMask(0x01U));
    b.axis_mask = 0x01U;
    b.axis_target[0] = 1200;

    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));

    TEST_ASSERT_EQ_INT(800, b.axis_target[0]);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_MIRROR) != 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ncKinematicsStatus.mirrored_blocks);
    return 0;
}

static int test_tilt_command_sets_warning_near_singularity_without_motion_target_change(void)
{
    NC_EXEC_BLOCK b = make_motion(1633U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    b.kinematics_command = NC_KINEMATICS_CMD_TILT_ENABLE;
    b.motion_type = NC_MOTION_AUX_ONLY;
    b.tilt_angle_deg[1] = 86 * NC_ANGLE_SCALE;

    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));

    TEST_ASSERT_EQ_UINT(1U, g_ncKinematicsStatus.tilted_plane_active);
    TEST_ASSERT_EQ_UINT(1U, g_ncKinematicsStatus.singularity_warnings);
    TEST_ASSERT_EQ_UINT(0U, g_ncKinematicsStatus.transformed_blocks);
    return 0;
}

static int test_retract_and_recover_commands_update_retract_mask_and_event_count(void)
{
    NC_EXEC_BLOCK b = make_motion(1634U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    b.axis_mask = 0x05U;
    b.axis_retract_command = NC_AXIS_RETRACT_CMD_RETRACT;
    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0x05U, g_ncKinematicsStatus.retract_axis_mask);
    TEST_ASSERT_EQ_UINT(1U, g_ncKinematicsStatus.retract_events);

    b.axis_retract_command = NC_AXIS_RETRACT_CMD_RECOVER;
    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0U, g_ncKinematicsStatus.retract_axis_mask);
    TEST_ASSERT_EQ_UINT(2U, g_ncKinematicsStatus.retract_events);
    return 0;
}

int main(void)
{
    RUN_TEST(test_axis_assignment_maps_logical_axis_to_physical_axis_with_sign);
    RUN_TEST(test_mirror_uses_tilt_origin_as_center_for_linear_axis);
    RUN_TEST(test_tilt_command_sets_warning_near_singularity_without_motion_target_change);
    RUN_TEST(test_retract_and_recover_commands_update_retract_mask_and_event_count);
    printf("test_nc_kinematics_transform_more: PASS\n");
    return 0;
}
