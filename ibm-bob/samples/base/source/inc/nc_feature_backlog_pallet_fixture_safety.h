/* ibm-bob/samples/base/source/inc/nc_feature_backlog_pallet_fixture_safety.h */
/* Declares the public contract for nc feature backlog pallet fixture safety backlog checks. */
/* This exists so tests and sample builds can share the same fixed-size RT-safe types. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_feature_backlog_pallet_fixture_safety.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_pallet_fixture_safety.c, ibm-bob/samples/base/source/scripts/additive_tests.tsv */
#ifndef NC_FEATURE_BACKLOG_PALLET_FIXTURE_SAFETY_H
#define NC_FEATURE_BACKLOG_PALLET_FIXTURE_SAFETY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * IBM-Bob implementation backlog continuation v38_10.
 *
 * This module adds feature IDs 295-304 after v37_10.
 * The implementation follows the IBM-Bob RT/TS split rules:
 * - fixed-size state only
 * - no dynamic allocation
 * - no file I/O
 * - no Windows API calls
 * - deterministic loops bounded by NC_FEATURE_BACKLOG_V38_FEATURE_COUNT
 */

#define NC_IMPL_BACKLOG_FEATURE_COUNT_V37      (295u)
#define NC_IMPL_BACKLOG_FEATURE_COUNT_V38      (305u)
#define NC_IMPL_BACKLOG_WORD_COUNT_V38         (10u)
#define NC_FEATURE_BACKLOG_V38_FEATURE_BASE_ID (295u)
#define NC_FEATURE_BACKLOG_V38_FEATURE_COUNT   (10u)
#define NC_FEATURE_BACKLOG_V38_AXIS_COUNT      (8u)

#define NC_FEATURE_BACKLOG_V38_OK              (0)
#define NC_FEATURE_BACKLOG_V38_ERR_NULL        (-1)
#define NC_FEATURE_BACKLOG_V38_ERR_RANGE       (-2)

#define NC_FEATURE_BACKLOG_V38_TRACE_NONE      (0u)
#define NC_FEATURE_BACKLOG_V38_TRACE_BASE      (0x3800u)

/* Added feature IDs: 295-304. */
typedef enum NC_FEATURE_BACKLOG_V38_ID_TAG {
    NC_FEATURE_V38_MACHINE_THERMAL_STABLE_READY_INHIBIT = 295,
    NC_FEATURE_V38_HYDRAULIC_FILTER_CLOG_TRACE = 296,
    NC_FEATURE_V38_ATC_POT_INDEX_CONFIRM_INTERLOCK = 297,
    NC_FEATURE_V38_PALLET_ID_MISMATCH_GATE = 298,
    NC_FEATURE_V38_Z_COUNTERBALANCE_PRESSURE_LOW_HOLD = 299,
    NC_FEATURE_V38_SERVO_ABSOLUTE_POSITION_MISMATCH_TRACE = 300,
    NC_FEATURE_V38_SPINDLE_TOOL_UNCLAMP_CONFIRM_RETRY = 301,
    NC_FEATURE_V38_COOLANT_TANK_LEVEL_LOW_GATE = 302,
    NC_FEATURE_V38_MIST_COLLECTOR_READY_GATE = 303,
    NC_FEATURE_V38_SAFETY_MAT_INTRUSION_HOLD = 304
} NC_FEATURE_BACKLOG_V38_ID;

typedef enum NC_FEATURE_BACKLOG_V38_POLICY_TAG {
    NC_FEATURE_BACKLOG_V38_POLICY_MONITOR = 0,
    NC_FEATURE_BACKLOG_V38_POLICY_GATE = 1,
    NC_FEATURE_BACKLOG_V38_POLICY_HOLD = 2,
    NC_FEATURE_BACKLOG_V38_POLICY_INTERLOCK = 3,
    NC_FEATURE_BACKLOG_V38_POLICY_SLOWDOWN = 4,
    NC_FEATURE_BACKLOG_V38_POLICY_TRACE = 5,
    NC_FEATURE_BACKLOG_V38_POLICY_INHIBIT = 6,
    NC_FEATURE_BACKLOG_V38_POLICY_RETRY = 7
} NC_FEATURE_BACKLOG_V38_POLICY;

typedef struct NC_FEATURE_BACKLOG_V38_BLOCK_TAG {
    int32_t axis_target[NC_FEATURE_BACKLOG_V38_AXIS_COUNT];
    int32_t feed_override_percent;
    uint32_t aux_request_bits;
    uint32_t hold_request;
    uint32_t interlock_mask;
    uint32_t sequence_gate_mask;
    uint32_t diagnostics_mask;
    uint32_t retry_request_mask;
    uint32_t inhibit_mask;
    uint32_t trace_event_code;
} NC_FEATURE_BACKLOG_V38_BLOCK;

typedef struct NC_FEATURE_BACKLOG_V38_RT_IO_TAG {
    uint32_t cycle_counter;
    uint32_t thermal_stable_request;
    uint32_t thermal_temp_ready;
    int32_t thermal_drift_um;
    uint32_t hydraulic_filter_monitored;
    uint32_t hydraulic_filter_clogged;
    uint32_t atc_pot_index_request;
    uint32_t atc_pot_position_ok;
    uint32_t pallet_id_check_request;
    uint32_t pallet_present;
    uint32_t expected_pallet_id;
    uint32_t actual_pallet_id;
    uint32_t z_axis_enabled;
    uint32_t z_counterbalance_pressure_ok;
    uint32_t servo_abs_check_mask;
    uint32_t servo_abs_mismatch_mask;
    uint32_t spindle_tool_unclamp_request;
    uint32_t spindle_tool_unclamp_sensor;
    uint32_t coolant_request;
    uint32_t coolant_tank_level_low;
    uint32_t mist_collector_required;
    uint32_t mist_collector_ready;
    uint32_t safety_mat_enabled;
    uint32_t safety_mat_intrusion;
} NC_FEATURE_BACKLOG_V38_RT_IO;

typedef struct NC_FEATURE_BACKLOG_V38_STATUS_TAG {
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
    uint32_t last_pallet_expected;
    uint32_t last_pallet_actual;
    int32_t last_thermal_drift_um;
    uint32_t per_feature_hits[NC_FEATURE_BACKLOG_V38_FEATURE_COUNT];
    int32_t per_feature_last_value[NC_FEATURE_BACKLOG_V38_FEATURE_COUNT];
} NC_FEATURE_BACKLOG_V38_STATUS;

extern NC_FEATURE_BACKLOG_V38_STATUS g_ncFeatureBacklogV38Status;

void NcFeatureBacklogV38_Init(void);
int NcFeatureBacklogV38_IsFeatureEnabled(uint32_t feature_id);
int Api_SetNcFeatureBacklogV38Feature(uint32_t feature_id, uint32_t enabled);
void Api_EnableAllNcFeatureBacklogV38Features(void);
int Api_GetNcFeatureBacklogV38Status(NC_FEATURE_BACKLOG_V38_STATUS *out_status);
int Api_RunNcFeatureBacklogV38SelfCheck(void);

void NcFeatureBacklogV38_ApplyBlockTs(NC_FEATURE_BACKLOG_V38_BLOCK *block);
void NcFeatureBacklogV38_RunRtTick(const NC_FEATURE_BACKLOG_V38_RT_IO *io);
const char *NcFeatureBacklogV38_GetFeatureName(uint32_t feature_id);

#ifdef __cplusplus
}
#endif

#endif /* NC_FEATURE_BACKLOG_PALLET_FIXTURE_SAFETY_H */
