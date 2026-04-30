#include "test_common.h"
#include "stub_external_io.h"
#include "nc_codes.h"
#include "nc_rotary_mcc.h"

#define GD(whole, dec) NC_G_CODE10((whole), (dec))

static int test_rotary_mcc_reset_radius_and_output_validation(void)
{
    Test_ResetSystem();
    NcRotaryMcc_Reset();

    TEST_ASSERT_EQ_UINT(1U, g_ncRotaryMccStatus.enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncRotaryMccStatus.mcc_output_enabled);
    TEST_ASSERT_EQ_UINT(3U, g_ncRotaryMccStatus.active_rotary_axis);
    TEST_ASSERT_EQ_INT(NC_ROTARY_MCC_DEFAULT_RADIUS, g_ncRotaryMccStatus.radius[0]);

    TEST_ASSERT_EQ_INT(-1, NcRotaryMcc_SetAxisRadius(AXIS_MAX, NC_ROTARY_MCC_DEFAULT_RADIUS));
    TEST_ASSERT_EQ_INT(-1, NcRotaryMcc_SetAxisRadius(3U, (int32_t)(NC_ROTARY_MCC_MIN_RADIUS - 1)));
    TEST_ASSERT_TRUE(g_ncRotaryMccStatus.rejected_calls >= 2U);

    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_SetAxisRadius(3U, 72000));
    TEST_ASSERT_EQ_INT(72000, g_ncRotaryMccStatus.radius[3]);
    TEST_ASSERT_TRUE((g_ncRotaryMccStatus.configured_axis_mask & 0x8U) != 0U);

    g_ioOut.command_bits = NC_ROTARY_MCC_COMMAND_BIT;
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_SetMccOutput(0U));
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.mcc_output_enabled);
    TEST_ASSERT_TRUE((g_ioOut.command_bits & NC_ROTARY_MCC_COMMAND_BIT) == 0U);
    return 0;
}

static int test_apply_block_mode_commands_and_virtual_speed_status(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;

    Test_ResetSystem();
    NcRotaryMcc_Reset();
    block = Test_MakeMotionBlock(80U, NC_MOTION_LINEAR, 0x8U, 0, 0, 0, 360 * NC_ANGLE_SCALE);
    block.axis_start[3] = 0;
    block.g_code10 = (uint16_t)GD(7, 1);

    TEST_ASSERT_EQ_INT(-1, NcRotaryMcc_ApplyBlockTs(NULL, &err));
    TEST_ASSERT_EQ_INT(-1, NcRotaryMcc_ApplyBlockTs(&block, NULL));
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_ApplyBlockTs(&block, &err));
    TEST_ASSERT_EQ_UINT(1U, g_ncRotaryMccStatus.enabled);
    TEST_ASSERT_EQ_UINT(1U, g_ncRotaryMccStatus.parsed_virtual_speed_blocks);
    TEST_ASSERT_EQ_INT(360 * NC_ANGLE_SCALE, g_ncRotaryMccStatus.last_rotary_delta);
    TEST_ASSERT_EQ_INT(NC_ROTARY_MCC_DEFAULT_RADIUS, g_ncRotaryMccStatus.last_virtual_linear_delta);
    TEST_ASSERT_TRUE((block.feature_flags & NC_FEATURE_FLAG_ADVANCED_INTERP) != 0U);

    block.g_code10 = (uint16_t)GD(13, 1);
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_ApplyBlockTs(&block, &err));
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.enabled);
    return 0;
}

static int test_on_block_and_service_rt_output_follows_machine_state(void)
{
    NC_EXEC_BLOCK block;

    Test_ResetSystem();
    NcRotaryMcc_Reset();
    block = Test_MakeMotionBlock(81U, NC_MOTION_LINEAR, 0x8U, 0, 0, 0, 1000);
    block.axis_start[3] = 0;

    NcRotaryMcc_OnBlockRt(NULL);
    NcRotaryMcc_OnBlockRt(&block);
    TEST_ASSERT_EQ_UINT(1U, g_ncRotaryMccStatus.executed_virtual_speed_blocks);
    TEST_ASSERT_TRUE((g_ncRotaryMccStatus.active_axis_mask & 0x8U) != 0U);

    g_machineCtx.machine_state = MACHINE_RUNNING;
    g_ncProgramStatus.state = NC_RUNNING;
    NcRotaryMcc_ServiceRt();
    TEST_ASSERT_TRUE((g_ioOut.command_bits & NC_ROTARY_MCC_COMMAND_BIT) != 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ncRotaryMccStatus.mcc_output_active);

    g_machineCtx.machine_state = MACHINE_ALARM;
    NcRotaryMcc_ServiceRt();
    TEST_ASSERT_TRUE((g_ioOut.command_bits & NC_ROTARY_MCC_COMMAND_BIT) == 0U);
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.mcc_output_active);
    return 0;
}

int main(void)
{
    RUN_TEST(test_rotary_mcc_reset_radius_and_output_validation);
    RUN_TEST(test_apply_block_mode_commands_and_virtual_speed_status);
    RUN_TEST(test_on_block_and_service_rt_output_follows_machine_state);
    printf("[  PASSED  ] test_nc_rotary_mcc\n");
    return 0;
}
