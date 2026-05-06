/* ibm-bob/samples/base/source/inc/nc_feature_backlog_loader_environment.h */
/* Declares the public contract for nc feature backlog loader environment backlog checks. */
/* This exists so tests and sample builds can share the same fixed-size RT-safe types. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_feature_backlog_loader_environment.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_loader_environment.c, ibm-bob/samples/base/source/scripts/additive_tests.tsv */
#ifndef NC_FEATURE_BACKLOG_LOADER_ENVIRONMENT_H
#define NC_FEATURE_BACKLOG_LOADER_ENVIRONMENT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * IBM-Bob implementation backlog continuation v42_10.
 *
 * This module adds feature IDs 335-344 after v41_10.
 * RT constraints: fixed-size state only, no dynamic allocation,
 * no file I/O, no Windows API calls, and bounded loops.
 */

#define NC_IMPL_BACKLOG_FEATURE_COUNT_V41      (335u)
#define NC_IMPL_BACKLOG_FEATURE_COUNT_V42      (345u)
#define NC_IMPL_BACKLOG_WORD_COUNT_V42         (11u)
#define NC_FEATURE_BACKLOG_V42_FEATURE_BASE_ID (335u)
#define NC_FEATURE_BACKLOG_V42_FEATURE_COUNT   (10u)
#define NC_FEATURE_BACKLOG_V42_AXIS_COUNT      (8u)

#define NC_FEATURE_BACKLOG_V42_OK              (0)
#define NC_FEATURE_BACKLOG_V42_ERR_NULL        (-1)
#define NC_FEATURE_BACKLOG_V42_ERR_RANGE       (-2)

#define NC_FEATURE_BACKLOG_V42_TRACE_NONE      (0u)
#define NC_FEATURE_BACKLOG_V42_TRACE_BASE      (0x4200u)

/* Added feature IDs: 335-344. */
typedef enum NC_FEATURE_BACKLOG_V42_ID_TAG {
    NC_FEATURE_V42_SPINDLE_AIR_PURGE_PRESSURE_GATE = 335,
    NC_FEATURE_V42_AXIS_LIMIT_SWITCH_STUCK_TRACE = 336,
    NC_FEATURE_V42_OIL_MIST_SUPPLY_LOW_HOLD = 337,
    NC_FEATURE_V42_LOADER_CHUCK_CONFIRM_INTERLOCK = 338,
    NC_FEATURE_V42_AUTO_DOOR_MOTION_TIMEOUT_RETRY = 339,
    NC_FEATURE_V42_TABLE_SEATING_SENSOR_MISMATCH_HOLD = 340,
    NC_FEATURE_V42_DOOR_UNLOCK_DURING_SPINDLE_INHIBIT = 341,
    NC_FEATURE_V42_TOOL_RFID_READ_FAIL_GATE = 342,
    NC_FEATURE_V42_MACHINING_ROOM_LIGHT_FAULT_TRACE = 343,
    NC_FEATURE_V42_SERVO_TEMPERATURE_RISE_SLOWDOWN = 344
} NC_FEATURE_BACKLOG_V42_ID;

typedef enum NC_FEATURE_BACKLOG_V42_POLICY_TAG {
    NC_FEATURE_BACKLOG_V42_POLICY_MONITOR = 0,
    NC_FEATURE_BACKLOG_V42_POLICY_GATE = 1,
    NC_FEATURE_BACKLOG_V42_POLICY_HOLD = 2,
    NC_FEATURE_BACKLOG_V42_POLICY_INTERLOCK = 3,
    NC_FEATURE_BACKLOG_V42_POLICY_SLOWDOWN = 4,
    NC_FEATURE_BACKLOG_V42_POLICY_TRACE = 5,
    NC_FEATURE_BACKLOG_V42_POLICY_INHIBIT = 6,
    NC_FEATURE_BACKLOG_V42_POLICY_RETRY = 7
} NC_FEATURE_BACKLOG_V42_POLICY;

typedef struct NC_FEATURE_BACKLOG_V42_BLOCK_TAG {
    int32_t axis_target[NC_FEATURE_BACKLOG_V42_AXIS_COUNT];
    int32_t feed_override_percent;
    uint32_t aux_request_bits;
    uint32_t hold_request;
    uint32_t interlock_mask;
    uint32_t sequence_gate_mask;
    uint32_t diagnostics_mask;
    uint32_t retry_request_mask;
    uint32_t inhibit_mask;
    uint32_t trace_event_code;
} NC_FEATURE_BACKLOG_V42_BLOCK;

typedef struct NC_FEATURE_BACKLOG_V42_RT_IO_TAG {
    uint32_t cycle_counter;
    uint32_t spindle_air_purge_required;
    int32_t spindle_air_pressure_kpa;
    uint32_t axis_limit_switch_monitor;
    uint32_t commanded_axis_mask;
    uint32_t limit_switch_active_mask;
    uint32_t oil_mist_monitor;
    int32_t oil_mist_pressure_kpa;
    uint32_t loader_chuck_check_request;
    uint32_t loader_chuck_open_signal;
    uint32_t loader_chuck_closed_signal;
    uint32_t auto_door_command_active;
    uint32_t auto_door_done;
    uint32_t table_seat_check_request;
    uint32_t table_seat_a_mask;
    uint32_t table_seat_b_mask;
    uint32_t door_unlock_request;
    int32_t spindle_speed_rpm;
    uint32_t tool_rfid_required;
    uint32_t tool_rfid_read_ok;
    uint32_t machining_room_light_monitor;
    uint32_t machining_room_light_ok;
    uint32_t servo_temperature_monitor;
    uint32_t servo_temperature_axis_mask;
    int32_t servo_temperature_c;
} NC_FEATURE_BACKLOG_V42_RT_IO;

typedef struct NC_FEATURE_BACKLOG_V42_STATUS_TAG {
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
    int32_t last_pressure_kpa;
    int32_t last_spindle_speed_rpm;
    int32_t last_temperature_c;
    uint32_t last_table_mismatch_mask;
    uint32_t last_tool_rfid_ok;
    uint32_t per_feature_hits[NC_FEATURE_BACKLOG_V42_FEATURE_COUNT];
    int32_t per_feature_last_value[NC_FEATURE_BACKLOG_V42_FEATURE_COUNT];
} NC_FEATURE_BACKLOG_V42_STATUS;

extern NC_FEATURE_BACKLOG_V42_STATUS g_ncFeatureBacklogV42Status;

void NcFeatureBacklogV42_Init(void);
int NcFeatureBacklogV42_IsFeatureEnabled(uint32_t feature_id);
int Api_SetNcFeatureBacklogV42Feature(uint32_t feature_id, uint32_t enabled);
void Api_EnableAllNcFeatureBacklogV42Features(void);
int Api_GetNcFeatureBacklogV42Status(NC_FEATURE_BACKLOG_V42_STATUS *out_status);
int Api_RunNcFeatureBacklogV42SelfCheck(void);

void NcFeatureBacklogV42_ApplyBlockTs(NC_FEATURE_BACKLOG_V42_BLOCK *block);
void NcFeatureBacklogV42_RunRtTick(const NC_FEATURE_BACKLOG_V42_RT_IO *io);
const char *NcFeatureBacklogV42_GetFeatureName(uint32_t feature_id);

#ifdef __cplusplus
}
#endif

#endif /* NC_FEATURE_BACKLOG_LOADER_ENVIRONMENT_H */
