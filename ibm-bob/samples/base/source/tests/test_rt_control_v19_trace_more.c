#include "test_common.h"
#include "system_shared.h"
#include "rt_control.h"

#define SENSOR_READY_SWITCH     (0x00000008UL)
#define CMD_ENABLE_SERVO        (0x00000001UL)
#define CMD_START_MOTION        (0x00000002UL)
#define CMD_SAFE_OUTPUT         (0x80000000UL)

static int test_prestart_reports_nc_buffer_requirement_when_ready_without_blocks(void)
{
    SystemShared_Initialize();
    g_ioIn.sensor_bits = SENSOR_READY_SWITCH;
    g_ioIn.analog[0] = READY_PRESSURE_MIN;
    g_ncProgramStatus.state = NC_READY;
    g_ncProgramStatus.buffered_blocks = 0U;
    RtControl_UpdatePrestartInterlock();
    TEST_ASSERT_TRUE((g_prestartInterlockStatus.mask & PRESTART_ILK_NC_BUFFER) != 0U);
    TEST_ASSERT_EQ_UINT(0U, g_prestartInterlockStatus.ok);
    return 0;
}

static int test_io_trace_freezes_on_alarm_and_preserves_trigger_metadata(void)
{
    uint32_t count;
    SystemShared_Initialize();
    g_machineCtx.cycle_count = 77U;
    g_machineCtx.machine_state = MACHINE_ALARM;
    g_machineCtx.alarm_code = 9001U;
    RtControl_CaptureIoTrace();
    TEST_ASSERT_EQ_UINT(1U, g_ioTraceBuffer.frozen);
    TEST_ASSERT_EQ_UINT(77U, g_ioTraceBuffer.trigger_cycle);
    TEST_ASSERT_EQ_UINT(9001U, g_ioTraceBuffer.trigger_alarm_code);
    count = g_ioTraceBuffer.captured_count;
    g_machineCtx.cycle_count++;
    RtControl_CaptureIoTrace();
    TEST_ASSERT_EQ_UINT(count, g_ioTraceBuffer.captured_count);
    return 0;
}

static int test_axis_targets_include_prefetch_result_when_generation_matches(void)
{
    SystemShared_Initialize();
    g_machineCtx.machine_state = MACHINE_READY;
    g_prefetchReq.ready = 1U;
    g_prefetchReq.generation = 5U;
    g_prefetchResult.generation = 5U;
    g_prefetchResult.table[0] = 3.0F;
    RtControl_UpdateAxisTargets();
    TEST_ASSERT_TRUE((g_ioOut.command_bits & CMD_ENABLE_SERVO) != 0U);
    TEST_ASSERT_EQ_INT(130, g_ioOut.axis_target[0]);
    TEST_ASSERT_EQ_INT(155, g_ioOut.axis_target[1]);
    return 0;
}

static int test_output_commit_sets_lamps_for_running_alarm_and_idle_states(void)
{
    SystemShared_Initialize();
    g_machineCtx.machine_state = MACHINE_RUNNING;
    RtOutput_CommitOutputs();
    TEST_ASSERT_EQ_UINT(0x00000002UL, g_ioOut.lamp_bits);
    g_machineCtx.machine_state = MACHINE_ALARM;
    g_ioOut.command_bits = 0U;
    RtOutput_CommitOutputs();
    TEST_ASSERT_TRUE((g_ioOut.command_bits & CMD_SAFE_OUTPUT) != 0U);
    TEST_ASSERT_EQ_UINT(0x00000004UL, g_ioOut.lamp_bits);
    g_machineCtx.machine_state = MACHINE_STOPPED;
    RtOutput_CommitOutputs();
    TEST_ASSERT_EQ_UINT(0x00000001UL, g_ioOut.lamp_bits);
    (void)CMD_START_MOTION;
    return 0;
}

int main(void)
{
    RUN_TEST(test_prestart_reports_nc_buffer_requirement_when_ready_without_blocks);
    RUN_TEST(test_io_trace_freezes_on_alarm_and_preserves_trigger_metadata);
    RUN_TEST(test_axis_targets_include_prefetch_result_when_generation_matches);
    RUN_TEST(test_output_commit_sets_lamps_for_running_alarm_and_idle_states);
    printf("test_rt_control_v19_trace_more: PASS\n");
    return 0;
}
