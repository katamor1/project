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
    b.axis_mask = 0x07U;
    b.axis_start[0] = 10;
    b.axis_start[1] = 20;
    b.axis_start[2] = 30;
    b.axis_target[0] = 1010;
    b.axis_target[1] = 2020;
    b.axis_target[2] = 3030;
    return b;
}

static int test_detached_axis_is_removed_from_motion_and_target_restored(void)
{
    NC_EXEC_BLOCK b = make_motion(221U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    TEST_ASSERT_EQ_INT(0, NcKinematics_SetAxisAssignment(1U, 1U, 1, 1U));

    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_TRUE((b.axis_mask & 0x02U) == 0U);
    TEST_ASSERT_EQ_INT(20, b.axis_target[1]);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_AXIS_ASSIGNMENT) != 0U);
    return 0;
}

static int test_mirror_enable_then_cancel_clears_status(void)
{
    NC_EXEC_BLOCK b = make_motion(222U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    b.mirror_command = NC_MIRROR_CMD_ENABLE;
    b.mirror_axis_mask = 0x01U;
    b.tilt_origin[0] = 100;
    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_TRUE((g_ncKinematicsStatus.mirror_axis_mask & 0x01U) != 0U);
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_MIRROR) != 0U);

    memset(&b, 0, sizeof(b));
    b.mirror_command = NC_MIRROR_CMD_CANCEL;
    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0U, g_ncKinematicsStatus.mirror_axis_mask);
    return 0;
}

static int test_tilt_cancel_resets_angles_after_enable(void)
{
    NC_EXEC_BLOCK b = make_motion(223U);
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    b.kinematics_command = NC_KINEMATICS_CMD_TILT_ENABLE;
    b.tilt_angle_deg[0] = 10 * NC_ANGLE_SCALE;
    b.tilt_angle_deg[1] = 20 * NC_ANGLE_SCALE;
    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(1U, g_ncKinematicsStatus.tilted_plane_active);

    memset(&b, 0, sizeof(b));
    b.kinematics_command = NC_KINEMATICS_CMD_TILT_CANCEL;
    TEST_ASSERT_EQ_INT(0, NcKinematics_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0U, g_ncKinematicsStatus.tilted_plane_active);
    TEST_ASSERT_EQ_INT(0, g_ncKinematicsStatus.tilt_angle_deg[0]);
    TEST_ASSERT_EQ_INT(0, g_ncKinematicsStatus.tilt_angle_deg[1]);
    return 0;
}

int main(void)
{
    RUN_TEST(test_detached_axis_is_removed_from_motion_and_target_restored);
    RUN_TEST(test_mirror_enable_then_cancel_clears_status);
    RUN_TEST(test_tilt_cancel_resets_angles_after_enable);
    printf("test_nc_kinematics_detach_tilt_cancel: PASS\n");
    return 0;
}
