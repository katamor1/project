/* ibm-bob/samples/base/source/inc/system_shared.h */
/* Defines public shared memory types for the IBM BOB baseline sample. */
/* This exists so API, TS, and RT code exchange fixed-size state safely. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/system_config.h, ibm-bob/samples/base/source/src/system_shared.c, ibm-bob/samples/base/source/inc/control_api.h */
#ifndef SYSTEM_SHARED_H
#define SYSTEM_SHARED_H

#include <stdint.h>
#include <stddef.h>
#include "system_config.h"

typedef enum {
    RUN_MODE_MANUAL = 0,
    RUN_MODE_AUTO   = 1,
    RUN_MODE_MAINT  = 2
} RUN_MODE;

typedef enum {
    MACHINE_STOPPED = 0,
    MACHINE_READY   = 1,
    MACHINE_RUNNING = 2,
    MACHINE_ALARM   = 3
} MACHINE_STATE;

typedef enum {
    RESPONSE_NONE        = 0,
    RESPONSE_ACCEPTED    = 1,
    RESPONSE_REJECTED    = 2,
    RESPONSE_COMPLETED   = 3,
    RESPONSE_PARAM_ERROR = 4
} RESPONSE_CODE;

typedef enum {
    LOG_INFO  = 0,
    LOG_WARN  = 1,
    LOG_ALARM = 2,
    LOG_AUDIT = 3
} LOG_EVENT_TYPE;

typedef enum {
    NC_IDLE           = 0,
    NC_LOAD_REQUESTED = 1,
    NC_PREFETCHING    = 2,
    NC_READY          = 3,
    NC_RUNNING        = 4,
    NC_HOLD           = 5,
    NC_COMPLETED      = 6,
    NC_ERROR          = 7
} NC_PROGRAM_STATE;

typedef enum {
    NC_ERR_NONE           = 0,
    NC_ERR_FILE_OPEN      = 1,
    NC_ERR_FILE_READ      = 2,
    NC_ERR_LINE_TOO_LONG  = 3,
    NC_ERR_TOKEN          = 4,
    NC_ERR_UNSUPPORTED    = 5,
    NC_ERR_RANGE          = 6,
    NC_ERR_BUFFER_UNDERRUN = 7,
    NC_ERR_GENERATION     = 8,
    NC_ERR_INTERLOCK      = 9,
    NC_ERR_MODAL_CONFLICT = 10,
    NC_ERR_ARC_GEOMETRY   = 11,
    NC_ERR_FEED           = 12,
    NC_ERR_COORDINATE     = 13,
    NC_ERR_UNIT           = 14,
    NC_ERR_TOOL_LIFE_EXPIRED = 15
} NC_ERROR_CODE;

typedef enum {
    NC_MOTION_NONE              = 0,
    NC_MOTION_RAPID             = 1,
    NC_MOTION_LINEAR            = 2,
    NC_MOTION_AUX_ONLY          = 3,
    NC_MOTION_ARC_CW            = 4,
    NC_MOTION_ARC_CCW           = 5,
    NC_MOTION_DWELL             = 6,
    NC_MOTION_CANNED_CYCLE      = 7,
    NC_MOTION_THREAD            = 8,
    NC_MOTION_SKIP              = 9,
    NC_MOTION_REFERENCE_RETURN  = 10,
    NC_MOTION_ADVANCED_INTERP   = 11
} NC_MOTION_TYPE;

typedef enum {
    NC_PLANE_XY = 0,
    NC_PLANE_ZX = 1,
    NC_PLANE_YZ = 2
} NC_PLANE;

typedef enum {
    NC_FEED_MODE_PER_MIN = 0,
    NC_FEED_MODE_PER_REV = 1
} NC_FEED_MODE;

typedef enum {
    NC_UNIT_MM   = 0,
    NC_UNIT_INCH = 1
} NC_UNIT_MODE;

typedef enum {
    NC_DISTANCE_ABSOLUTE    = 0,
    NC_DISTANCE_INCREMENTAL = 1
} NC_DISTANCE_MODE;

typedef enum {
    NC_INTERP_IDLE      = 0,
    NC_INTERP_ACTIVE    = 1,
    NC_INTERP_HOLD      = 2,
    NC_INTERP_COMPLETED = 3,
    NC_INTERP_ERROR     = 4
} NC_INTERP_STATE;

typedef enum {
    NC_FEED_PROFILE_NONE      = 0,
    NC_FEED_PROFILE_TRAPEZOID = 1,
    NC_FEED_PROFILE_DWELL     = 2
} NC_FEED_PROFILE;

typedef enum {
    NC_FEED_CTRL_IDLE      = 0,
    NC_FEED_CTRL_ACTIVE    = 1,
    NC_FEED_CTRL_DWELL     = 2,
    NC_FEED_CTRL_HOLD      = 3,
    NC_FEED_CTRL_COMPLETED = 4,
    NC_FEED_CTRL_ERROR     = 5
} NC_FEED_CONTROL_STATE;

typedef struct {
    uint32_t sensor_bits;
    uint32_t seq_bits;
    int32_t  analog[ANALOG_CH_MAX];
} IO_INPUT_AREA;

typedef struct {
    uint32_t command_bits;
    int32_t  axis_target[AXIS_MAX];
    uint32_t lamp_bits;
    uint32_t aux_m_code;
    uint32_t spindle_speed;
    uint32_t tool_no;
    uint32_t aux_flags;
} IO_OUTPUT_AREA;

typedef struct {
    RUN_MODE      run_mode;
    MACHINE_STATE machine_state;
    uint32_t      alarm_code;
    uint32_t      cycle_count;
    uint32_t      rt_total_ticks;
    uint32_t      rt_input_ticks;
    uint32_t      rt_control_ticks;
    uint32_t      rt_output_ticks;
} MACHINE_CONTEXT;

typedef struct {
    uint8_t       mode_change_request;
    uint8_t       requested_mode;
    uint8_t       mode_response;
    uint8_t       log_flush_request;
    uint32_t      ui_message_id;
    char          ui_message_text[UI_MESSAGE_TEXT_MAX];
} UI_REQUEST_AREA;

typedef struct {
    uint32_t event_type;
    uint32_t code;
    int32_t  value;
    uint32_t cycle;
} LOG_ITEM;

typedef struct {
    uint16_t head;
    uint16_t tail;
    LOG_ITEM items[LOG_QUEUE_MAX];
} LOG_QUEUE;

typedef struct {
    uint8_t  request;
    uint8_t  busy;
    uint8_t  ready;
    uint16_t section_id;
    uint16_t generation;
} PREFETCH_REQ_CTRL;

typedef struct {
    uint16_t section_id;
    uint16_t generation;
    float    table[PREFETCH_TABLE_SIZE];
} PREFETCH_RESULT;

typedef struct {
    RUN_MODE      run_mode;
    MACHINE_STATE machine_state;
    uint32_t      alarm_code;
    uint32_t      cycle_count;
    uint32_t      command_bits;
    uint32_t      sensor_bits;
    uint8_t       mode_response;
    uint8_t       prefetch_ready;
    uint16_t      prefetch_generation;
} STATUS_SNAPSHOT;

typedef struct {
    uint32_t request_seq;
    uint8_t  load_request;
    uint8_t  start_request;
    uint8_t  hold_request;
    uint8_t  stop_request;
    uint8_t  resume_request;
    uint8_t  reset_request;
    char     file_path[NC_FILE_PATH_MAX];
} NC_PROGRAM_REQUEST;

typedef struct {
    NC_PROGRAM_STATE state;
    NC_ERROR_CODE    error_code;
    uint32_t         error_line_no;
    uint32_t         read_line_no;
    uint32_t         exec_line_no;
    uint32_t         buffered_blocks;
    uint32_t         generation;
    uint8_t          response_code;
} NC_PROGRAM_STATUS;

typedef struct {
    uint32_t       valid;
    uint32_t       generation;
    uint32_t       line_no;
    NC_MOTION_TYPE motion_type;
    int32_t        axis_start[AXIS_MAX];
    int32_t        axis_target[AXIS_MAX];
    uint32_t       axis_mask;
    int32_t        feed;
    uint32_t       interp_ticks;
    uint32_t       accel_ticks;
    uint32_t       decel_ticks;
    uint16_t       feed_override_percent;
    NC_FEED_PROFILE feed_profile;
    NC_PLANE       plane;
    NC_FEED_MODE   feed_mode;
    int32_t        arc_center[AXIS_MAX];
    int32_t        arc_radius;
    uint32_t       spindle_speed;
    uint32_t       tool_no;
    uint32_t       aux_m_code;
    uint32_t       aux_flags;
    uint32_t       modal_flags;
    uint32_t       feature_flags;
    int32_t        r_value;
    int32_t        q_value;
    uint32_t       p_word;
    uint16_t       cycle_repeat;
    uint16_t       cycle_flags;
    uint16_t       g_code10;
    uint16_t       cycle_code10;
    uint8_t        return_mode;
    uint8_t        tool_length_command;
    uint8_t        cutter_comp_command;
    uint8_t        rotation_command;
    uint8_t        polar_command;
    uint16_t       h_offset_no;
    uint16_t       d_offset_no;
    uint8_t        kinematics_command;
    uint8_t        tool_axis_command;
    uint8_t        mirror_command;
    uint8_t        axis_retract_command;
    uint32_t       mirror_axis_mask;
    uint32_t       axis_control_mask;
    int32_t        rotation_center[2];
    int32_t        rotation_angle_deg;
    int32_t        tilt_origin[3];
    int32_t        tilt_angle_deg[3];
    int32_t        tool_axis_angle_deg[3];
} NC_EXEC_BLOCK;

typedef struct {
    uint32_t      head_index;
    uint32_t      tail_index;
    uint32_t      producer_generation;
    uint32_t      consumer_generation;
    uint8_t       eof_reached;
    NC_EXEC_BLOCK slots[NC_MAX_LOOKAHEAD_LINES];
} NC_EXEC_BUFFER;

typedef struct {
    uint8_t  selected_work_index;
    NC_UNIT_MODE     unit_mode;
    NC_DISTANCE_MODE distance_mode;
    uint32_t active_local_mask;
    uint32_t active_temporary_mask;
    int32_t  program_position[AXIS_MAX];
    int32_t  work_offset[NC_WORK_COORD_SYSTEMS][AXIS_MAX];
    int32_t  local_shift[AXIS_MAX];
    int32_t  temporary_shift[AXIS_MAX];
    int32_t  machine_position[AXIS_MAX];
    uint32_t generation;
} NC_COORDINATE_STATE;

typedef struct {
    NC_INTERP_STATE state;
    NC_ERROR_CODE   last_error;
    uint32_t        line_no;
    uint32_t        active_tick;
    uint32_t        active_total_ticks;
    uint32_t        completed_segments;
    uint32_t        completed_ticks;
    uint32_t        generation;
} NC_INTERP_STATUS;

typedef struct {
    NC_FEED_CONTROL_STATE state;
    NC_FEED_PROFILE       feed_profile;
    uint16_t              feed_override_percent;
    uint32_t              line_no;
    uint32_t              active_tick;
    uint32_t              active_total_ticks;
    uint32_t              completed_dwell_ticks;
    uint32_t              generation;
} NC_FEED_STATUS;

typedef struct {
    uint32_t active_feature_flags;
    uint32_t last_block_feature_flags;
    uint32_t last_g_code10;
    uint32_t last_cycle_code10;
    uint32_t last_line_no;
    uint32_t executed_feature_blocks;
    uint32_t canned_cycle_blocks;
    uint32_t thread_blocks;
    uint32_t skip_blocks;
    uint32_t reference_return_blocks;
    uint32_t advanced_interp_blocks;
    uint32_t g08_standard_blocks;
    uint32_t g08_high_precision_blocks;
    uint32_t g08_cancel_blocks;
    uint32_t hpcc_blocks;
    uint32_t hpcc_cancel_blocks;
    uint32_t g05_p3_blocks;
    uint8_t  return_mode;
    uint8_t  exact_stop_enabled;
    uint8_t  high_precision_enabled;
    uint8_t  smoothing_enabled;
    uint8_t  hpcc_enabled;
    uint8_t  high_speed_mode_enabled;
    uint8_t  sync_enabled;
    uint8_t  compound_enabled;
    uint8_t  overlay_enabled;
    uint32_t generation;
} NC_FEATURE_STATUS;

#define PRESTART_ILK_ESTOP          (0x00000001UL)
#define PRESTART_ILK_SERVO_ALARM    (0x00000002UL)
#define PRESTART_ILK_PRESSURE_RANGE (0x00000004UL)
#define PRESTART_ILK_READY_SWITCH   (0x00000008UL)
#define PRESTART_ILK_MACHINE_ALARM  (0x00000010UL)
#define PRESTART_ILK_NC_BUFFER      (0x00000020UL)

#define NC_AUX_FLAG_MFIN_WAIT       (0x00000008UL)

#define NC_FEATURE_FLAG_TOOL_LENGTH      (0x00002000UL)
#define NC_FEATURE_FLAG_CUTTER_RADIUS    (0x00004000UL)
#define NC_FEATURE_FLAG_COORD_ROTATION   (0x00008000UL)
#define NC_FEATURE_FLAG_POLAR_COORD      (0x00010000UL)
#define NC_FEATURE_FLAG_TILTED_PLANE     (0x00020000UL)
#define NC_FEATURE_FLAG_TOOL_AXIS_DIR    (0x00040000UL)
#define NC_FEATURE_FLAG_AXIS_ASSIGNMENT  (0x00080000UL)
#define NC_FEATURE_FLAG_AXIS_RETRACT     (0x00100000UL)
#define NC_FEATURE_FLAG_MIRROR           (0x00200000UL)

#define NC_TOOL_LENGTH_CMD_NONE          (0U)
#define NC_TOOL_LENGTH_CMD_POSITIVE      (1U)
#define NC_TOOL_LENGTH_CMD_NEGATIVE      (2U)
#define NC_TOOL_LENGTH_CMD_CANCEL        (3U)
#define NC_CUTTER_COMP_CMD_NONE          (0U)
#define NC_CUTTER_COMP_CMD_CANCEL        (1U)
#define NC_CUTTER_COMP_CMD_LEFT          (2U)
#define NC_CUTTER_COMP_CMD_RIGHT         (3U)
#define NC_ROTATION_CMD_NONE             (0U)
#define NC_ROTATION_CMD_ENABLE           (1U)
#define NC_ROTATION_CMD_CANCEL           (2U)
#define NC_POLAR_CMD_NONE                (0U)
#define NC_POLAR_CMD_ENABLE              (1U)
#define NC_POLAR_CMD_CANCEL              (2U)
#define NC_KINEMATICS_CMD_NONE           (0U)
#define NC_KINEMATICS_CMD_TILT_ENABLE    (1U)
#define NC_KINEMATICS_CMD_TILT_CANCEL    (2U)
#define NC_TOOL_AXIS_CMD_NONE            (0U)
#define NC_TOOL_AXIS_CMD_ENABLE          (1U)
#define NC_MIRROR_CMD_NONE               (0U)
#define NC_MIRROR_CMD_ENABLE             (1U)
#define NC_MIRROR_CMD_CANCEL             (2U)
#define NC_AXIS_RETRACT_CMD_NONE         (0U)
#define NC_AXIS_RETRACT_CMD_RETRACT      (1U)
#define NC_AXIS_RETRACT_CMD_RECOVER      (2U)

#define NC_MOTION_FILTER_SECOND_STAGE_MOVING_AVERAGE (1U)
#define NC_MOTION_FILTER_SECOND_STAGE_FIR            (2U)
#define NC_EVENT_CODE_AXIS_DECEL_APPLIED             (8101U)
#define NC_EVENT_CODE_TOOL_LIFE_EXPIRED_HOLD         (8201U)
#define NC_EVENT_CODE_MOTION_FILTER_APPLIED          (8301U)
#define NC_EVENT_CODE_MOTION_FILTER_SATURATED        (8302U)
#define NC_EVENT_CODE_MOTION_FILTER_ENDPOINT_CORRECT (8303U)

#define NC_AUX_STATE_IDLE           (0U)
#define NC_AUX_STATE_WAIT_MFIN      (1U)

typedef struct {
    uint8_t  ok;
    uint32_t mask;
    uint32_t sensor_bits;
    int32_t  pressure;
    uint32_t buffered_blocks;
    uint32_t generation;
} PRESTART_INTERLOCK_STATUS;

typedef struct {
    uint8_t  state;
    uint32_t active_m_code;
    uint32_t wait_line_no;
    uint32_t wait_cycles;
    uint32_t last_wait_cycles;
    uint32_t total_wait_cycles;
    uint32_t completed_m_codes;
    uint32_t generation;
} NC_AUX_STATUS;

typedef struct {
    uint8_t  active;
    uint8_t  skip_triggered;
    uint8_t  reserved0;
    uint8_t  reserved1;
    uint32_t line_no;
    uint32_t cycle_code10;
    uint32_t last_cycle_code10;
    uint32_t phase_index;
    uint32_t phase_count;
    uint32_t peck_index;
    uint32_t cycle_repeat;
    uint32_t repeat_completed;
    uint32_t peck_blocks;
    uint32_t tap_blocks;
    uint32_t boring_blocks;
    uint32_t spindle_sync_blocks;
    uint32_t spindle_reverse_events;
    uint32_t manual_return_blocks;
    uint32_t completed_cycles;
    uint32_t completed_skips;
    uint32_t last_skip_cycle;
    int32_t  last_skip_position[AXIS_MAX];
    uint32_t generation;
} NC_CYCLE_STATUS;

typedef struct {
    uint8_t  tool_length_active;
    uint8_t  tool_length_sign;
    uint8_t  cutter_comp_mode;
    uint8_t  rotation_active;
    uint8_t  polar_active;
    uint16_t active_h_no;
    uint16_t active_d_no;
    int32_t  tool_length_offset;
    int32_t  cutter_radius_offset;
    int32_t  rotation_center[2];
    int32_t  rotation_angle_deg;
    uint32_t adjusted_axis_mask;
    uint32_t applied_blocks;
    uint32_t last_line_no;
    uint32_t generation;
} NC_COMPENSATION_STATUS;

typedef struct {
    uint32_t arc_blocks;
    uint32_t helical_blocks;
    uint32_t arc_segments_generated;
    uint32_t lookahead_blocks;
    uint32_t corner_slowdown_events;
    uint32_t last_line_no;
    uint32_t last_corner_angle_deg;
    uint16_t last_override_percent;
    uint32_t generation;
} NC_PATH_CONTROL_STATUS;

typedef struct {
    uint8_t  tilted_plane_active;
    uint8_t  tool_axis_active;
    uint8_t  reserved0;
    uint8_t  reserved1;
    uint32_t logical_axis_mask;
    uint32_t physical_axis_mask;
    uint32_t detached_axis_mask;
    uint32_t mirror_axis_mask;
    uint32_t retract_axis_mask;
    int8_t   axis_map[AXIS_MAX];
    int8_t   axis_sign[AXIS_MAX];
    int32_t  tilt_origin[3];
    int32_t  tilt_angle_deg[3];
    int32_t  tool_axis_angle_deg[3];
    int32_t  tool_axis_vector[3];
    int32_t  last_transformed_target[AXIS_MAX];
    uint32_t transformed_blocks;
    uint32_t mirrored_blocks;
    uint32_t retract_events;
    uint32_t singularity_warnings;
    uint32_t last_line_no;
    uint32_t generation;
} NC_KINEMATICS_STATUS;

typedef struct {
    int32_t  current_load[AXIS_MAX];
    int32_t  filtered_load[AXIS_MAX];
    int32_t  peak_load[AXIS_MAX];
    uint32_t warning_mask;
    uint32_t limit_mask;
    uint32_t critical_mask;
    uint8_t  decel_request;
    uint8_t  staged_decel_level;
    uint16_t decel_override_percent;
    uint32_t warning_events;
    uint32_t limit_events;
    uint32_t critical_events;
    uint32_t last_warning_cycle;
    uint32_t generation;
} NC_AXIS_LOAD_STATUS;

typedef struct {
    uint32_t active_tool_no;
    uint32_t last_tool_no;
    uint32_t last_m_code;
    uint32_t tool_use_count[NC_TOOL_LIFE_TABLE_SIZE];
    uint32_t tool_life_limit[NC_TOOL_LIFE_TABLE_SIZE];
    uint32_t warning_mask;
    uint32_t expired_mask;
    uint32_t warning_events;
    uint32_t expired_events;
    uint32_t last_update_line_no;
    uint32_t generation;
} NC_TOOL_LIFE_STATUS;

typedef struct {
    uint32_t cycle_count;
    RUN_MODE run_mode;
    MACHINE_STATE machine_state;
    NC_PROGRAM_STATE nc_state;
    uint32_t alarm_code;
    uint32_t prestart_mask;
    uint32_t buffered_blocks;
    uint32_t exec_line_no;
    uint32_t read_line_no;
    uint32_t axis_warning_mask;
    uint32_t axis_limit_mask;
    uint32_t axis_critical_mask;
    uint8_t  staged_decel_level;
    uint16_t decel_override_percent;
    uint32_t tool_warning_mask;
    uint32_t tool_expired_mask;
    uint32_t active_tool_no;
    uint32_t completed_segments;
    uint32_t completed_cycles;
    uint32_t aux_wait_cycles;
    uint8_t  trace_frozen;
    uint32_t generation;
} NC_DIAGNOSTIC_SNAPSHOT;

typedef struct {
    uint8_t  enabled;
    uint8_t  second_stage_mode;
    uint8_t  velocity_window;
    uint8_t  accel_window;
    uint32_t applied_samples;
    uint32_t filtered_axis_mask;
    int32_t  last_raw_velocity[AXIS_MAX];
    int32_t  last_velocity_stage1[AXIS_MAX];
    int32_t  last_velocity_stage2[AXIS_MAX];
    int32_t  last_filtered_target[AXIS_MAX];
    int32_t  max_raw_velocity[AXIS_MAX];
    int32_t  max_filtered_velocity[AXIS_MAX];
    int32_t  max_velocity_per_tick[AXIS_MAX];
    int32_t  max_accel_per_tick[AXIS_MAX];
    int32_t  last_lag_error[AXIS_MAX];
    int32_t  max_lag_error[AXIS_MAX];
    int32_t  endpoint_residual[AXIS_MAX];
    uint32_t velocity_limit_events;
    uint32_t accel_limit_events;
    uint32_t endpoint_corrections;
    uint32_t residual_correction_samples;
    uint8_t  endpoint_correction_window;
    uint32_t generation;
} NC_MOTION_FILTER_STATUS;

typedef struct {
    uint32_t cycle;
    uint32_t code;
    int32_t  value0;
    int32_t  value1;
    uint32_t line_no;
    NC_PROGRAM_STATE nc_state;
} NC_EVENT_ITEM;

typedef struct {
    uint32_t write_index;
    uint32_t captured_count;
    uint32_t lost_count;
    uint32_t generation;
    NC_EVENT_ITEM items[NC_EVENT_RING_SIZE];
} NC_EVENT_RING;

typedef struct {
    uint8_t  load_request;
    uint8_t  busy;
    uint8_t  ready;
    uint8_t  rejected;
    uint32_t requested_count;
    uint32_t accepted_count;
    uint32_t generation;
    NC_EXEC_BLOCK blocks[NC_BINARY_PROGRAM_MAX_BLOCKS];
} NC_BINARY_PROGRAM_REQUEST;

typedef struct {
    uint32_t loaded_blocks;
    uint32_t rejected_blocks;
    uint32_t committed_blocks;
    uint32_t last_error_line_no;
    uint8_t  active;
    uint8_t  source_ready;
    uint8_t  generation;
    uint8_t  reserved0;
} NC_BINARY_PROGRAM_STATUS;

typedef struct {
    uint32_t parsed_blocks;
    uint32_t supported_g_blocks;
    uint32_t supported_m_blocks;
    uint32_t address_words_seen;
    uint32_t unsupported_g_events;
    uint32_t unsupported_m_events;
    uint32_t unsupported_address_events;
    uint32_t last_g_code10;
    uint32_t last_m_code;
    uint32_t last_address_mask;
    uint32_t modal_conflict_events;
    uint32_t g_history[NC_CAPABILITY_G_HISTORY_SIZE];
    uint32_t m_history[NC_CAPABILITY_M_HISTORY_SIZE];
    uint32_t g_history_index;
    uint32_t m_history_index;
    uint32_t generation;
} NC_CAPABILITY_STATUS;

typedef struct {
    uint8_t  enabled;
    uint8_t  last_warning_kind;
    uint8_t  reserved0;
    uint8_t  reserved1;
    uint32_t checked_blocks;
    uint32_t path_interference_warnings;
    uint32_t three_d_interference_warnings;
    uint32_t axis_limit_warnings;
    uint32_t slowdown_applied_blocks;
    uint32_t last_line_no;
    uint32_t last_warning_line_no;
    int32_t  last_clearance[AXIS_MAX];
    uint32_t generation;
} NC_INTERFERENCE_STATUS;

typedef struct {
    uint8_t  external_decel_active;
    uint8_t  powerfail_decel_stop_active;
    uint8_t  position_switch_latched;
    uint8_t  absolute_reference_valid;
    uint16_t active_override_percent;
    uint16_t reserved0;
    uint32_t external_decel_events;
    uint32_t powerfail_events;
    uint32_t position_switch_events;
    uint32_t absolute_ref_lost_events;
    uint32_t last_line_no;
    uint32_t generation;
} NC_SAFETY_MOTION_STATUS;

typedef struct {
    uint32_t cycle;
    uint32_t sensor_bits;
    uint32_t seq_bits;
    uint32_t command_bits;
    uint32_t aux_m_code;
    uint32_t spindle_speed;
    uint32_t tool_no;
    RUN_MODE run_mode;
    MACHINE_STATE machine_state;
    NC_PROGRAM_STATE nc_state;
    uint32_t alarm_code;
    int32_t  axis_target[AXIS_MAX];
} IO_TRACE_ITEM;

typedef struct {
    uint32_t write_index;
    uint32_t captured_count;
    uint8_t  frozen;
    uint32_t trigger_cycle;
    uint32_t trigger_alarm_code;
    uint32_t generation;
    IO_TRACE_ITEM items[IO_TRACE_RING_SIZE];
} IO_TRACE_BUFFER;

extern volatile IO_INPUT_AREA      g_ioIn;
extern volatile IO_OUTPUT_AREA     g_ioOut;
extern volatile MACHINE_CONTEXT    g_machineCtx;
extern volatile UI_REQUEST_AREA    g_uiRequest;
extern volatile LOG_QUEUE          g_logQueue;
extern volatile PREFETCH_REQ_CTRL  g_prefetchReq;
extern volatile PREFETCH_RESULT    g_prefetchResult;
extern volatile NC_PROGRAM_REQUEST g_ncProgramRequest;
extern volatile NC_PROGRAM_STATUS  g_ncProgramStatus;
extern volatile NC_EXEC_BUFFER     g_ncProgramBuffer;
extern volatile NC_COORDINATE_STATE g_ncCoordinateState;
extern volatile NC_INTERP_STATUS   g_ncInterpStatus;
extern volatile NC_FEED_STATUS     g_ncFeedStatus;
extern volatile NC_FEATURE_STATUS  g_ncFeatureStatus;
extern volatile PRESTART_INTERLOCK_STATUS g_prestartInterlockStatus;
extern volatile NC_AUX_STATUS      g_ncAuxStatus;
extern volatile NC_CYCLE_STATUS    g_ncCycleStatus;
extern volatile NC_COMPENSATION_STATUS g_ncCompensationStatus;
extern volatile NC_PATH_CONTROL_STATUS g_ncPathControlStatus;
extern volatile NC_KINEMATICS_STATUS g_ncKinematicsStatus;
extern volatile NC_AXIS_LOAD_STATUS  g_ncAxisLoadStatus;
extern volatile NC_TOOL_LIFE_STATUS  g_ncToolLifeStatus;
extern volatile NC_DIAGNOSTIC_SNAPSHOT g_ncDiagnosticSnapshot;
extern volatile NC_MOTION_FILTER_STATUS g_ncMotionFilterStatus;
extern volatile NC_EVENT_RING    g_ncEventRing;
extern volatile NC_BINARY_PROGRAM_REQUEST g_ncBinaryProgramRequest;
extern volatile NC_BINARY_PROGRAM_STATUS  g_ncBinaryProgramStatus;
extern volatile NC_CAPABILITY_STATUS      g_ncCapabilityStatus;
extern volatile NC_INTERFERENCE_STATUS    g_ncInterferenceStatus;
extern volatile NC_SAFETY_MOTION_STATUS   g_ncSafetyMotionStatus;
extern volatile IO_TRACE_BUFFER    g_ioTraceBuffer;

void SystemShared_Initialize(void);
int32_t LogQueue_Push(uint32_t eventType, uint32_t code, int32_t value);
void NcEvent_Push(uint32_t code, int32_t value0, int32_t value1, uint32_t lineNo);

#endif /* SYSTEM_SHARED_H */
