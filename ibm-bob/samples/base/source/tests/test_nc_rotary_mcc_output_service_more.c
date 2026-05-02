#include "test_common.h"
#include "system_shared.h"
#include "nc_rotary_mcc.h"
static int test_mcc_output_is_gated_by_machine_state_and_disable_clears(void){
    SystemShared_Initialize();
    NcRotaryMcc_Reset();
    TEST_ASSERT_EQ_INT(0,NcRotaryMcc_SetMccOutput(1U));
    g_ncRotaryMccStatus.enabled = 1U;
    g_ncRotaryMccStatus.active_axis_mask = 1U;
    g_machineCtx.machine_state = MACHINE_STOPPED;
    g_ncProgramStatus.state = NC_RUNNING;
    NcRotaryMcc_ServiceRt();
    TEST_ASSERT_TRUE((g_ioOut.command_bits & NC_ROTARY_MCC_COMMAND_BIT) == 0U);
    TEST_ASSERT_EQ_INT(0,NcRotaryMcc_SetMccOutput(0U));
    TEST_ASSERT_TRUE((g_ioOut.command_bits & NC_ROTARY_MCC_COMMAND_BIT) == 0U);
    return 0;
}
int main(void){RUN_TEST(test_mcc_output_is_gated_by_machine_state_and_disable_clears);return 0;}
