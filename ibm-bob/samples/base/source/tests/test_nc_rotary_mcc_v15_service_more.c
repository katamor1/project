#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_rotary_mcc.h"
#include "nc_codes.h"

#define GD(w,d) NC_G_CODE10((w),(d))

static void setup(void)
{
    SystemShared_Initialize();
    NcRotaryMcc_Reset();
}

static NC_EXEC_BLOCK rotary_block(uint32_t lineNo, int32_t aTarget)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = lineNo;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x08U;
    b.axis_start[3] = 0;
    b.axis_target[3] = aTarget;
    return b;
}

static int test_radius_accepts_max_boundary_and_sets_active_axis(void)
{
    setup();
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_SetAxisRadius(2U, NC_ROTARY_MCC_MAX_RADIUS));
    TEST_ASSERT_EQ_UINT(2U, g_ncRotaryMccStatus.active_rotary_axis);
    TEST_ASSERT_EQ_INT(NC_ROTARY_MCC_MAX_RADIUS, g_ncRotaryMccStatus.radius[2]);
    TEST_ASSERT_TRUE((g_ncRotaryMccStatus.configured_axis_mask & 0x04U) != 0U);
    return 0;
}

static int test_zero_rotary_delta_is_noop_for_virtual_speed_counter(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    b = rotary_block(41U, 0);
    b.g_code10 = (uint16_t)GD(7, 1);
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.parsed_virtual_speed_blocks);
    TEST_ASSERT_EQ_UINT(GD(7, 1), g_ncRotaryMccStatus.last_g_code10);
    return 0;
}

static int test_non_motion_mode_command_changes_mode_but_does_not_parse_virtual_speed(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    memset(&b, 0, sizeof(b));
    b.line_no = 42U;
    b.motion_type = NC_MOTION_AUX_ONLY;
    b.g_code10 = (uint16_t)GD(13, 1);
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.enabled);
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.parsed_virtual_speed_blocks);
    return 0;
}

static int test_service_turns_output_off_when_nc_state_leaves_running(void)
{
    NC_EXEC_BLOCK b;
    setup();
    b = rotary_block(43U, 360 * NC_ANGLE_SCALE);
    NcRotaryMcc_OnBlockRt(&b);
    g_machineCtx.machine_state = MACHINE_RUNNING;
    g_ncProgramStatus.state = NC_RUNNING;
    NcRotaryMcc_ServiceRt();
    TEST_ASSERT_TRUE((g_ioOut.command_bits & NC_ROTARY_MCC_COMMAND_BIT) != 0U);
    g_ncProgramStatus.state = NC_HOLD;
    NcRotaryMcc_ServiceRt();
    TEST_ASSERT_TRUE((g_ioOut.command_bits & NC_ROTARY_MCC_COMMAND_BIT) == 0U);
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.mcc_output_active);
    return 0;
}

int main(void)
{
    RUN_TEST(test_radius_accepts_max_boundary_and_sets_active_axis);
    RUN_TEST(test_zero_rotary_delta_is_noop_for_virtual_speed_counter);
    RUN_TEST(test_non_motion_mode_command_changes_mode_but_does_not_parse_virtual_speed);
    RUN_TEST(test_service_turns_output_off_when_nc_state_leaves_running);
    printf("test_nc_rotary_mcc_v15_service_more: PASS\n");
    return 0;
}
