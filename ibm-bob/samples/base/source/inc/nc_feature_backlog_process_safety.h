#ifndef NC_FEATURE_BACKLOG_V40_H
#define NC_FEATURE_BACKLOG_V40_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * IBM-Bob implementation backlog continuation v40_10.
 *
 * This module adds feature IDs 315-324 after v39_10.
 * The implementation follows the IBM-Bob RT/TS split rules:
 * - fixed-size state only
 * - no dynamic allocation
 * - no file I/O
 * - no Windows API calls
 * - deterministic loops bounded by NC_FEATURE_BACKLOG_V40_FEATURE_COUNT
 */

#define NC_IMPL_BACKLOG_FEATURE_COUNT_V39      (315u)
#define NC_IMPL_BACKLOG_FEATURE_COUNT_V40      (325u)
#define NC_IMPL_BACKLOG_WORD_COUNT_V40         (11u)
#define NC_FEATURE_BACKLOG_V40_FEATURE_BASE_ID (315u)
#define NC_FEATURE_BACKLOG_V40_FEATURE_COUNT   (10u)
#define NC_FEATURE_BACKLOG_V40_AXIS_COUNT      (8u)

#define NC_FEATURE_BACKLOG_V40_OK              (0)
#define NC_FEATURE_BACKLOG_V40_ERR_NULL        (-1)
#define NC_FEATURE_BACKLOG_V40_ERR_RANGE       (-2)

#define NC_FEATURE_BACKLOG_V40_TRACE_NONE      (0u)
#define NC_FEATURE_BACKLOG_V40_TRACE_BASE      (0x4000u)

/* Added feature IDs: 315-324. */
typedef enum NC_FEATURE_BACKLOG_V40_ID_TAG {
    NC_FEATURE_V40_LASER_AREA_SCANNER_INTRUSION_HOLD = 315,
    NC_FEATURE_V40_SPINDLE_VIBRATION_RISE_TRACE = 316,
    NC_FEATURE_V40_FEED_AXIS_LUBE_PUMP_READY_GATE = 317,
    NC_FEATURE_V40_NC_PROGRAM_SIGNATURE_MISMATCH_INHIBIT = 318,
    NC_FEATURE_V40_CHUCK_SENSOR_DUAL_MISMATCH_INTERLOCK = 319,
    NC_FEATURE_V40_TOOL_BREAK_AUTO_RETRACT_REQUEST = 320,
    NC_FEATURE_V40_TOOL_LENGTH_MEASURE_JUMP_HOLD = 321,
    NC_FEATURE_V40_COOLANT_TEMPERATURE_HIGH_SLOWDOWN = 322,
    NC_FEATURE_V40_PALLET_ROBOT_READY_TIMEOUT_RETRY = 323,
    NC_FEATURE_V40_SAFETY_PLC_HEARTBEAT_LOST_HOLD = 324
} NC_FEATURE_BACKLOG_V40_ID;

typedef enum NC_FEATURE_BACKLOG_V40_POLICY_TAG {
    NC_FEATURE_BACKLOG_V40_POLICY_MONITOR = 0,
    NC_FEATURE_BACKLOG_V40_POLICY_GATE = 1,
    NC_FEATURE_BACKLOG_V40_POLICY_HOLD = 2,
    NC_FEATURE_BACKLOG_V40_POLICY_INTERLOCK = 3,
    NC_FEATURE_BACKLOG_V40_POLICY_SLOWDOWN = 4,
    NC_FEATURE_BACKLOG_V40_POLICY_TRACE = 5,
    NC_FEATURE_BACKLOG_V40_POLICY_INHIBIT = 6,
    NC_FEATURE_BACKLOG_V40_POLICY_RETRY = 7,
    NC_FEATURE_BACKLOG_V40_POLICY_RETRACT = 8
} NC_FEATURE_BACKLOG_V40_POLICY;

typedef struct NC_FEATURE_BACKLOG_V40_BLOCK_TAG {
    int32_t axis_target[NC_FEATURE_BACKLOG_V40_AXIS_COUNT];
    int32_t feed_override_percent;
    uint32_t aux_request_bits;
    uint32_t hold_request;
    uint32_t interlock_mask;
    uint32_t sequence_gate_mask;
    uint32_t diagnostics_mask;
    uint32_t retry_request_mask;
    uint32_t inhibit_mask;
    uint32_t retract_request_mask;
    uint32_t trace_event_code;
} NC_FEATURE_BACKLOG_V40_BLOCK;

typedef struct NC_FEATURE_BACKLOG_V40_RT_IO_TAG {
    uint32_t cycle_counter;
    uint32_t laser_area_scanner_required;
    uint32_t laser_area_intrusion_detected;
    uint32_t spindle_vibration_monitor;
    int32_t spindle_vibration_um;
    uint32_t feed_axis_lube_required_mask;
    uint32_t feed_axis_lube_ready_mask;
    uint32_t nc_program_signature_check_request;
    uint32_t nc_program_signature_ok;
    uint32_t chuck_dual_sensor_check_request;
    uint32_t chuck_sensor_a_closed;
    uint32_t chuck_sensor_b_closed;
    uint32_t tool_break_monitor_request;
    uint32_t tool_break_detected;
    uint32_t tool_length_measure_check_request;
    int32_t tool_length_last_um;
    int32_t tool_length_current_um;
    uint32_t coolant_temperature_monitor;
    int32_t coolant_temperature_c;
    uint32_t pallet_robot_ready_required;
    uint32_t pallet_robot_ready;
    uint32_t safety_plc_heartbeat_monitor;
    uint32_t safety_plc_heartbeat_counter;
} NC_FEATURE_BACKLOG_V40_RT_IO;

typedef struct NC_FEATURE_BACKLOG_V40_STATUS_TAG {
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
    uint32_t retract_count;
    uint32_t warning_count;
    uint32_t cat_mask;
    uint32_t last_feature_id;
    uint32_t last_policy;
    uint32_t last_trace_event_code;
    int32_t last_feed_override_percent;
    int32_t last_x;
    uint32_t last_fault_mask;
    uint32_t last_axis_mask;
    uint32_t last_sensor_a;
    uint32_t last_sensor_b;
    int32_t last_spindle_vibration_um;
    int32_t last_tool_length_delta_um;
    int32_t last_coolant_temperature_c;
    uint32_t last_heartbeat_counter;
    uint32_t per_feature_hits[NC_FEATURE_BACKLOG_V40_FEATURE_COUNT];
    int32_t per_feature_last_value[NC_FEATURE_BACKLOG_V40_FEATURE_COUNT];
} NC_FEATURE_BACKLOG_V40_STATUS;

extern NC_FEATURE_BACKLOG_V40_STATUS g_ncFeatureBacklogV40Status;

void NcFeatureBacklogV40_Init(void);
int NcFeatureBacklogV40_IsFeatureEnabled(uint32_t feature_id);
int Api_SetNcFeatureBacklogV40Feature(uint32_t feature_id, uint32_t enabled);
void Api_EnableAllNcFeatureBacklogV40Features(void);
int Api_GetNcFeatureBacklogV40Status(NC_FEATURE_BACKLOG_V40_STATUS *out_status);
int Api_RunNcFeatureBacklogV40SelfCheck(void);

void NcFeatureBacklogV40_ApplyBlockTs(NC_FEATURE_BACKLOG_V40_BLOCK *block);
void NcFeatureBacklogV40_RunRtTick(const NC_FEATURE_BACKLOG_V40_RT_IO *io);
const char *NcFeatureBacklogV40_GetFeatureName(uint32_t feature_id);

#ifdef __cplusplus
}
#endif

#endif /* NC_FEATURE_BACKLOG_V40_H */
