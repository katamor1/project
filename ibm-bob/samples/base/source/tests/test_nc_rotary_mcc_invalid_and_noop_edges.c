#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_rotary_mcc.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcRotaryMcc_Reset();
}

static int test_apply_rejects_null_arguments(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    memset(&b, 0, sizeof(b));
    TEST_ASSERT_EQ_INT(-1, NcRotaryMcc_ApplyBlockTs(NULL, &err));
    TEST_ASSERT_EQ_INT(-1, NcRotaryMcc_ApplyBlockTs(&b, NULL));
    return 0;
}

static int test_axis_radius_invalid_bounds_increment_reject_count(void)
{
    setup();
    TEST_ASSERT_EQ_INT(-1, NcRotaryMcc_SetAxisRadius(AXIS_MAX, NC_ROTARY_MCC_DEFAULT_RADIUS));
    TEST_ASSERT_EQ_INT(-1, NcRotaryMcc_SetAxisRadius(0U, NC_ROTARY_MCC_MIN_RADIUS - 1));
    TEST_ASSERT_EQ_INT(-1, NcRotaryMcc_SetAxisRadius(0U, NC_ROTARY_MCC_MAX_RADIUS + 1));
    TEST_ASSERT_EQ_UINT(3U, g_ncRotaryMccStatus.rejected_calls);

    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_SetAxisRadius(2U, NC_ROTARY_MCC_MIN_RADIUS));
    TEST_ASSERT_EQ_INT(NC_ROTARY_MCC_MIN_RADIUS, g_ncRotaryMccStatus.radius[2]);
    TEST_ASSERT_TRUE((g_ncRotaryMccStatus.configured_axis_mask & (1UL << 2U)) != 0U);
    return 0;
}

static int test_output_disable_clears_command_bit_and_cancel_gcode_disables_mode(void)
{
    NC_EXEC_BLOCK b;
    NC_ERROR_CODE err = NC_ERR_NONE;
    setup();
    g_ioOut.command_bits |= NC_ROTARY_MCC_COMMAND_BIT;
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_SetMccOutput(0U));
    TEST_ASSERT_EQ_UINT(0U, g_ioOut.command_bits & NC_ROTARY_MCC_COMMAND_BIT);
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.mcc_output_enabled);
    TEST_ASSERT_TRUE(g_ncRotaryMccStatus.mcc_off_events >= 1U);

    memset(&b, 0, sizeof(b));
    b.line_no = 512U;
    b.g_code10 = (uint16_t)NC_G_CODE10(13, 1);
    b.motion_type = NC_MOTION_LINEAR;
    b.axis_mask = 0x08U;
    b.axis_target[3] = 360 * NC_ANGLE_SCALE;
    TEST_ASSERT_EQ_INT(0, NcRotaryMcc_ApplyBlockTs(&b, &err));
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.enabled);
    TEST_ASSERT_EQ_UINT(0U, g_ncRotaryMccStatus.active_axis_mask);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_ROTARY_MCC_MODE, g_ncEventRing.items[0].code);
    return 0;
}

int main(void)
{
    RUN_TEST(test_apply_rejects_null_arguments);
    RUN_TEST(test_axis_radius_invalid_bounds_increment_reject_count);
    RUN_TEST(test_output_disable_clears_command_bit_and_cancel_gcode_disables_mode);
    printf("test_nc_rotary_mcc_invalid_and_noop_edges: PASS\n");
    return 0;
}
