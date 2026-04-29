/* ibm-bob/samples/base/source/src/system_shared.c */
/* Defines and initializes shared memory for the IBM BOB baseline sample. */
/* This exists so RT, TS, API, and NC modules start from known safe values. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/src/control_api.c, ibm-bob/samples/base/source/src/nc_program.c */
#include <string.h>
#include "system_shared.h"

volatile IO_INPUT_AREA      g_ioIn;
volatile IO_OUTPUT_AREA     g_ioOut;
volatile MACHINE_CONTEXT    g_machineCtx;
volatile UI_REQUEST_AREA    g_uiRequest;
volatile LOG_QUEUE          g_logQueue;
volatile PREFETCH_REQ_CTRL  g_prefetchReq;
volatile PREFETCH_RESULT    g_prefetchResult;
volatile NC_PROGRAM_REQUEST g_ncProgramRequest;
volatile NC_PROGRAM_STATUS  g_ncProgramStatus;
volatile NC_EXEC_BUFFER     g_ncProgramBuffer;
volatile NC_COORDINATE_STATE g_ncCoordinateState;
volatile NC_INTERP_STATUS   g_ncInterpStatus;
volatile NC_FEED_STATUS     g_ncFeedStatus;
volatile NC_FEATURE_STATUS  g_ncFeatureStatus;
volatile PRESTART_INTERLOCK_STATUS g_prestartInterlockStatus;
volatile NC_AUX_STATUS      g_ncAuxStatus;
volatile NC_CYCLE_STATUS    g_ncCycleStatus;
volatile NC_COMPENSATION_STATUS g_ncCompensationStatus;
volatile NC_PATH_CONTROL_STATUS g_ncPathControlStatus;
volatile NC_KINEMATICS_STATUS g_ncKinematicsStatus;
volatile NC_AXIS_LOAD_STATUS  g_ncAxisLoadStatus;
volatile NC_TOOL_LIFE_STATUS  g_ncToolLifeStatus;
volatile NC_DIAGNOSTIC_SNAPSHOT g_ncDiagnosticSnapshot;
volatile NC_MOTION_FILTER_STATUS g_ncMotionFilterStatus;
volatile NC_EVENT_RING    g_ncEventRing;
volatile NC_BINARY_PROGRAM_REQUEST g_ncBinaryProgramRequest;
volatile NC_BINARY_PROGRAM_STATUS  g_ncBinaryProgramStatus;
volatile NC_CAPABILITY_STATUS      g_ncCapabilityStatus;
volatile NC_INTERFERENCE_STATUS    g_ncInterferenceStatus;
volatile NC_SAFETY_MOTION_STATUS   g_ncSafetyMotionStatus;
volatile IO_TRACE_BUFFER    g_ioTraceBuffer;

void SystemShared_Initialize(void)
{
    (void)memset((void*)&g_ioIn, 0, sizeof(g_ioIn));
    (void)memset((void*)&g_ioOut, 0, sizeof(g_ioOut));
    (void)memset((void*)&g_machineCtx, 0, sizeof(g_machineCtx));
    (void)memset((void*)&g_uiRequest, 0, sizeof(g_uiRequest));
    (void)memset((void*)&g_logQueue, 0, sizeof(g_logQueue));
    (void)memset((void*)&g_prefetchReq, 0, sizeof(g_prefetchReq));
    (void)memset((void*)&g_prefetchResult, 0, sizeof(g_prefetchResult));
    (void)memset((void*)&g_ncProgramRequest, 0, sizeof(g_ncProgramRequest));
    (void)memset((void*)&g_ncProgramStatus, 0, sizeof(g_ncProgramStatus));
    (void)memset((void*)&g_ncProgramBuffer, 0, sizeof(g_ncProgramBuffer));
    (void)memset((void*)&g_ncCoordinateState, 0, sizeof(g_ncCoordinateState));
    (void)memset((void*)&g_ncInterpStatus, 0, sizeof(g_ncInterpStatus));
    (void)memset((void*)&g_ncFeedStatus, 0, sizeof(g_ncFeedStatus));
    (void)memset((void*)&g_ncFeatureStatus, 0, sizeof(g_ncFeatureStatus));
    (void)memset((void*)&g_prestartInterlockStatus, 0, sizeof(g_prestartInterlockStatus));
    (void)memset((void*)&g_ncAuxStatus, 0, sizeof(g_ncAuxStatus));
    (void)memset((void*)&g_ncCycleStatus, 0, sizeof(g_ncCycleStatus));
    (void)memset((void*)&g_ncCompensationStatus, 0, sizeof(g_ncCompensationStatus));
    (void)memset((void*)&g_ncPathControlStatus, 0, sizeof(g_ncPathControlStatus));
    (void)memset((void*)&g_ncKinematicsStatus, 0, sizeof(g_ncKinematicsStatus));
    (void)memset((void*)&g_ncAxisLoadStatus, 0, sizeof(g_ncAxisLoadStatus));
    (void)memset((void*)&g_ncToolLifeStatus, 0, sizeof(g_ncToolLifeStatus));
    (void)memset((void*)&g_ncDiagnosticSnapshot, 0, sizeof(g_ncDiagnosticSnapshot));
    (void)memset((void*)&g_ncMotionFilterStatus, 0, sizeof(g_ncMotionFilterStatus));
    (void)memset((void*)&g_ncEventRing, 0, sizeof(g_ncEventRing));
    (void)memset((void*)&g_ncBinaryProgramRequest, 0, sizeof(g_ncBinaryProgramRequest));
    (void)memset((void*)&g_ncBinaryProgramStatus, 0, sizeof(g_ncBinaryProgramStatus));
    (void)memset((void*)&g_ncCapabilityStatus, 0, sizeof(g_ncCapabilityStatus));
    (void)memset((void*)&g_ncInterferenceStatus, 0, sizeof(g_ncInterferenceStatus));
    (void)memset((void*)&g_ncSafetyMotionStatus, 0, sizeof(g_ncSafetyMotionStatus));
    (void)memset((void*)&g_ioTraceBuffer, 0, sizeof(g_ioTraceBuffer));

    g_machineCtx.run_mode = RUN_MODE_MANUAL;
    g_machineCtx.machine_state = MACHINE_STOPPED;
    g_uiRequest.mode_response = RESPONSE_NONE;
    g_ncProgramStatus.state = NC_IDLE;
    g_ncProgramStatus.error_code = NC_ERR_NONE;
    g_ncProgramStatus.response_code = RESPONSE_NONE;
    g_ncInterpStatus.state = NC_INTERP_IDLE;
    g_ncInterpStatus.last_error = NC_ERR_NONE;
    g_ncFeedStatus.state = NC_FEED_CTRL_IDLE;
    g_ncFeedStatus.feed_override_percent = NC_FEED_OVERRIDE_DEFAULT_PERCENT;
    g_prestartInterlockStatus.mask = PRESTART_ILK_READY_SWITCH | PRESTART_ILK_PRESSURE_RANGE;
    g_prestartInterlockStatus.ok = 0U;
    g_ncAuxStatus.state = NC_AUX_STATE_IDLE;
    g_ncAxisLoadStatus.decel_override_percent = NC_FEED_OVERRIDE_DEFAULT_PERCENT;
    g_ncMotionFilterStatus.enabled = 1U;
    g_ncMotionFilterStatus.second_stage_mode = NC_MOTION_FILTER_SECOND_STAGE_FIR;
    g_ncMotionFilterStatus.velocity_window = NC_MOTION_FILTER_DEFAULT_VELOCITY_WINDOW;
    g_ncMotionFilterStatus.accel_window = NC_MOTION_FILTER_DEFAULT_ACCEL_WINDOW;
    g_ncMotionFilterStatus.endpoint_correction_window = NC_MOTION_FILTER_ENDPOINT_WINDOW;
    g_ncInterferenceStatus.enabled = 1U;
    g_ncSafetyMotionStatus.active_override_percent = NC_FEED_OVERRIDE_DEFAULT_PERCENT;
    for (uint32_t axis = 0U; axis < AXIS_MAX; ++axis) {
        g_ncMotionFilterStatus.max_velocity_per_tick[axis] = NC_MOTION_FILTER_DEFAULT_MAX_VELOCITY;
        g_ncMotionFilterStatus.max_accel_per_tick[axis] = NC_MOTION_FILTER_DEFAULT_MAX_ACCEL;
    }
    for (uint32_t i = 0U; i < NC_TOOL_LIFE_TABLE_SIZE; ++i) {
        g_ncToolLifeStatus.tool_life_limit[i] = NC_TOOL_LIFE_DEFAULT_LIMIT;
    }
}

int32_t LogQueue_Push(uint32_t eventType, uint32_t code, int32_t value)
{
    uint16_t nextTail = (uint16_t)((g_logQueue.tail + 1U) % LOG_QUEUE_MAX);
    LOG_ITEM* pItem;

    if (nextTail == g_logQueue.head) {
        return -1;
    }

    pItem = (LOG_ITEM*)&g_logQueue.items[g_logQueue.tail];
    pItem->event_type = eventType;
    pItem->code = code;
    pItem->value = value;
    pItem->cycle = g_machineCtx.cycle_count;
    g_logQueue.tail = nextTail;

    return 0;
}

void NcEvent_Push(uint32_t code, int32_t value0, int32_t value1, uint32_t lineNo)
{
    NC_EVENT_ITEM* pItem;
    uint32_t index = g_ncEventRing.write_index;

    if (index >= NC_EVENT_RING_SIZE) {
        index = 0U;
        g_ncEventRing.write_index = 0U;
    }
    pItem = (NC_EVENT_ITEM*)&g_ncEventRing.items[index];
    pItem->cycle = g_machineCtx.cycle_count;
    pItem->code = code;
    pItem->value0 = value0;
    pItem->value1 = value1;
    pItem->line_no = lineNo;
    pItem->nc_state = g_ncProgramStatus.state;
    g_ncEventRing.write_index = (index + 1U) % NC_EVENT_RING_SIZE;
    if (g_ncEventRing.captured_count < NC_EVENT_RING_SIZE) {
        g_ncEventRing.captured_count++;
    } else {
        g_ncEventRing.lost_count++;
    }
    g_ncEventRing.generation++;
}
