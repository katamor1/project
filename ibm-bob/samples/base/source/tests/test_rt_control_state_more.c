#include "test_common.h"
#include "system_shared.h"
#include "rt_control.h"

#define SENSOR_EMERGENCY_STOP   (0x00000001UL)
#define SENSOR_PRESSURE_HIGH    (0x00000002UL)
#define SENSOR_READY_SWITCH     (0x00000008UL)
#define SENSOR_RUN_SWITCH       (0x00000010UL)
#define CMD_ENABLE_SERVO        (0x00000001UL)
#define CMD_START_MOTION        (0x00000002UL)
#define CMD_SAFE_OUTPUT         (0x80000000UL)

static void setup(void)
{
    SystemShared_Initialize();
}

static int test_prestart_interlock_ok_when_ready_switch_pressure_and_no_buffer_requirement(void)
{
    setup();
    g_ioIn.sensor_bits = SENSOR_READY_SWITCH;
    g_ioIn.analog[0] = READY_PRESSURE_MIN;
    g_ncProgramStatus.state = NC_IDLE;
    RtControl_UpdatePrestartInterlock();
    TEST_ASSERT_EQ_UINT(0U, g_prestartInterlockStatus.mask);
    TEST_ASSERT_EQ_UINT(1U, g_prestartInterlockStatus.ok);
    return 0;
}

static int test_machine_state_transitions_stopped_ready_running_stopped(void)
{
    setup();
    g_ioIn.sensor_bits = SENSOR_READY_SWITCH;
    g_ioIn.analog[0] = READY_PRESSURE_MIN;
    RtControl_UpdateMachineState();
    TEST_ASSERT_EQ_UINT(MACHINE_READY, g_machineCtx.machine_state);

    g_machineCtx.run_mode = RUN_MODE_AUTO;
    g_ioIn.sensor_bits = SENSOR_READY_SWITCH | SENSOR_RUN_SWITCH;
    RtControl_UpdateMachineState();
    TEST_ASSERT_EQ_UINT(MACHINE_RUNNING, g_machineCtx.machine_state);

    g_ioIn.sensor_bits = SENSOR_READY_SWITCH;
    RtControl_UpdateMachineState();
    TEST_ASSERT_EQ_UINT(MACHINE_STOPPED, g_machineCtx.machine_state);
    return 0;
}

static int test_process_mode_request_rejects_auto_unless_ready_and_interlock_ok(void)
{
    setup();
    g_uiRequest.mode_change_request = 1U;
    g_uiRequest.requested_mode = RUN_MODE_AUTO;
    g_machineCtx.machine_state = MACHINE_STOPPED;
    g_prestartInterlockStatus.ok = 1U;
    RtControl_ProcessModeRequest();
    TEST_ASSERT_EQ_UINT(RESPONSE_REJECTED, g_uiRequest.mode_response);
    TEST_ASSERT_EQ_UINT(RUN_MODE_MANUAL, g_machineCtx.run_mode);

    g_uiRequest.mode_change_request = 1U;
    g_uiRequest.requested_mode = RUN_MODE_AUTO;
    g_machineCtx.machine_state = MACHINE_READY;
    g_prestartInterlockStatus.ok = 1U;
    RtControl_ProcessModeRequest();
    TEST_ASSERT_EQ_UINT(RESPONSE_COMPLETED, g_uiRequest.mode_response);
    TEST_ASSERT_EQ_UINT(RUN_MODE_AUTO, g_machineCtx.run_mode);
    return 0;
}

static int test_interlock_forces_alarm_safe_output_and_alarm_log(void)
{
    setup();
    g_machineCtx.machine_state = MACHINE_RUNNING;
    g_ioIn.sensor_bits = SENSOR_PRESSURE_HIGH;
    RtControl_UpdateInterlock();
    TEST_ASSERT_EQ_UINT(MACHINE_ALARM, g_machineCtx.machine_state);
    TEST_ASSERT_EQ_UINT(9001U, g_machineCtx.alarm_code);
    TEST_ASSERT_TRUE((g_ioOut.command_bits & CMD_SAFE_OUTPUT) != 0U);
    TEST_ASSERT_TRUE(g_logQueue.tail != g_logQueue.head);
    return 0;
}

int main(void)
{
    RUN_TEST(test_prestart_interlock_ok_when_ready_switch_pressure_and_no_buffer_requirement);
    RUN_TEST(test_machine_state_transitions_stopped_ready_running_stopped);
    RUN_TEST(test_process_mode_request_rejects_auto_unless_ready_and_interlock_ok);
    RUN_TEST(test_interlock_forces_alarm_safe_output_and_alarm_log);
    printf("test_rt_control_state_more: PASS\n");
    return 0;
}
