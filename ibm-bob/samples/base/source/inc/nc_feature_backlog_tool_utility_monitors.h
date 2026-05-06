#ifndef NC_FEATURE_BACKLOG_V39_H
#define NC_FEATURE_BACKLOG_V39_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * IBM-Bob implementation backlog continuation v39_10.
 *
 * This module adds feature IDs 305-314 after v38_10.
 * The implementation follows the IBM-Bob RT/TS split rules:
 * - fixed-size state only
 * - no dynamic allocation
 * - no file I/O
 * - no Windows API calls
 * - deterministic loops bounded by NC_FEATURE_BACKLOG_V39_FEATURE_COUNT
 */

#define NC_IMPL_BACKLOG_FEATURE_COUNT_V38      (305u)
#define NC_IMPL_BACKLOG_FEATURE_COUNT_V39      (315u)
#define NC_IMPL_BACKLOG_WORD_COUNT_V39         (10u)
#define NC_FEATURE_BACKLOG_V39_FEATURE_BASE_ID (305u)
#define NC_FEATURE_BACKLOG_V39_FEATURE_COUNT   (10u)
#define NC_FEATURE_BACKLOG_V39_AXIS_COUNT      (8u)

#define NC_FEATURE_BACKLOG_V39_OK              (0)
#define NC_FEATURE_BACKLOG_V39_ERR_NULL        (-1)
#define NC_FEATURE_BACKLOG_V39_ERR_RANGE       (-2)

#define NC_FEATURE_BACKLOG_V39_TRACE_NONE      (0u)
#define NC_FEATURE_BACKLOG_V39_TRACE_BASE      (0x3900u)

/* Added feature IDs: 305-314. */
typedef enum NC_FEATURE_BACKLOG_V39_ID_TAG {
    NC_FEATURE_V39_SPINDLE_BEARING_TEMP_SLOWDOWN = 305,
    NC_FEATURE_V39_LINEAR_SCALE_DIRTY_TRACE = 306,
    NC_FEATURE_V39_TOOL_CLAMP_DRAWBAR_CONFIRM_INTERLOCK = 307,
    NC_FEATURE_V39_ROTARY_TABLE_CLAMP_CONFIRM_GATE = 308,
    NC_FEATURE_V39_COOLANT_FILTER_DIFF_PRESSURE_HOLD = 309,
    NC_FEATURE_V39_DUST_COLLECTOR_AIRFLOW_LOW_GATE = 310,
    NC_FEATURE_V39_SPINDLE_OVERRIDE_LOCK_ZONE_INHIBIT = 311,
    NC_FEATURE_V39_ATC_TOOL_NUMBER_MISMATCH_HOLD = 312,
    NC_FEATURE_V39_WORK_PROBE_READY_TIMEOUT_RETRY = 313,
    NC_FEATURE_V39_UPS_VOLTAGE_LOW_DECEL_STOP = 314
} NC_FEATURE_BACKLOG_V39_ID;

typedef enum NC_FEATURE_BACKLOG_V39_POLICY_TAG {
    NC_FEATURE_BACKLOG_V39_POLICY_MONITOR = 0,
    NC_FEATURE_BACKLOG_V39_POLICY_GATE = 1,
    NC_FEATURE_BACKLOG_V39_POLICY_HOLD = 2,
    NC_FEATURE_BACKLOG_V39_POLICY_INTERLOCK = 3,
    NC_FEATURE_BACKLOG_V39_POLICY_SLOWDOWN = 4,
    NC_FEATURE_BACKLOG_V39_POLICY_TRACE = 5,
    NC_FEATURE_BACKLOG_V39_POLICY_INHIBIT = 6,
    NC_FEATURE_BACKLOG_V39_POLICY_RETRY = 7
} NC_FEATURE_BACKLOG_V39_POLICY;

typedef struct NC_FEATURE_BACKLOG_V39_BLOCK_TAG {
    int32_t axis_target[NC_FEATURE_BACKLOG_V39_AXIS_COUNT];
    int32_t feed_override_percent;
    uint32_t aux_request_bits;
    uint32_t hold_request;
    uint32_t interlock_mask;
    uint32_t sequence_gate_mask;
    uint32_t diagnostics_mask;
    uint32_t retry_request_mask;
    uint32_t inhibit_mask;
    uint32_t trace_event_code;
} NC_FEATURE_BACKLOG_V39_BLOCK;

typedef struct NC_FEATURE_BACKLOG_V39_RT_IO_TAG {
    uint32_t cycle_counter;
    uint32_t spindle_bearing_temp_check;
    int32_t spindle_bearing_temp_c;
    uint32_t linear_scale_monitor_mask;
    uint32_t linear_scale_dirty_mask;
    uint32_t tool_clamp_check_request;
    uint32_t tool_clamp_drawbar_ok;
    uint32_t rotary_table_motion_request;
    uint32_t rotary_table_clamp_confirmed;
    uint32_t coolant_filter_monitor;
    uint32_t coolant_filter_diff_pressure_high;
    uint32_t dust_collector_required;
    uint32_t dust_collector_airflow_ok;
    uint32_t spindle_override_lock_zone;
    uint32_t spindle_override_change_request;
    uint32_t atc_tool_number_check_request;
    uint32_t atc_expected_tool_no;
    uint32_t atc_actual_tool_no;
    uint32_t work_probe_measure_request;
    uint32_t work_probe_ready;
    uint32_t ups_monitor_enabled;
    uint32_t ups_voltage_low;
} NC_FEATURE_BACKLOG_V39_RT_IO;

typedef struct NC_FEATURE_BACKLOG_V39_STATUS_TAG {
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
    uint32_t last_tool_expected;
    uint32_t last_tool_actual;
    int32_t last_spindle_bearing_temp_c;
    uint32_t per_feature_hits[NC_FEATURE_BACKLOG_V39_FEATURE_COUNT];
    int32_t per_feature_last_value[NC_FEATURE_BACKLOG_V39_FEATURE_COUNT];
} NC_FEATURE_BACKLOG_V39_STATUS;

extern NC_FEATURE_BACKLOG_V39_STATUS g_ncFeatureBacklogV39Status;

void NcFeatureBacklogV39_Init(void);
int NcFeatureBacklogV39_IsFeatureEnabled(uint32_t feature_id);
int Api_SetNcFeatureBacklogV39Feature(uint32_t feature_id, uint32_t enabled);
void Api_EnableAllNcFeatureBacklogV39Features(void);
int Api_GetNcFeatureBacklogV39Status(NC_FEATURE_BACKLOG_V39_STATUS *out_status);
int Api_RunNcFeatureBacklogV39SelfCheck(void);

void NcFeatureBacklogV39_ApplyBlockTs(NC_FEATURE_BACKLOG_V39_BLOCK *block);
void NcFeatureBacklogV39_RunRtTick(const NC_FEATURE_BACKLOG_V39_RT_IO *io);
const char *NcFeatureBacklogV39_GetFeatureName(uint32_t feature_id);

#ifdef __cplusplus
}
#endif

#endif /* NC_FEATURE_BACKLOG_V39_H */
