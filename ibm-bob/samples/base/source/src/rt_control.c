/* ibm-bob/samples/base/source/src/rt_control.c */
/* Runs the real-time dispatcher and fixed-cycle machine control logic. */
/* This exists so safety checks and fixed-size NC block consumption stay in RT. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/rt_control.h, ibm-bob/samples/base/source/inc/nc_program.h, ibm-bob/samples/base/source/src/system_shared.c */
#include "rt_control.h"
#include "nc_program.h"
#include "nc_diagnostics.h"
#include "nc_safety_motion.h"
#include "system_shared.h"
#include "system_config.h"

#define SENSOR_EMERGENCY_STOP   (0x00000001UL)
#define SENSOR_PRESSURE_HIGH    (0x00000002UL)
#define SENSOR_SERVO_ALARM      (0x00000004UL)
#define SENSOR_READY_SWITCH     (0x00000008UL)
#define SENSOR_RUN_SWITCH       (0x00000010UL)

#define CMD_ENABLE_SERVO        (0x00000001UL)
#define CMD_START_MOTION        (0x00000002UL)
#define CMD_SAFE_OUTPUT         (0x80000000UL)

static int32_t GetPressureFeedback(void)
{
    return g_ioIn.analog[0];
}

static uint8_t IsPressureReadyRange(int32_t pressure)
{
    return (uint8_t)((pressure >= READY_PRESSURE_MIN) &&
                     (pressure <= READY_PRESSURE_MAX));
}

void RtControl_UpdatePrestartInterlock(void)
{
    uint32_t mask = 0U;
    uint32_t bits = g_ioIn.sensor_bits;
    int32_t pressure = GetPressureFeedback();

    if ((bits & SENSOR_EMERGENCY_STOP) != 0U) {
        mask |= PRESTART_ILK_ESTOP;
    }
    if ((bits & SENSOR_SERVO_ALARM) != 0U) {
        mask |= PRESTART_ILK_SERVO_ALARM;
    }
    if (IsPressureReadyRange(pressure) == 0U) {
        mask |= PRESTART_ILK_PRESSURE_RANGE;
    }
    if ((bits & SENSOR_READY_SWITCH) == 0U) {
        mask |= PRESTART_ILK_READY_SWITCH;
    }
    if (g_machineCtx.machine_state == MACHINE_ALARM) {
        mask |= PRESTART_ILK_MACHINE_ALARM;
    }
    if ((g_ncProgramStatus.state == NC_READY) &&
        (g_ncProgramStatus.buffered_blocks == 0U)) {
        mask |= PRESTART_ILK_NC_BUFFER;
    }

    g_prestartInterlockStatus.mask = mask;
    g_prestartInterlockStatus.ok = (uint8_t)(mask == 0U);
    g_prestartInterlockStatus.sensor_bits = bits;
    g_prestartInterlockStatus.pressure = pressure;
    g_prestartInterlockStatus.buffered_blocks = g_ncProgramStatus.buffered_blocks;
    g_prestartInterlockStatus.generation++;
}

void RtControl_CaptureIoTrace(void)
{
    IO_TRACE_ITEM* pItem;
    uint32_t i;

    if (g_ioTraceBuffer.frozen != 0U) {
        return;
    }

    pItem = (IO_TRACE_ITEM*)&g_ioTraceBuffer.items[g_ioTraceBuffer.write_index];
    pItem->cycle = g_machineCtx.cycle_count;
    pItem->sensor_bits = g_ioIn.sensor_bits;
    pItem->seq_bits = g_ioIn.seq_bits;
    pItem->command_bits = g_ioOut.command_bits;
    pItem->aux_m_code = g_ioOut.aux_m_code;
    pItem->spindle_speed = g_ioOut.spindle_speed;
    pItem->tool_no = g_ioOut.tool_no;
    pItem->run_mode = g_machineCtx.run_mode;
    pItem->machine_state = g_machineCtx.machine_state;
    pItem->nc_state = g_ncProgramStatus.state;
    pItem->alarm_code = g_machineCtx.alarm_code;
    for (i = 0U; i < AXIS_MAX; ++i) {
        pItem->axis_target[i] = g_ioOut.axis_target[i];
    }

    g_ioTraceBuffer.write_index =
        (g_ioTraceBuffer.write_index + 1U) % IO_TRACE_RING_SIZE;
    if (g_ioTraceBuffer.captured_count < IO_TRACE_RING_SIZE) {
        g_ioTraceBuffer.captured_count++;
    }
    if (g_machineCtx.machine_state == MACHINE_ALARM) {
        g_ioTraceBuffer.frozen = 1U;
        g_ioTraceBuffer.trigger_cycle = g_machineCtx.cycle_count;
        g_ioTraceBuffer.trigger_alarm_code = g_machineCtx.alarm_code;
    }
    g_ioTraceBuffer.generation++;
}

void RtDispatcher_ExecuteCycle(void)
{
    g_machineCtx.cycle_count++;

    RtInput_ScanInputs();
    RtControl_UpdatePrestartInterlock();
    NcDiagnostics_UpdateAxisLoadRt();
    NcSafetyMotion_UpdateRt();
    RtControl_UpdateMachineState();
    RtControl_ProcessModeRequest();
    RtControl_UpdateInterlock();
    RtNcProgram_ProcessRequests((uint8_t)((g_machineCtx.machine_state != MACHINE_ALARM) &&
                                          (g_prestartInterlockStatus.ok != 0U)));
    RtControl_UpdateAxisTargets();
    RtNcProgram_ConsumeBlocks();
    RtOutput_CommitOutputs();
    RtControl_CaptureIoTrace();
    NcDiagnostics_BuildSnapshot();

    g_machineCtx.rt_input_ticks = 1U;
    g_machineCtx.rt_control_ticks = 2U;
    g_machineCtx.rt_output_ticks = 1U;
    g_machineCtx.rt_total_ticks = (g_machineCtx.rt_input_ticks +
                                   g_machineCtx.rt_control_ticks +
                                   g_machineCtx.rt_output_ticks);
}

void RtInput_ScanInputs(void)
{
    /* 評価用コードでは外部入力ミラー更新を省略し、既存 g_ioIn を参照する。 */
    if (g_prefetchReq.ready == 0U) {
        g_prefetchReq.request = 1U;
        g_prefetchReq.section_id = (uint16_t)(g_machineCtx.cycle_count % 4U);
    }
}

void RtControl_UpdateMachineState(void)
{
    int32_t pressure = GetPressureFeedback();

    switch (g_machineCtx.machine_state) {
    case MACHINE_STOPPED:
        if (((g_ioIn.sensor_bits & SENSOR_READY_SWITCH) != 0U) &&
            (IsPressureReadyRange(pressure) != 0U)) {
            g_machineCtx.machine_state = MACHINE_READY;
            (void)LogQueue_Push(LOG_INFO, 1001U, pressure);
        }
        break;

    case MACHINE_READY:
        if (((g_ioIn.sensor_bits & SENSOR_RUN_SWITCH) != 0U) &&
            (g_machineCtx.run_mode == RUN_MODE_AUTO)) {
            g_machineCtx.machine_state = MACHINE_RUNNING;
            (void)LogQueue_Push(LOG_AUDIT, 1002U, 0);
        }
        break;

    case MACHINE_RUNNING:
        if ((g_ioIn.sensor_bits & SENSOR_RUN_SWITCH) == 0U) {
            g_machineCtx.machine_state = MACHINE_STOPPED;
            (void)LogQueue_Push(LOG_AUDIT, 1003U, 0);
        }
        break;

    case MACHINE_ALARM:
    default:
        break;
    }
}

void RtControl_ProcessModeRequest(void)
{
    if (g_uiRequest.mode_change_request == 0U) {
        return;
    }

    switch ((RUN_MODE)g_uiRequest.requested_mode) {
    case RUN_MODE_MANUAL:
        g_machineCtx.run_mode = RUN_MODE_MANUAL;
        g_uiRequest.mode_response = RESPONSE_COMPLETED;
        break;

    case RUN_MODE_AUTO:
        if ((g_machineCtx.machine_state == MACHINE_READY) &&
            (g_prestartInterlockStatus.ok != 0U)) {
            g_machineCtx.run_mode = RUN_MODE_AUTO;
            g_uiRequest.mode_response = RESPONSE_COMPLETED;
        } else {
            g_uiRequest.mode_response = RESPONSE_REJECTED;
        }
        break;

    case RUN_MODE_MAINT:
        if (g_machineCtx.machine_state != MACHINE_RUNNING) {
            g_machineCtx.run_mode = RUN_MODE_MAINT;
            g_uiRequest.mode_response = RESPONSE_COMPLETED;
        } else {
            g_uiRequest.mode_response = RESPONSE_REJECTED;
        }
        break;

    default:
        g_uiRequest.mode_response = RESPONSE_PARAM_ERROR;
        break;
    }

    g_uiRequest.mode_change_request = 0U;
}

void RtControl_UpdateInterlock(void)
{
    int32_t pressure = GetPressureFeedback();
    uint32_t bits = g_ioIn.sensor_bits;

    if (((bits & SENSOR_EMERGENCY_STOP) != 0U) ||
        ((bits & SENSOR_SERVO_ALARM) != 0U) ||
        ((bits & SENSOR_PRESSURE_HIGH) != 0U) ||
        (pressure > SAFE_PRESSURE_LIMIT)) {
        g_machineCtx.machine_state = MACHINE_ALARM;
        g_machineCtx.alarm_code = 9001U;
        g_ioOut.command_bits = CMD_SAFE_OUTPUT;
        g_ioOut.lamp_bits = 0x00000004UL;
        (void)LogQueue_Push(LOG_ALARM, g_machineCtx.alarm_code, pressure);
    }
}

void RtControl_UpdateAxisTargets(void)
{
    uint32_t i;
    int32_t baseTarget = 0;

    if (g_machineCtx.machine_state == MACHINE_RUNNING) {
        g_ioOut.command_bits = (CMD_ENABLE_SERVO | CMD_START_MOTION);
        baseTarget = 1000;
    } else if (g_machineCtx.machine_state == MACHINE_READY) {
        g_ioOut.command_bits = CMD_ENABLE_SERVO;
        baseTarget = 100;
    } else {
        g_ioOut.command_bits = 0U;
        baseTarget = 0;
    }

    if ((g_prefetchReq.ready != 0U) &&
        (g_prefetchResult.generation == g_prefetchReq.generation)) {
        baseTarget += (int32_t)(g_prefetchResult.table[0] * 10.0F);
    }

    for (i = 0U; i < AXIS_MAX; ++i) {
        g_ioOut.axis_target[i] = baseTarget + ((int32_t)i * 25);
    }

    if (g_machineCtx.machine_state == MACHINE_ALARM) {
        for (i = 0U; i < AXIS_MAX; ++i) {
            g_ioOut.axis_target[i] = 0;
        }
    }
}

void RtOutput_CommitOutputs(void)
{
    if (g_machineCtx.machine_state == MACHINE_ALARM) {
        g_ioOut.command_bits |= CMD_SAFE_OUTPUT;
        g_ioOut.lamp_bits = 0x00000004UL;
    } else if (g_machineCtx.machine_state == MACHINE_RUNNING) {
        g_ioOut.lamp_bits = 0x00000002UL;
    } else {
        g_ioOut.lamp_bits = 0x00000001UL;
    }
}
