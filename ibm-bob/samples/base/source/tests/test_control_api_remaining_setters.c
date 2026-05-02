#include "test_common.h"
#include "stub_external_io.h"
#include "control_api.h"

/* Additional API setter/getter coverage for modules that are otherwise tested directly. */

static int test_spindle_tool_lathe_sync_rotary_and_interference_setters(void)
{
    NC_SPINDLE_STATUS spindle;
    NC_TOOL_MANAGEMENT_STATUS tool;
    NC_SYNCHRONIZATION_STATUS sync;
    NC_ROTARY_MCC_STATUS rotary;
    NC_INTERFERENCE_STATUS interference;
    NC_TURNING_CYCLE_STATUS turning;

    Test_ResetSystem();

    TEST_ASSERT_EQ_INT(0, Api_SetNcSpindleSpeedLimit(100U, 5000U));
    TEST_ASSERT_EQ_INT(-1, Api_SetNcSpindleSpeedLimit(6000U, 1000U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcSpindleOrientWindow(77U));
    TEST_ASSERT_EQ_INT(0, Api_GetNcSpindleStatus(&spindle));
    TEST_ASSERT_EQ_UINT(100U, spindle.min_rpm);
    TEST_ASSERT_EQ_UINT(5000U, spindle.max_rpm);
    TEST_ASSERT_EQ_UINT(77U, spindle.orient_timeout_ticks);

    TEST_ASSERT_EQ_INT(0, Api_SetNcToolPocket(3U, 4U));
    TEST_ASSERT_EQ_INT(0, Api_RequestNcToolPrepare(3U));
    TEST_ASSERT_EQ_INT(0, Api_GetNcToolManagementStatus(&tool));
    TEST_ASSERT_EQ_UINT(3U, tool.requested_tool_no);

    TEST_ASSERT_EQ_INT(0, Api_SetNcDiameterMode(1U));
    TEST_ASSERT_EQ_INT(0, Api_GetNcTurningCycleStatus(&turning));
    TEST_ASSERT_EQ_UINT(0U, turning.radius_mode_active);

    TEST_ASSERT_EQ_INT(0, Api_SetNcSynchronizationMasterSlave(0U, 0x06U));
    TEST_ASSERT_EQ_INT(-1, Api_SetNcSynchronizationMasterSlave(AXIS_MAX, 0x01U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcCompoundPathMask(0x03U));
    TEST_ASSERT_EQ_INT(0, Api_SetNcDoubleTableControl(1U, 0x02U));
    TEST_ASSERT_EQ_INT(0, Api_GetNcSynchronizationStatus(&sync));
    TEST_ASSERT_EQ_UINT(1U, sync.double_table_enabled);
    TEST_ASSERT_EQ_UINT(0x02U, sync.slave_axis_mask);

    TEST_ASSERT_EQ_INT(0, Api_SetNcRotaryAxisRadius(3U, 25000));
    TEST_ASSERT_EQ_INT(-1, Api_SetNcRotaryAxisRadius(AXIS_MAX, 1));
    TEST_ASSERT_EQ_INT(0, Api_SetNcMccOutput(0U));
    TEST_ASSERT_EQ_INT(0, Api_GetNcRotaryMccStatus(&rotary));
    TEST_ASSERT_EQ_INT(25000, rotary.radius[3]);
    TEST_ASSERT_EQ_UINT(0U, rotary.mcc_output_enabled);

    TEST_ASSERT_EQ_INT(0, Api_SetNcInterferenceCheckEnabled(0U));
    TEST_ASSERT_EQ_INT(0, Api_GetNcInterferenceStatus(&interference));
    TEST_ASSERT_EQ_UINT(0U, interference.enabled);
    return 0;
}

static int test_remaining_null_getters(void)
{
    Test_ResetSystem();
    TEST_ASSERT_EQ_INT(-1, Api_GetNcInterferenceStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcSafetyMotionStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcTurningCycleStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcThreadCycleStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcSpindleStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcToolManagementStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcSynchronizationStatus(NULL));
    TEST_ASSERT_EQ_INT(-1, Api_GetNcRotaryMccStatus(NULL));
    return 0;
}

int main(void)
{
    RUN_TEST(test_spindle_tool_lathe_sync_rotary_and_interference_setters);
    RUN_TEST(test_remaining_null_getters);
    printf("test_control_api_remaining_setters: PASS\n");
    return 0;
}
