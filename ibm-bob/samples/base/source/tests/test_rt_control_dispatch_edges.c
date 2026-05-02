#include "test_common.h"
#include "rt_control.h"
#include "system_shared.h"

static int test_dispatch_cycle_updates_timing_and_snapshot(void)
{
    SystemShared_Initialize();
    g_ioIn.sensor_bits = 0x08U; /* ready switch */
    g_ioIn.analog[0] = 200;
    g_machineCtx.machine_state = MACHINE_READY;
    RtDispatcher_ExecuteCycle();
    TEST_ASSERT_EQ_UINT(1U, g_machineCtx.cycle_count);
    TEST_ASSERT_EQ_UINT(1U, g_machineCtx.rt_input_ticks);
    TEST_ASSERT_EQ_UINT(2U, g_machineCtx.rt_control_ticks);
    TEST_ASSERT_EQ_UINT(1U, g_machineCtx.rt_output_ticks);
    TEST_ASSERT_EQ_UINT(4U, g_machineCtx.rt_total_ticks);
    TEST_ASSERT_TRUE(g_ncDiagnosticSnapshot.generation > 0U);
    TEST_ASSERT_TRUE(g_ioTraceBuffer.captured_count > 0U);
    return 0;
}

static int test_alarm_cycle_freezes_trace_and_safe_output(void)
{
    SystemShared_Initialize();
    g_machineCtx.machine_state = MACHINE_ALARM;
    g_machineCtx.alarm_code = 123U;
    RtDispatcher_ExecuteCycle();
    TEST_ASSERT_TRUE((g_ioOut.command_bits & 0x80000000UL) != 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ioTraceBuffer.frozen);
    TEST_ASSERT_EQ_UINT(123U, g_ioTraceBuffer.trigger_alarm_code);
    return 0;
}

int main(void)
{
    RUN_TEST(test_dispatch_cycle_updates_timing_and_snapshot);
    RUN_TEST(test_alarm_cycle_freezes_trace_and_safe_output);
    printf("test_rt_control_dispatch_edges: PASS\n");
    return 0;
}
