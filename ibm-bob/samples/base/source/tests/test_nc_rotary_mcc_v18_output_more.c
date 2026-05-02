#include "test_common.h"
#include "system_shared.h"
#include "nc_rotary_mcc.h"
#include "nc_program.h"

static NC_EXEC_BLOCK make_rotary_block(uint32_t line)
{
    NC_EXEC_BLOCK b;
    memset(&b, 0, sizeof(b));
    b.line_no = line;
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x08U;
    b.axis_start[3] = 0;
    b.axis_target[3] = 360 * NC_ANGLE_SCALE;
    return b;
}

static int test_invalid_and_valid_radius_calls_update_counters(void)
{
    SystemShared_Initialize();
    NcRotaryMcc_Reset();
    TEST_ASSERT_EQ_INT(-1, NcRotaryMcc_SetAxisRadius((uint8_t)AXIS_MAX, NC_ROTARY_MCC_DEFAULT_RADIUS));
    TEST_ASSERT_EQ_UINT(1U, g_ncRotaryMccStatus.rejected_calls);
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_SetAxisRadius(2U, NC_ROTARY_MCC_MIN_RADIUS));
    TEST_ASSERT_EQ_INT(NC_ROTARY_MCC_MIN_RADIUS, g_ncRotaryMccStatus.radius[2]);
    TEST_ASSERT_EQ_UINT(2U, g_ncRotaryMccStatus.active_rotary_axis);
    return 0;
}

static int test_mode_enable_and_disable_g_codes_change_enabled_state(void)
{
    NC_ERROR_CODE err = NC_ERR_NONE;
    NC_EXEC_BLOCK b = make_rotary_block(1840U);
    SystemShared_Initialize();
    NcRotaryMcc_Reset();
    b.g_code10 = NC_G_CODE10(7, 1);
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(1U, g_ncRotaryMccStatus.enabled);
    b.g_code10 = NC_G_CODE10(13, 1);
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.enabled);
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.active_axis_mask);
    return 0;
}

static int test_motion_on_rotary_axis_records_virtual_delta(void)
{
    NC_ERROR_CODE err = NC_ERR_NONE;
    NC_EXEC_BLOCK b = make_rotary_block(1841U);
    SystemShared_Initialize();
    NcRotaryMcc_Reset();
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_SetAxisRadius(3U, 1000));
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_ApplyBlockTs(&b, &err));
    TEST_ASSERT_TRUE((b.feature_flags & NC_FEATURE_FLAG_ADVANCED_INTERP) != 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ncRotaryMccStatus.parsed_virtual_speed_blocks);
    TEST_ASSERT_EQ_INT(360 * NC_ANGLE_SCALE, g_ncRotaryMccStatus.last_rotary_delta);
    TEST_ASSERT_TRUE(g_ncRotaryMccStatus.last_virtual_linear_delta > 0);
    return 0;
}

static int test_service_output_requires_running_nc_state_and_enabled_output(void)
{
    NC_EXEC_BLOCK b = make_rotary_block(1842U);
    SystemShared_Initialize();
    NcRotaryMcc_Reset();
    NcRotaryMcc_OnBlockRt(&b);
    g_machineCtx.machine_state = MACHINE_READY;
    g_ncProgramStatus.state = NC_RUNNING;
    NcRotaryMcc_ServiceRt();
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.mcc_output_active);
    g_machineCtx.machine_state = MACHINE_RUNNING;
    NcRotaryMcc_ServiceRt();
    TEST_ASSERT_EQ_UINT(1U, g_ncRotaryMccStatus.mcc_output_active);
    TEST_ASSERT_TRUE((g_ioOut.command_bits & NC_ROTARY_MCC_COMMAND_BIT) != 0U);
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_SetMccOutput(0U));
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.mcc_output_active);
    return 0;
}

int main(void)
{
    RUN_TEST(test_invalid_and_valid_radius_calls_update_counters);
    RUN_TEST(test_mode_enable_and_disable_g_codes_change_enabled_state);
    RUN_TEST(test_motion_on_rotary_axis_records_virtual_delta);
    RUN_TEST(test_service_output_requires_running_nc_state_and_enabled_output);
    printf("test_nc_rotary_mcc_v18_output_more: PASS\n");
    return 0;
}
