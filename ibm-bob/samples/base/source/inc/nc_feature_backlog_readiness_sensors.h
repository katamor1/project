/* ibm-bob/samples/base/source/inc/nc_feature_backlog_readiness_sensors.h */
/* Declares the public contract for nc feature backlog readiness sensors backlog checks. */
/* This exists so tests and sample builds can share the same fixed-size RT-safe types. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_feature_backlog_readiness_sensors.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_readiness_sensors.c, ibm-bob/samples/base/source/scripts/additive_tests.tsv */
#ifndef NC_FEATURE_BACKLOG_READINESS_SENSORS_H
#define NC_FEATURE_BACKLOG_READINESS_SENSORS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * IBM-Bob implementation backlog continuation v37_10.
 *
 * This module adds feature IDs 285-294 after v36_5.
 * The implementation follows the IBM-Bob RT/TS split rules:
 * - fixed-size state only
 * - no dynamic allocation
 * - no file I/O
 * - no Windows API calls
 * - deterministic loops bounded by NC_FEATURE_BACKLOG_V37_FEATURE_COUNT
 */

#define NC_IMPL_BACKLOG_FEATURE_COUNT_V36      (285u)
#define NC_IMPL_BACKLOG_FEATURE_COUNT_V37      (295u)
#define NC_IMPL_BACKLOG_WORD_COUNT_V37         (10u)
#define NC_FEATURE_BACKLOG_V37_FEATURE_BASE_ID (285u)
#define NC_FEATURE_BACKLOG_V37_FEATURE_COUNT   (10u)
#define NC_FEATURE_BACKLOG_V37_AXIS_COUNT      (8u)

#define NC_FEATURE_BACKLOG_V37_OK              (0)
#define NC_FEATURE_BACKLOG_V37_ERR_NULL        (-1)
#define NC_FEATURE_BACKLOG_V37_ERR_RANGE       (-2)

#define NC_FEATURE_BACKLOG_V37_TRACE_NONE      (0u)
#define NC_FEATURE_BACKLOG_V37_TRACE_BASE      (0x3700u)

/* Added feature IDs: 285-294. */
typedef enum NC_FEATURE_BACKLOG_V37_ID_TAG {
    NC_FEATURE_V37_SPINDLE_CHILLER_FLOW_LOW_SLOWDOWN = 285,
    NC_FEATURE_V37_AXIS_SERVO_FAN_STOP_HOLD = 286,
    NC_FEATURE_V37_DOOR_LOCK_DUAL_CHANNEL_MISMATCH_TRACE = 287,
    NC_FEATURE_V37_TOOL_MAGAZINE_HOME_TIMEOUT_INTERLOCK = 288,
    NC_FEATURE_V37_HYDRAULIC_PRESSURE_RISE_READY_GATE = 289,
    NC_FEATURE_V37_WORK_AIR_SEAT_CHECK_RETRY_GATE = 290,
    NC_FEATURE_V37_CHIP_FLUSH_PRESSURE_LOW_HOLD = 291,
    NC_FEATURE_V37_OPERATOR_PANEL_ENABLE_SWITCH_GUARD = 292,
    NC_FEATURE_V37_PROBE_BATTERY_LOW_MEASURE_INHIBIT = 293,
    NC_FEATURE_V37_ABSOLUTE_ENCODER_BATTERY_WARN_TRACE = 294
} NC_FEATURE_BACKLOG_V37_ID;

typedef enum NC_FEATURE_BACKLOG_V37_POLICY_TAG {
    NC_FEATURE_BACKLOG_V37_POLICY_MONITOR = 0,
    NC_FEATURE_BACKLOG_V37_POLICY_GATE = 1,
    NC_FEATURE_BACKLOG_V37_POLICY_HOLD = 2,
    NC_FEATURE_BACKLOG_V37_POLICY_INTERLOCK = 3,
    NC_FEATURE_BACKLOG_V37_POLICY_SLOWDOWN = 4,
    NC_FEATURE_BACKLOG_V37_POLICY_TRACE = 5,
    NC_FEATURE_BACKLOG_V37_POLICY_INHIBIT = 6,
    NC_FEATURE_BACKLOG_V37_POLICY_RETRY = 7
} NC_FEATURE_BACKLOG_V37_POLICY;

typedef struct NC_FEATURE_BACKLOG_V37_BLOCK_TAG {
    int32_t axis_target[NC_FEATURE_BACKLOG_V37_AXIS_COUNT];
    int32_t feed_override_percent;
    uint32_t aux_request_bits;
    uint32_t hold_request;
    uint32_t interlock_mask;
    uint32_t sequence_gate_mask;
    uint32_t diagnostics_mask;
    uint32_t retry_request_mask;
    uint32_t inhibit_mask;
    uint32_t trace_event_code;
} NC_FEATURE_BACKLOG_V37_BLOCK;

typedef struct NC_FEATURE_BACKLOG_V37_RT_IO_TAG {
    uint32_t cycle_counter;
    uint32_t spindle_chiller_on;
    uint32_t spindle_chiller_flow_ok;
    int32_t spindle_temp_c_x10;
    uint32_t servo_ready_mask;
    uint32_t servo_fan_ok_mask;
    uint32_t servo_monitored_mask;
    uint32_t door_lock_command;
    uint32_t door_lock_ch1;
    uint32_t door_lock_ch2;
    uint32_t tool_magazine_home_request;
    uint32_t tool_magazine_home_sensor;
    uint32_t hydraulic_ready_request;
    uint32_t hydraulic_pump_on;
    uint32_t hydraulic_pressure_ok;
    uint32_t work_air_seat_check_request;
    uint32_t work_air_blow_on;
    uint32_t work_air_seat_ok;
    uint32_t chip_flush_pump_on;
    uint32_t chip_flush_pressure_ok;
    uint32_t operator_panel_active;
    uint32_t operator_enable_required;
    uint32_t operator_enable_switch_on;
    uint32_t probe_measure_request;
    uint32_t probe_battery_low;
    uint32_t abs_encoder_battery_low_mask;
} NC_FEATURE_BACKLOG_V37_RT_IO;

typedef struct NC_FEATURE_BACKLOG_V37_STATUS_TAG {
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
    uint32_t per_feature_hits[NC_FEATURE_BACKLOG_V37_FEATURE_COUNT];
    int32_t per_feature_last_value[NC_FEATURE_BACKLOG_V37_FEATURE_COUNT];
} NC_FEATURE_BACKLOG_V37_STATUS;

extern NC_FEATURE_BACKLOG_V37_STATUS g_ncFeatureBacklogV37Status;

void NcFeatureBacklogV37_Init(void);
int NcFeatureBacklogV37_IsFeatureEnabled(uint32_t feature_id);
int Api_SetNcFeatureBacklogV37Feature(uint32_t feature_id, uint32_t enabled);
void Api_EnableAllNcFeatureBacklogV37Features(void);
int Api_GetNcFeatureBacklogV37Status(NC_FEATURE_BACKLOG_V37_STATUS *out_status);
int Api_RunNcFeatureBacklogV37SelfCheck(void);

void NcFeatureBacklogV37_ApplyBlockTs(NC_FEATURE_BACKLOG_V37_BLOCK *block);
void NcFeatureBacklogV37_RunRtTick(const NC_FEATURE_BACKLOG_V37_RT_IO *io);
const char *NcFeatureBacklogV37_GetFeatureName(uint32_t feature_id);

#ifdef __cplusplus
}
#endif

#endif /* NC_FEATURE_BACKLOG_READINESS_SENSORS_H */
