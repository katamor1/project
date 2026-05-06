/* ibm-bob/samples/base/source/inc/nc_feature_backlog_service_interlocks.h */
/* Declares the public contract for nc feature backlog service interlocks backlog checks. */
/* This exists so tests and sample builds can share the same fixed-size RT-safe types. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_feature_backlog_service_interlocks.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_service_interlocks.c, ibm-bob/samples/base/source/scripts/additive_tests.tsv */
#ifndef NC_FEATURE_BACKLOG_SERVICE_INTERLOCKS_H
#define NC_FEATURE_BACKLOG_SERVICE_INTERLOCKS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * IBM-Bob implementation backlog continuation v41_10.
 *
 * This module adds feature IDs 325-334 after v40_10.
 * RT constraints: fixed-size state only, no dynamic allocation,
 * no file I/O, no Windows API calls, and bounded loops.
 */

#define NC_IMPL_BACKLOG_FEATURE_COUNT_V40      (325u)
#define NC_IMPL_BACKLOG_FEATURE_COUNT_V41      (335u)
#define NC_IMPL_BACKLOG_WORD_COUNT_V41         (11u)
#define NC_FEATURE_BACKLOG_V41_FEATURE_BASE_ID (325u)
#define NC_FEATURE_BACKLOG_V41_FEATURE_COUNT   (10u)
#define NC_FEATURE_BACKLOG_V41_AXIS_COUNT      (8u)

#define NC_FEATURE_BACKLOG_V41_OK              (0)
#define NC_FEATURE_BACKLOG_V41_ERR_NULL        (-1)
#define NC_FEATURE_BACKLOG_V41_ERR_RANGE       (-2)

#define NC_FEATURE_BACKLOG_V41_TRACE_NONE      (0u)
#define NC_FEATURE_BACKLOG_V41_TRACE_BASE      (0x4100u)

/* Added feature IDs: 325-334. */
typedef enum NC_FEATURE_BACKLOG_V41_ID_TAG {
    NC_FEATURE_V41_SERVO_POSITION_ERROR_SLOWDOWN = 325,
    NC_FEATURE_V41_SPINDLE_LOAD_SPIKE_HOLD = 326,
    NC_FEATURE_V41_COOLANT_FLOW_SENSOR_OPEN_TRACE = 327,
    NC_FEATURE_V41_FIXTURE_CLAMP_PRESSURE_INTERLOCK = 328,
    NC_FEATURE_V41_ROBOT_DOOR_CLOSED_READY_GATE = 329,
    NC_FEATURE_V41_CUTTING_ROOM_EXHAUST_FAN_READY_GATE = 330,
    NC_FEATURE_V41_TOOL_WASH_NOZZLE_CLOG_RETRY = 331,
    NC_FEATURE_V41_AXIS_HOME_SENSOR_DUAL_MISMATCH_TRACE = 332,
    NC_FEATURE_V41_NC_OPTION_LICENSE_MISMATCH_INHIBIT = 333,
    NC_FEATURE_V41_MAINTENANCE_MODE_KEY_REMOVED_HOLD = 334
} NC_FEATURE_BACKLOG_V41_ID;

typedef enum NC_FEATURE_BACKLOG_V41_POLICY_TAG {
    NC_FEATURE_BACKLOG_V41_POLICY_MONITOR = 0,
    NC_FEATURE_BACKLOG_V41_POLICY_GATE = 1,
    NC_FEATURE_BACKLOG_V41_POLICY_HOLD = 2,
    NC_FEATURE_BACKLOG_V41_POLICY_INTERLOCK = 3,
    NC_FEATURE_BACKLOG_V41_POLICY_SLOWDOWN = 4,
    NC_FEATURE_BACKLOG_V41_POLICY_TRACE = 5,
    NC_FEATURE_BACKLOG_V41_POLICY_INHIBIT = 6,
    NC_FEATURE_BACKLOG_V41_POLICY_RETRY = 7
} NC_FEATURE_BACKLOG_V41_POLICY;

typedef struct NC_FEATURE_BACKLOG_V41_BLOCK_TAG {
    int32_t axis_target[NC_FEATURE_BACKLOG_V41_AXIS_COUNT];
    int32_t feed_override_percent;
    uint32_t aux_request_bits;
    uint32_t hold_request;
    uint32_t interlock_mask;
    uint32_t sequence_gate_mask;
    uint32_t diagnostics_mask;
    uint32_t retry_request_mask;
    uint32_t inhibit_mask;
    uint32_t trace_event_code;
} NC_FEATURE_BACKLOG_V41_BLOCK;

typedef struct NC_FEATURE_BACKLOG_V41_RT_IO_TAG {
    uint32_t cycle_counter;
    uint32_t servo_position_error_monitor;
    uint32_t servo_position_error_axis_mask;
    int32_t servo_position_error_um;
    uint32_t spindle_load_monitor;
    int32_t spindle_load_percent;
    uint32_t coolant_flow_sensor_monitor;
    uint32_t coolant_flow_sensor_open;
    uint32_t fixture_clamp_pressure_required;
    int32_t fixture_clamp_pressure_kpa;
    uint32_t robot_door_closed_required;
    uint32_t robot_door_closed;
    uint32_t exhaust_fan_ready_required;
    uint32_t exhaust_fan_ready;
    uint32_t tool_wash_nozzle_monitor;
    int32_t tool_wash_flow_percent;
    uint32_t axis_home_sensor_check_request;
    uint32_t axis_home_sensor_a_mask;
    uint32_t axis_home_sensor_b_mask;
    uint32_t nc_option_license_check_request;
    uint32_t nc_option_required_mask;
    uint32_t nc_option_licensed_mask;
    uint32_t maintenance_mode_monitor;
    uint32_t maintenance_mode_active;
    uint32_t maintenance_key_present;
} NC_FEATURE_BACKLOG_V41_RT_IO;

typedef struct NC_FEATURE_BACKLOG_V41_STATUS_TAG {
    uint32_t implemented_count;
    uint32_t configured_count;
    uint32_t enabled_mask;
    uint32_t ts_apply_count;
    uint32_t rt_tick_count;
    uint32_t monitor_count;
    uint32_t gate_count;
    uint32_t hold_count;
    uint32_t interlock_count;
    uint32_t slowdown_count;
    uint32_t trace_count;
    uint32_t inhibit_count;
    uint32_t retry_count;
    uint32_t warning_count;
    uint32_t cat_mask;
    uint32_t last_feature_id;
    uint32_t last_policy;
    uint32_t last_trace_event_code;
    int32_t last_feed_override_percent;
    int32_t last_x;
    uint32_t last_fault_mask;
    uint32_t last_axis_mask;
    int32_t last_servo_error_um;
    int32_t last_spindle_load_percent;
    int32_t last_fixture_pressure_kpa;
    int32_t last_tool_wash_flow_percent;
    uint32_t last_license_missing_mask;
    uint32_t per_feature_hits[NC_FEATURE_BACKLOG_V41_FEATURE_COUNT];
    int32_t per_feature_last_value[NC_FEATURE_BACKLOG_V41_FEATURE_COUNT];
} NC_FEATURE_BACKLOG_V41_STATUS;

extern NC_FEATURE_BACKLOG_V41_STATUS g_ncFeatureBacklogV41Status;

void NcFeatureBacklogV41_Init(void);
int NcFeatureBacklogV41_IsFeatureEnabled(uint32_t feature_id);
int Api_SetNcFeatureBacklogV41Feature(uint32_t feature_id, uint32_t enabled);
void Api_EnableAllNcFeatureBacklogV41Features(void);
int Api_GetNcFeatureBacklogV41Status(NC_FEATURE_BACKLOG_V41_STATUS *out_status);
int Api_RunNcFeatureBacklogV41SelfCheck(void);

void NcFeatureBacklogV41_ApplyBlockTs(NC_FEATURE_BACKLOG_V41_BLOCK *block);
void NcFeatureBacklogV41_RunRtTick(const NC_FEATURE_BACKLOG_V41_RT_IO *io);
const char *NcFeatureBacklogV41_GetFeatureName(uint32_t feature_id);

#ifdef __cplusplus
}
#endif

#endif /* NC_FEATURE_BACKLOG_SERVICE_INTERLOCKS_H */
