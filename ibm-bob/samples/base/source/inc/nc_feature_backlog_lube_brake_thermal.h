/* ibm-bob/samples/base/source/inc/nc_feature_backlog_lube_brake_thermal.h */
/* Declares the public contract for nc feature backlog lube brake thermal backlog checks. */
/* This exists so tests and sample builds can share the same fixed-size RT-safe types. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_feature_backlog_lube_brake_thermal.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_lube_brake_thermal.c, ibm-bob/samples/base/source/scripts/additive_tests.tsv */
#ifndef NC_FEATURE_BACKLOG_LUBE_BRAKE_THERMAL_H
#define NC_FEATURE_BACKLOG_LUBE_BRAKE_THERMAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * IBM-Bob implementation backlog continuation v36_5.
 *
 * This module adds feature IDs 280-284 after v35_10.
 * The implementation follows the IBM-Bob RT/TS split rules:
 * - fixed-size state only
 * - no dynamic allocation
 * - no file I/O
 * - no Windows API calls
 * - deterministic loops bounded by NC_FEATURE_BACKLOG_V36_FEATURE_COUNT
 */

#define NC_IMPL_BACKLOG_FEATURE_COUNT_V35      (280u)
#define NC_IMPL_BACKLOG_FEATURE_COUNT_V36      (285u)
#define NC_IMPL_BACKLOG_WORD_COUNT_V36         (9u)
#define NC_FEATURE_BACKLOG_V36_FEATURE_BASE_ID (280u)
#define NC_FEATURE_BACKLOG_V36_FEATURE_COUNT   (5u)
#define NC_FEATURE_BACKLOG_V36_AXIS_COUNT      (8u)

#define NC_FEATURE_BACKLOG_V36_OK              (0)
#define NC_FEATURE_BACKLOG_V36_ERR_NULL        (-1)
#define NC_FEATURE_BACKLOG_V36_ERR_RANGE       (-2)

#define NC_FEATURE_BACKLOG_V36_TRACE_NONE      (0u)
#define NC_FEATURE_BACKLOG_V36_TRACE_BASE      (0x3600u)

/* Added feature IDs: 280-284. */
typedef enum NC_FEATURE_BACKLOG_V36_ID_TAG {
    NC_FEATURE_V36_LUBE_PRESSURE_LOW_STAGED_HOLD = 280,
    NC_FEATURE_V36_AXIS_BRAKE_RELEASE_FEEDBACK_GUARD = 281,
    NC_FEATURE_V36_CHUCK_CLAMP_PRESSURE_CONFIRM = 282,
    NC_FEATURE_V36_PALLET_CLAMP_UNCLAMP_TIMEOUT = 283,
    NC_FEATURE_V36_THERMAL_DRIFT_COMP_READY_GATE = 284
} NC_FEATURE_BACKLOG_V36_ID;

typedef enum NC_FEATURE_BACKLOG_V36_POLICY_TAG {
    NC_FEATURE_BACKLOG_V36_POLICY_MONITOR = 0,
    NC_FEATURE_BACKLOG_V36_POLICY_GATE = 1,
    NC_FEATURE_BACKLOG_V36_POLICY_HOLD = 2,
    NC_FEATURE_BACKLOG_V36_POLICY_INTERLOCK = 3,
    NC_FEATURE_BACKLOG_V36_POLICY_SLOWDOWN = 4,
    NC_FEATURE_BACKLOG_V36_POLICY_TRACE = 5
} NC_FEATURE_BACKLOG_V36_POLICY;

typedef struct NC_FEATURE_BACKLOG_V36_BLOCK_TAG {
    int32_t axis_target[NC_FEATURE_BACKLOG_V36_AXIS_COUNT];
    int32_t feed_override_percent;
    int32_t thermal_comp_offset_um[NC_FEATURE_BACKLOG_V36_AXIS_COUNT];
    uint32_t aux_request_bits;
    uint32_t hold_request;
    uint32_t interlock_mask;
    uint32_t sequence_gate_mask;
    uint32_t diagnostics_mask;
    uint32_t trace_event_code;
} NC_FEATURE_BACKLOG_V36_BLOCK;

typedef struct NC_FEATURE_BACKLOG_V36_RT_IO_TAG {
    uint32_t cycle_counter;
    uint32_t lube_pump_on;
    uint32_t lube_pressure_ok;
    uint32_t lube_level_ok;
    uint32_t axis_brake_release_command_mask;
    uint32_t axis_brake_released_feedback_mask;
    uint32_t axis_brake_expected_mask;
    uint32_t chuck_clamp_command;
    uint32_t chuck_unclamp_command;
    uint32_t chuck_pressure_ok;
    uint32_t pallet_clamp_request;
    uint32_t pallet_unclamp_request;
    uint32_t pallet_clamped_sensor;
    uint32_t pallet_unclamped_sensor;
    uint32_t thermal_comp_request;
    uint32_t thermal_comp_ready;
    int32_t thermal_delta_c_x10;
} NC_FEATURE_BACKLOG_V36_RT_IO;

typedef struct NC_FEATURE_BACKLOG_V36_STATUS_TAG {
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
    uint32_t last_fault_mask;
    uint32_t per_feature_hits[NC_FEATURE_BACKLOG_V36_FEATURE_COUNT];
    int32_t per_feature_last_value[NC_FEATURE_BACKLOG_V36_FEATURE_COUNT];
} NC_FEATURE_BACKLOG_V36_STATUS;

extern NC_FEATURE_BACKLOG_V36_STATUS g_ncFeatureBacklogV36Status;

void NcFeatureBacklogV36_Init(void);
int NcFeatureBacklogV36_IsFeatureEnabled(uint32_t feature_id);
int Api_SetNcFeatureBacklogV36Feature(uint32_t feature_id, uint32_t enabled);
void Api_EnableAllNcFeatureBacklogV36Features(void);
int Api_GetNcFeatureBacklogV36Status(NC_FEATURE_BACKLOG_V36_STATUS *out_status);
int Api_RunNcFeatureBacklogV36SelfCheck(void);

void NcFeatureBacklogV36_ApplyBlockTs(NC_FEATURE_BACKLOG_V36_BLOCK *block);
void NcFeatureBacklogV36_RunRtTick(const NC_FEATURE_BACKLOG_V36_RT_IO *io);
const char *NcFeatureBacklogV36_GetFeatureName(uint32_t feature_id);

#ifdef __cplusplus
}
#endif

#endif /* NC_FEATURE_BACKLOG_LUBE_BRAKE_THERMAL_H */
