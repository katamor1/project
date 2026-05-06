/* ibm-bob/samples/base/source/inc/nc_feature_backlog_aux_safety.h */
/* Declares the public contract for nc feature backlog aux safety backlog checks. */
/* This exists so tests and sample builds can share the same fixed-size RT-safe types. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_feature_backlog_aux_safety.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_aux_safety.c, ibm-bob/samples/base/source/scripts/additive_tests.tsv */
#ifndef NC_FEATURE_BACKLOG_AUX_SAFETY_H
#define NC_FEATURE_BACKLOG_AUX_SAFETY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * IBM-Bob implementation backlog continuation v35_10.
 *
 * This module is intentionally fixed-size and RT-safe:
 * - no dynamic allocation
 * - no file I/O
 * - no Windows API calls
 * - deterministic loops bounded by NC_FEATURE_BACKLOG_V35_FEATURE_COUNT
 */

#define NC_IMPL_BACKLOG_FEATURE_COUNT_V34   (270u)
#define NC_IMPL_BACKLOG_FEATURE_COUNT_V35   (280u)
#define NC_IMPL_BACKLOG_WORD_COUNT_V35      (9u)
#define NC_FEATURE_BACKLOG_V35_FEATURE_BASE_ID (270u)
#define NC_FEATURE_BACKLOG_V35_FEATURE_COUNT   (10u)
#define NC_FEATURE_BACKLOG_V35_AXIS_COUNT      (8u)

#define NC_FEATURE_BACKLOG_V35_OK              (0)
#define NC_FEATURE_BACKLOG_V35_ERR_NULL        (-1)
#define NC_FEATURE_BACKLOG_V35_ERR_RANGE       (-2)

#define NC_FEATURE_BACKLOG_V35_TRACE_NONE      (0u)
#define NC_FEATURE_BACKLOG_V35_TRACE_BASE      (0x3500u)

/* Added feature IDs: 270-279. */
typedef enum NC_FEATURE_BACKLOG_V35_ID_TAG {
    NC_FEATURE_V35_SPINDLE_ORIENT_DONE_WATCHDOG = 270,
    NC_FEATURE_V35_ATC_ARM_HOME_MISMATCH_GUARD = 271,
    NC_FEATURE_V35_CLAMP_UNCLAMP_DOUBLE_CONFIRM = 272,
    NC_FEATURE_V35_WORK_SEATING_AIR_BLOW_CONFIRM = 273,
    NC_FEATURE_V35_CHIP_CONVEYOR_OVERLOAD_HOLD = 274,
    NC_FEATURE_V35_COOLANT_PUMP_DRY_RUN_PROTECT = 275,
    NC_FEATURE_V35_SPINDLE_COOLANT_TEMP_STAGED_SLOWDOWN = 276,
    NC_FEATURE_V35_SERVO_AMP_READY_LOSS_HOLD = 277,
    NC_FEATURE_V35_ESTOP_CHAIN_DISCREPANCY_TRACE = 278,
    NC_FEATURE_V35_PROTECTIVE_COVER_LOCK_TIMEOUT = 279
} NC_FEATURE_BACKLOG_V35_ID;

typedef enum NC_FEATURE_BACKLOG_V35_POLICY_TAG {
    NC_FEATURE_BACKLOG_V35_POLICY_MONITOR = 0,
    NC_FEATURE_BACKLOG_V35_POLICY_GATE = 1,
    NC_FEATURE_BACKLOG_V35_POLICY_HOLD = 2,
    NC_FEATURE_BACKLOG_V35_POLICY_INTERLOCK = 3,
    NC_FEATURE_BACKLOG_V35_POLICY_SLOWDOWN = 4,
    NC_FEATURE_BACKLOG_V35_POLICY_TRACE = 5
} NC_FEATURE_BACKLOG_V35_POLICY;

typedef struct NC_FEATURE_BACKLOG_V35_BLOCK_TAG {
    int32_t axis_target[NC_FEATURE_BACKLOG_V35_AXIS_COUNT];
    int32_t feed_override_percent;
    uint32_t aux_request_bits;
    uint32_t hold_request;
    uint32_t interlock_mask;
    uint32_t sequence_gate_mask;
    uint32_t diagnostics_mask;
    uint32_t trace_event_code;
} NC_FEATURE_BACKLOG_V35_BLOCK;

typedef struct NC_FEATURE_BACKLOG_V35_RT_IO_TAG {
    uint32_t cycle_counter;
    uint32_t spindle_orient_request;
    uint32_t spindle_orient_done;
    uint32_t atc_arm_home_request;
    uint32_t atc_arm_home_sensor;
    uint32_t clamp_command;
    uint32_t unclamp_command;
    uint32_t clamp_confirm_sensor;
    uint32_t unclamp_confirm_sensor;
    uint32_t work_seat_check_request;
    uint32_t work_seated_sensor;
    uint32_t air_pressure_ok;
    uint32_t chip_conveyor_overload;
    uint32_t coolant_pump_on;
    uint32_t coolant_flow_ok;
    int32_t spindle_coolant_temp_c_x10;
    uint32_t servo_amp_ready_mask;
    uint32_t servo_amp_expected_mask;
    uint32_t emergency_stop_input;
    uint32_t safety_relay_feedback;
    uint32_t protective_cover_lock_request;
    uint32_t protective_cover_locked;
} NC_FEATURE_BACKLOG_V35_RT_IO;

typedef struct NC_FEATURE_BACKLOG_V35_STATUS_TAG {
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
    uint32_t warning_count;
    uint32_t cat_mask;
    uint32_t last_feature_id;
    uint32_t last_policy;
    uint32_t last_trace_event_code;
    int32_t last_feed_override_percent;
    int32_t last_x;
    uint32_t per_feature_hits[NC_FEATURE_BACKLOG_V35_FEATURE_COUNT];
    int32_t per_feature_last_value[NC_FEATURE_BACKLOG_V35_FEATURE_COUNT];
} NC_FEATURE_BACKLOG_V35_STATUS;

extern NC_FEATURE_BACKLOG_V35_STATUS g_ncFeatureBacklogV35Status;

void NcFeatureBacklogV35_Init(void);
int NcFeatureBacklogV35_IsFeatureEnabled(uint32_t feature_id);
int Api_SetNcFeatureBacklogV35Feature(uint32_t feature_id, uint32_t enabled);
void Api_EnableAllNcFeatureBacklogV35Features(void);
int Api_GetNcFeatureBacklogV35Status(NC_FEATURE_BACKLOG_V35_STATUS *out_status);
int Api_RunNcFeatureBacklogV35SelfCheck(void);

void NcFeatureBacklogV35_ApplyBlockTs(NC_FEATURE_BACKLOG_V35_BLOCK *block);
void NcFeatureBacklogV35_RunRtTick(const NC_FEATURE_BACKLOG_V35_RT_IO *io);
const char *NcFeatureBacklogV35_GetFeatureName(uint32_t feature_id);

#ifdef __cplusplus
}
#endif

#endif /* NC_FEATURE_BACKLOG_AUX_SAFETY_H */
