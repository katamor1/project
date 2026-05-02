#include "test_common.h"
#include "system_shared.h"
#include "rt_control.h"

#define TEST_SENSOR_EMERGENCY_STOP   (0x00000001UL)
#define TEST_SENSOR_PRESSURE_HIGH    (0x00000002UL)
#define TEST_SENSOR_SERVO_ALARM      (0x00000004UL)
#define TEST_SENSOR_READY_SWITCH     (0x00000008UL)
#define TEST_SENSOR_RUN_SWITCH       (0x00000010UL)
#define TEST_CMD_ENABLE_SERVO        (0x00000001UL)
#define TEST_CMD_START_MOTION        (0x00000002UL)
#define TEST_CMD_SAFE_OUTPUT         (0x80000000UL)

static void setup(void)
{
    SystemShared_Initialize();
}

static int test_prestart_interlock_ok_and_ng_masks(void)
{
    setup();
    g_ioIn.sensor_bits = TEST_SENSOR_READY_SWITCH;
    g_ioIn.analog[0] = READY_PRESSURE_MIN;
    g_ncProgramStatus.state = NC_READY;
    g_ncProgramStatus.buffered_blocks = 2U;
    RtControl_UpdatePrestartInterlock();
    TEST_ASSERT_EQ_UINT(1U, g_prestartInterlockStatus.ok);
    TEST_ASSERT_EQ_UINT(0U, g_prestartInterlockStatus.mask);

    g_ioIn.sensor_bits = TEST_SENSOR_READY_SWITCH | TEST_SENSOR_EMERGENCY_STOP | TEST_SENSOR_SERVO_ALARM;
    g_ioIn.analog[0] = READY_PRESSURE_MIN - 1;
    g_machineCtx.machine_state = MACHINE_ALARM;
    g_ncProgramStatus.buffered_blocks = 0U;
    RtControl_UpdatePrestartInterlock();
    TEST_ASSERT_EQ_UINT(0U, g_prestartInterlockStatus.ok);
    TEST_ASSERT_TRUE((g_prestartInterlockStatus.mask & PRESTART_ILK_ESTOP) != 0U);
    TEST_ASSERT_TRUE((g_prestartInterlockStatus.mask & PRESTART_ILK_SERVO_ALARM) != 0U);
    TEST_ASSERT_TRUE((g_prestartInterlockStatus.mask & PRESTART_ILK_PRESSURE_RANGE) != 0U);
    TEST_ASSERT_TRUE((g_prestartInterlockStatus.mask & PRESTART_ILK_MACHINE_ALARM) != 0U);
    TEST_ASSERT_TRUE((g_prestartInterlockStatus.mask & PRESTART_ILK_NC_BUFFER) != 0U);
    return 0;
}

static int test_machine_state_transitions_and_axis_outputs(void)
{
    setup();
    g_ioIn.sensor_bits = TEST_SENSOR_READY_SWITCH;
    g_ioIn.analog[0] = READY_PRESSURE_MIN;
    RtControl_UpdateMachineState();
    TEST_ASSERT_EQ_UINT(MACHINE_READY, g_machineCtx.machine_state);
    RtControl_UpdateAxisTargets();
    TEST_ASSERT_EQ_UINT(TEST_CMD_ENABLE_SERVO, g_ioOut.command_bits);
    TEST_ASSERT_EQ_INT(100, g_ioOut.axis_target[0]);

    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_ioIn.sensor_bits = TEST_SENSOR_READY_SWITCH | TEST_SENSOR_RUN_SWITCH;
    RtControl_UpdateMachineState();
    TEST_ASSERT_EQ_UINT(MACHINE_RUNNING, g_machineCtx.machine_state);
    RtControl_UpdateAxisTargets();
    TEST_ASSERT_EQ_UINT(TEST_CMD_ENABLE_SERVO | TEST_CMD_START_MOTION, g_ioOut.command_bits);
    TEST_ASSERT_EQ_INT(1000, g_ioOut.axis_target[0]);

    g_ioIn.sensor_bits = TEST_SENSOR_READY_SWITCH;
    RtControl_UpdateMachineState();
    TEST_ASSERT_EQ_UINT(MACHINE_STOPPED, g_machineCtx.machine_state);
    return 0;
}

static int test_mode_request_accept_reject_and_param_error(void)
{
    setup();
    g_uiRequest.mode_change_request = 1U;
    g_uiRequest.requested_mode = RUN_MODE_AUTO;
    g_machineCtx.machine_state = MACHINE_READY;
    g_prestartInterlockStatus.ok = 1U;
    RtControl_ProcessModeRequest();
    TEST_ASSERT_EQ_UINT(RUN_MODE_AUTO, g_machineCtx.run_mode);
    TEST_ASSERT_EQ_UINT(RESPONSE_COMPLETED, g_uiRequest.mode_response);
    TEST_ASSERT_EQ_UINT(0U, g_uiRequest.mode_change_request);

    g_uiRequest.mode_change_request = 1U;
    g_uiRequest.requested_mode = RUN_MODE_MAINT;
    g_machineCtx.machine_state = MACHINE_RUNNING;
    RtControl_ProcessModeRequest();
    TEST_ASSERT_EQ_UINT(RESPONSE_REJECTED, g_uiRequest.mode_response);

    g_uiRequest.mode_change_request = 1U;
    g_uiRequest.requested_mode = 99U;
    RtControl_ProcessModeRequest();
    TEST_ASSERT_EQ_UINT(RESPONSE_PARAM_ERROR, g_uiRequest.mode_response);
    return 0;
}

static int test_interlock_forces_alarm_and_safe_output(void)
{
    setup();
    g_machineCtx.machine_state = MACHINE_RUNNING;
    g_ioIn.sensor_bits = TEST_SENSOR_EMERGENCY_STOP;
    g_ioIn.analog[0] = READY_PRESSURE_MIN;
    RtControl_UpdateInterlock();
    TEST_ASSERT_EQ_UINT(MACHINE_ALARM, g_machineCtx.machine_state);
    TEST_ASSERT_EQ_UINT(9001U, g_machineCtx.alarm_code);
    TEST_ASSERT_EQ_UINT(TEST_CMD_SAFE_OUTPUT, g_ioOut.command_bits);
    TEST_ASSERT_EQ_UINT(1U, g_logQueue.tail);
    TEST_ASSERT_EQ_UINT(LOG_ALARM, g_logQueue.items[0].event_type);
    return 0;
}

static int test_io_trace_ring_and_alarm_freeze(void)
{
    setup();
    g_machineCtx.cycle_count = 12U;
    g_ioIn.sensor_bits = TEST_SENSOR_READY_SWITCH;
    g_ioOut.command_bits = TEST_CMD_ENABLE_SERVO;
    RtControl_CaptureIoTrace();
    TEST_ASSERT_EQ_UINT(1U, g_ioTraceBuffer.captured_count);
    TEST_ASSERT_EQ_UINT(1U, g_ioTraceBuffer.write_index);
    TEST_ASSERT_EQ_UINT(0U, g_ioTraceBuffer.frozen);

    g_machineCtx.machine_state = MACHINE_ALARM;
    g_machineCtx.alarm_code = 9001U;
    g_machineCtx.cycle_count = 13U;
    RtControl_CaptureIoTrace();
    TEST_ASSERT_EQ_UINT(1U, g_ioTraceBuffer.frozen);
    TEST_ASSERT_EQ_UINT(13U, g_ioTraceBuffer.trigger_cycle);
    TEST_ASSERT_EQ_UINT(2U, g_ioTraceBuffer.captured_count);
    RtControl_CaptureIoTrace();
    TEST_ASSERT_EQ_UINT(2U, g_ioTraceBuffer.captured_count);
    return 0;
}

int main(void)
{
    RUN_TEST(test_prestart_interlock_ok_and_ng_masks);
    RUN_TEST(test_machine_state_transitions_and_axis_outputs);
    RUN_TEST(test_mode_request_accept_reject_and_param_error);
    RUN_TEST(test_interlock_forces_alarm_and_safe_output);
    RUN_TEST(test_io_trace_ring_and_alarm_freeze);
    printf("test_rt_control: PASS\n");
    return 0;
}
