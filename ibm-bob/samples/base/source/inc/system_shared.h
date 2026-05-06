/* ibm-bob/samples/base/source/inc/system_shared.h */
/* Defines public shared memory types for the IBM BOB baseline sample. */
/* This exists so API, TS, and RT code exchange fixed-size state safely. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/system_config.h, ibm-bob/samples/base/source/src/system_shared.c, ibm-bob/samples/base/source/inc/control_api.h */
#ifndef SYSTEM_SHARED_H
#define SYSTEM_SHARED_H

#include <stdint.h>
#include <stddef.h>
#include "system_config.h"

/**
 * @brief Enumeration for run mode.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    RUN_MODE_MANUAL = 0, /**< Definition value for run mode manual. */
    RUN_MODE_AUTO = 1, /**< Definition value for run mode auto. */
    RUN_MODE_MAINT = 2 /**< Definition value for run mode maint. */
} RUN_MODE;

/**
 * @brief Enumeration for machine state.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    MACHINE_STOPPED = 0, /**< Definition value for machine stopped. */
    MACHINE_READY = 1, /**< Definition value for machine ready. */
    MACHINE_RUNNING = 2, /**< Definition value for machine running. */
    MACHINE_ALARM = 3 /**< Definition value for machine alarm. */
} MACHINE_STATE;

/**
 * @brief Enumeration for response code.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    RESPONSE_NONE = 0, /**< Definition value for response none. */
    RESPONSE_ACCEPTED = 1, /**< Definition value for response accepted. */
    RESPONSE_REJECTED = 2, /**< Definition value for response rejected. */
    RESPONSE_COMPLETED = 3, /**< Definition value for response completed. */
    RESPONSE_PARAM_ERROR = 4 /**< Definition value for response param error. */
} RESPONSE_CODE;

/**
 * @brief Enumeration for log event type.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    LOG_INFO = 0, /**< Definition value for log info. */
    LOG_WARN = 1, /**< Definition value for log warn. */
    LOG_ALARM = 2, /**< Definition value for log alarm. */
    LOG_AUDIT = 3 /**< Definition value for log audit. */
} LOG_EVENT_TYPE;

/**
 * @brief Enumeration for NC program state.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_IDLE = 0, /**< Definition value for NC idle. */
    NC_LOAD_REQUESTED = 1, /**< Definition value for NC load requested. */
    NC_PREFETCHING = 2, /**< Definition value for NC prefetching. */
    NC_READY = 3, /**< Definition value for NC ready. */
    NC_RUNNING = 4, /**< Definition value for NC running. */
    NC_HOLD = 5, /**< Definition value for NC hold. */
    NC_COMPLETED = 6, /**< Definition value for NC completed. */
    NC_ERROR = 7 /**< Definition value for NC error. */
} NC_PROGRAM_STATE;

/**
 * @brief Enumeration for NC error code.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_ERR_NONE = 0, /**< Definition value for NC err none. */
    NC_ERR_FILE_OPEN = 1, /**< Definition value for NC err file open. */
    NC_ERR_FILE_READ = 2, /**< Definition value for NC err file read. */
    NC_ERR_LINE_TOO_LONG = 3, /**< Definition value for NC err line too long. */
    NC_ERR_TOKEN = 4, /**< Definition value for NC err token. */
    NC_ERR_UNSUPPORTED = 5, /**< Definition value for NC err unsupported. */
    NC_ERR_RANGE = 6, /**< Definition value for NC err range. */
    NC_ERR_BUFFER_UNDERRUN = 7, /**< Definition value for NC err buffer underrun. */
    NC_ERR_GENERATION = 8, /**< Definition value for NC err generation. */
    NC_ERR_INTERLOCK = 9, /**< Definition value for NC err interlock. */
    NC_ERR_MODAL_CONFLICT = 10, /**< Definition value for NC err modal conflict. */
    NC_ERR_ARC_GEOMETRY = 11, /**< Definition value for NC err arc geometry. */
    NC_ERR_FEED = 12, /**< Definition value for NC err feed. */
    NC_ERR_COORDINATE = 13, /**< Definition value for NC err coordinate. */
    NC_ERR_UNIT = 14, /**< Definition value for NC err unit. */
    NC_ERR_TOOL_LIFE_EXPIRED = 15 /**< Definition value for NC err tool life expired. */
} NC_ERROR_CODE;

/**
 * @brief Enumeration for NC motion type.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_MOTION_NONE = 0, /**< Definition value for NC motion none. */
    NC_MOTION_RAPID = 1, /**< Definition value for NC motion rapid. */
    NC_MOTION_LINEAR = 2, /**< Definition value for NC motion linear. */
    NC_MOTION_AUX_ONLY = 3, /**< Definition value for NC motion aux only. */
    NC_MOTION_ARC_CW = 4, /**< Definition value for NC motion arc cw. */
    NC_MOTION_ARC_CCW = 5, /**< Definition value for NC motion arc ccw. */
    NC_MOTION_DWELL = 6, /**< Definition value for NC motion dwell. */
    NC_MOTION_CANNED_CYCLE = 7, /**< Definition value for NC motion canned cycle. */
    NC_MOTION_THREAD = 8, /**< Definition value for NC motion thread. */
    NC_MOTION_SKIP = 9, /**< Definition value for NC motion skip. */
    NC_MOTION_REFERENCE_RETURN = 10, /**< Definition value for NC motion reference return. */
    NC_MOTION_ADVANCED_INTERP = 11 /**< Definition value for NC motion advanced interp. */
} NC_MOTION_TYPE;

/**
 * @brief Enumeration for NC plane.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_PLANE_XY = 0, /**< Definition value for NC plane xy. */
    NC_PLANE_ZX = 1, /**< Definition value for NC plane zx. */
    NC_PLANE_YZ = 2 /**< Definition value for NC plane yz. */
} NC_PLANE;

/**
 * @brief Enumeration for NC feed mode.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_FEED_MODE_PER_MIN = 0, /**< Definition value for NC feed mode per min. */
    NC_FEED_MODE_PER_REV = 1 /**< Definition value for NC feed mode per rev. */
} NC_FEED_MODE;

/**
 * @brief Enumeration for NC unit mode.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_UNIT_MM = 0, /**< Definition value for NC unit mm. */
    NC_UNIT_INCH = 1 /**< Definition value for NC unit inch. */
} NC_UNIT_MODE;

/**
 * @brief Enumeration for NC distance mode.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_DISTANCE_ABSOLUTE = 0, /**< Definition value for NC distance absolute. */
    NC_DISTANCE_INCREMENTAL = 1 /**< Definition value for NC distance incremental. */
} NC_DISTANCE_MODE;

/**
 * @brief Enumeration for NC interp state.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_INTERP_IDLE = 0, /**< Definition value for NC interp idle. */
    NC_INTERP_ACTIVE = 1, /**< Definition value for NC interp active. */
    NC_INTERP_HOLD = 2, /**< Definition value for NC interp hold. */
    NC_INTERP_COMPLETED = 3, /**< Definition value for NC interp completed. */
    NC_INTERP_ERROR = 4 /**< Definition value for NC interp error. */
} NC_INTERP_STATE;

/**
 * @brief Enumeration for NC feed profile.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_FEED_PROFILE_NONE = 0, /**< Definition value for NC feed profile none. */
    NC_FEED_PROFILE_TRAPEZOID = 1, /**< Definition value for NC feed profile trapezoid. */
    NC_FEED_PROFILE_DWELL = 2 /**< Definition value for NC feed profile dwell. */
} NC_FEED_PROFILE;

/**
 * @brief Enumeration for NC feed control state.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_FEED_CTRL_IDLE = 0, /**< Definition value for NC feed ctrl idle. */
    NC_FEED_CTRL_ACTIVE = 1, /**< Definition value for NC feed ctrl active. */
    NC_FEED_CTRL_DWELL = 2, /**< Definition value for NC feed ctrl dwell. */
    NC_FEED_CTRL_HOLD = 3, /**< Definition value for NC feed ctrl hold. */
    NC_FEED_CTRL_COMPLETED = 4, /**< Definition value for NC feed ctrl completed. */
    NC_FEED_CTRL_ERROR = 5 /**< Definition value for NC feed ctrl error. */
} NC_FEED_CONTROL_STATE;

/**
 * @brief Structure for NC reference status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t  active;
    uint8_t  reserved0;
    uint16_t reserved1;
    uint32_t requested_axis_mask;
    uint32_t completed_axis_mask;
    uint32_t homed_axis_mask;
    uint32_t reference_marker_axis_mask;
    uint32_t distance_coded_axis_mask;
    uint32_t distance_coded_captured_axis_mask;
    uint32_t planned_reference_return_blocks;
    uint32_t executed_reference_return_blocks;
    uint32_t second_reference_return_blocks;
    uint32_t one_direction_positioning_blocks;
    uint32_t executed_one_direction_blocks;
    uint32_t reference_marker_captures;
    uint32_t distance_coded_marker_captures;
    uint32_t rollover_events;
    uint32_t last_reference_code10;
    uint32_t last_one_direction_axis_mask;
    uint32_t last_line_no;
    uint32_t last_marker_axis;
    uint32_t last_rollover_axis;
    int32_t  reference_position[AXIS_MAX];
    int32_t  last_reference_position[AXIS_MAX];
    int32_t  last_marker_position[AXIS_MAX];
    int32_t  last_approach_start[AXIS_MAX];
    int32_t  rollover_limit[AXIS_MAX];
    int32_t  last_rollover_position;
    int32_t  one_direction_approach[AXIS_MAX];
    int8_t   approach_direction[AXIS_MAX];
    uint32_t generation;
} NC_REFERENCE_STATUS;


/**
 * @brief Structure for IO input area.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint32_t sensor_bits;
    uint32_t seq_bits;
    int32_t  analog[ANALOG_CH_MAX];
} IO_INPUT_AREA;

/**
 * @brief Structure for IO output area.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint32_t command_bits;
    int32_t  axis_target[AXIS_MAX];
    uint32_t lamp_bits;
    uint32_t aux_m_code;
    uint32_t spindle_speed;
    uint32_t tool_no;
    uint32_t aux_flags;
} IO_OUTPUT_AREA;

/**
 * @brief Structure for machine context.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for UI request area.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t       mode_change_request;
    uint8_t       requested_mode;
    uint8_t       mode_response;
    uint8_t       log_flush_request;
    uint32_t      ui_message_id;
    char          ui_message_text[UI_MESSAGE_TEXT_MAX];
} UI_REQUEST_AREA;

/**
 * @brief Structure for log item.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint32_t event_type;
    uint32_t code;
    int32_t  value;
    uint32_t cycle;
} LOG_ITEM;

/**
 * @brief Structure for log queue.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint16_t head;
    uint16_t tail;
    LOG_ITEM items[LOG_QUEUE_MAX];
} LOG_QUEUE;

/**
 * @brief Structure for prefetch req ctrl.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t  request;
    uint8_t  busy;
    uint8_t  ready;
    uint16_t section_id;
    uint16_t generation;
} PREFETCH_REQ_CTRL;

/**
 * @brief Structure for prefetch result.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint16_t section_id;
    uint16_t generation;
    float    table[PREFETCH_TABLE_SIZE];
} PREFETCH_RESULT;

/**
 * @brief Structure for status snapshot.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC program request.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC program status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC exec block.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC exec buffer.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint32_t      head_index;
    uint32_t      tail_index;
    uint32_t      producer_generation;
    uint32_t      consumer_generation;
    uint8_t       eof_reached;
    NC_EXEC_BLOCK slots[NC_MAX_LOOKAHEAD_LINES];
} NC_EXEC_BUFFER;

/**
 * @brief Structure for NC coordinate state.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC coordinate transform status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t  dynamic_fixture_enabled;
    uint8_t  work_mount_error_enabled;
    uint8_t  rotary_fixture_enabled;
    uint8_t  reserved0;
    uint32_t active_correction_mask;
    uint32_t corrected_blocks;
    uint32_t dynamic_fixture_blocks;
    uint32_t work_mount_error_blocks;
    uint32_t rotary_fixture_blocks;
    uint32_t work_offset_set_calls;
    uint32_t local_shift_set_calls;
    uint32_t temporary_absolute_set_calls;
    uint32_t api_rejected_calls;
    uint32_t last_axis_mask;
    uint32_t last_line_no;
    int32_t  dynamic_fixture_offset[AXIS_MAX];
    int32_t  work_mount_error[AXIS_MAX];
    int32_t  rotary_table_offset[AXIS_MAX];
    int32_t  last_input_target[AXIS_MAX];
    int32_t  last_corrected_target[AXIS_MAX];
    uint32_t generation;
} NC_COORDINATE_TRANSFORM_STATUS;

/**
 * @brief Structure for NC interp status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC feed status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC feature status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Definition value for prestart ilk estop.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define PRESTART_ILK_ESTOP          (0x00000001UL)
/**
 * @brief Definition value for prestart ilk servo alarm.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define PRESTART_ILK_SERVO_ALARM    (0x00000002UL)
/**
 * @brief Definition value for prestart ilk pressure range.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define PRESTART_ILK_PRESSURE_RANGE (0x00000004UL)
/**
 * @brief Definition value for prestart ilk ready switch.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define PRESTART_ILK_READY_SWITCH   (0x00000008UL)
/**
 * @brief Definition value for prestart ilk machine alarm.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define PRESTART_ILK_MACHINE_ALARM  (0x00000010UL)
/**
 * @brief Definition value for prestart ilk NC buffer.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define PRESTART_ILK_NC_BUFFER      (0x00000020UL)

/**
 * @brief Definition value for NC aux flag MFIN wait.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AUX_FLAG_MFIN_WAIT       (0x00000008UL)

/**
 * @brief Definition value for NC feature flag tool length.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_TOOL_LENGTH      (0x00002000UL)
/**
 * @brief Definition value for NC feature flag cutter radius.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_CUTTER_RADIUS    (0x00004000UL)
/**
 * @brief Definition value for NC feature flag coord rotation.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_COORD_ROTATION   (0x00008000UL)
/**
 * @brief Definition value for NC feature flag polar coord.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_POLAR_COORD      (0x00010000UL)
/**
 * @brief Definition value for NC feature flag tilted plane.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_TILTED_PLANE     (0x00020000UL)
/**
 * @brief Definition value for NC feature flag tool axis dir.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_TOOL_AXIS_DIR    (0x00040000UL)
/**
 * @brief Definition value for NC feature flag axis assignment.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_AXIS_ASSIGNMENT  (0x00080000UL)
/**
 * @brief Definition value for NC feature flag axis retract.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_AXIS_RETRACT     (0x00100000UL)
/**
 * @brief Definition value for NC feature flag mirror.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_MIRROR           (0x00200000UL)

/**
 * @brief Definition value for NC tool length cmd none.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_LENGTH_CMD_NONE          (0U)
/**
 * @brief Definition value for NC tool length cmd positive.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_LENGTH_CMD_POSITIVE      (1U)
/**
 * @brief Definition value for NC tool length cmd negative.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_LENGTH_CMD_NEGATIVE      (2U)
/**
 * @brief Definition value for NC tool length cmd cancel.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_LENGTH_CMD_CANCEL        (3U)
/**
 * @brief Definition value for NC cutter comp cmd none.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CUTTER_COMP_CMD_NONE          (0U)
/**
 * @brief Definition value for NC cutter comp cmd cancel.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CUTTER_COMP_CMD_CANCEL        (1U)
/**
 * @brief Definition value for NC cutter comp cmd left.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CUTTER_COMP_CMD_LEFT          (2U)
/**
 * @brief Definition value for NC cutter comp cmd right.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CUTTER_COMP_CMD_RIGHT         (3U)
/**
 * @brief Definition value for NC rotation cmd none.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ROTATION_CMD_NONE             (0U)
/**
 * @brief Definition value for NC rotation cmd enable.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ROTATION_CMD_ENABLE           (1U)
/**
 * @brief Definition value for NC rotation cmd cancel.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ROTATION_CMD_CANCEL           (2U)
/**
 * @brief Definition value for NC polar cmd none.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_POLAR_CMD_NONE                (0U)
/**
 * @brief Definition value for NC polar cmd enable.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_POLAR_CMD_ENABLE              (1U)
/**
 * @brief Definition value for NC polar cmd cancel.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_POLAR_CMD_CANCEL              (2U)
/**
 * @brief Definition value for NC kinematics cmd none.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_KINEMATICS_CMD_NONE           (0U)
/**
 * @brief Definition value for NC kinematics cmd tilt enable.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_KINEMATICS_CMD_TILT_ENABLE    (1U)
/**
 * @brief Definition value for NC kinematics cmd tilt cancel.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_KINEMATICS_CMD_TILT_CANCEL    (2U)
/**
 * @brief Definition value for NC tool axis cmd none.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_AXIS_CMD_NONE            (0U)
/**
 * @brief Definition value for NC tool axis cmd enable.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_AXIS_CMD_ENABLE          (1U)
/**
 * @brief Definition value for NC mirror cmd none.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MIRROR_CMD_NONE               (0U)
/**
 * @brief Definition value for NC mirror cmd enable.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MIRROR_CMD_ENABLE             (1U)
/**
 * @brief Definition value for NC mirror cmd cancel.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MIRROR_CMD_CANCEL             (2U)
/**
 * @brief Definition value for NC axis retract cmd none.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AXIS_RETRACT_CMD_NONE         (0U)
/**
 * @brief Definition value for NC axis retract cmd retract.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AXIS_RETRACT_CMD_RETRACT      (1U)
/**
 * @brief Definition value for NC axis retract cmd recover.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AXIS_RETRACT_CMD_RECOVER      (2U)

/**
 * @brief Definition value for NC motion filter second stage moving average.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MOTION_FILTER_SECOND_STAGE_MOVING_AVERAGE (1U)
/**
 * @brief Definition value for NC motion filter second stage FIR.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MOTION_FILTER_SECOND_STAGE_FIR            (2U)
/**
 * @brief Definition value for NC event code axis decel applied.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_AXIS_DECEL_APPLIED             (8101U)
/**
 * @brief Definition value for NC event code tool life expired hold.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_TOOL_LIFE_EXPIRED_HOLD         (8201U)
/**
 * @brief Definition value for NC event code motion filter applied.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_MOTION_FILTER_APPLIED          (8301U)
/**
 * @brief Definition value for NC event code motion filter saturated.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_MOTION_FILTER_SATURATED        (8302U)
/**
 * @brief Definition value for NC event code motion filter endpoint correct.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_MOTION_FILTER_ENDPOINT_CORRECT (8303U)

/**
 * @brief Definition value for NC aux state idle.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AUX_STATE_IDLE           (0U)
/**
 * @brief Definition value for NC aux state wait MFIN.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AUX_STATE_WAIT_MFIN      (1U)

/**
 * @brief Structure for prestart interlock status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t  ok;
    uint32_t mask;
    uint32_t sensor_bits;
    int32_t  pressure;
    uint32_t buffered_blocks;
    uint32_t generation;
} PRESTART_INTERLOCK_STATUS;

/**
 * @brief Structure for NC aux status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC cycle status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC compensation status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC path control status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC kinematics status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC axis load status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC tool life status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC diagnostic snapshot.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC motion filter status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Enumeration for NC spindle direction.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_SPINDLE_DIR_STOP = 0, /**< Definition value for NC spindle dir stop. */
    NC_SPINDLE_DIR_CW = 1, /**< Definition value for NC spindle dir cw. */
    NC_SPINDLE_DIR_CCW = 2 /**< Definition value for NC spindle dir ccw. */
} NC_SPINDLE_DIRECTION;

/**
 * @brief Structure for NC spindle status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t  enabled;
    uint8_t  running;
    uint8_t  direction;
    uint8_t  css_mode_active;
    uint8_t  orient_requested;
    uint8_t  orient_completed;
    uint8_t  clamped;
    uint8_t  reserved0;
    uint32_t commanded_rpm;
    uint32_t output_rpm;
    uint32_t max_rpm;
    uint32_t min_rpm;
    uint32_t css_surface_speed;
    uint32_t last_s_code;
    uint32_t last_m_code;
    uint32_t last_g_code10;
    uint32_t last_line_no;
    uint32_t s_word_blocks;
    uint32_t m_word_blocks;
    uint32_t css_blocks;
    uint32_t fixed_rpm_blocks;
    uint32_t start_events;
    uint32_t stop_events;
    uint32_t direction_change_events;
    uint32_t orientation_events;
    uint32_t orient_timeout_events;
    uint32_t speed_clamp_events;
    uint32_t sync_required_blocks;
    uint16_t orient_elapsed_ticks;
    uint16_t orient_timeout_ticks;
    uint32_t generation;
} NC_SPINDLE_STATUS;

/**
 * @brief Structure for NC tool management status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t  table_initialized;
    uint8_t  prepare_requested;
    uint8_t  exchange_in_progress;
    uint8_t  mfin_required;
    uint32_t active_tool_no;
    uint32_t prepared_tool_no;
    uint32_t requested_tool_no;
    uint32_t last_t_code;
    uint32_t last_m_code;
    uint32_t last_line_no;
    uint32_t tool_to_pocket[NC_TOOL_TABLE_SIZE];
    uint32_t pocket_tool[NC_TOOL_MAGAZINE_POCKETS];
    uint32_t registered_tools;
    uint32_t parsed_t_words;
    uint32_t rt_t_commands;
    uint32_t prepare_events;
    uint32_t exchange_requests;
    uint32_t exchange_completed;
    uint32_t exchange_wait_cycles;
    uint32_t max_exchange_wait_cycles;
    uint32_t invalid_tool_events;
    uint32_t duplicate_pocket_events;
    uint32_t no_pocket_events;
    uint32_t timeout_events;
    uint32_t generation;
} NC_TOOL_MANAGEMENT_STATUS;

/**
 * @brief Structure for NC event item.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint32_t cycle;
    uint32_t code;
    int32_t  value0;
    int32_t  value1;
    uint32_t line_no;
    NC_PROGRAM_STATE nc_state;
} NC_EVENT_ITEM;

/**
 * @brief Structure for NC event ring.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint32_t write_index;
    uint32_t captured_count;
    uint32_t lost_count;
    uint32_t generation;
    NC_EVENT_ITEM items[NC_EVENT_RING_SIZE];
} NC_EVENT_RING;

/**
 * @brief Structure for NC binary program request.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC binary program status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC capability status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC interference status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for NC safety motion status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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


/**
 * @brief Structure for NC turning cycle status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t  active;
    uint8_t  diameter_mode_active;
    uint8_t  radius_mode_active;
    uint8_t  reserved0;
    uint32_t planned_cycle_blocks;
    uint32_t executed_cycle_blocks;
    uint32_t roughing_blocks;
    uint32_t finishing_blocks;
    uint32_t facing_blocks;
    uint32_t grooving_blocks;
    uint32_t pattern_repeat_blocks;
    uint32_t linear_taper_blocks;
    uint32_t end_face_blocks;
    uint32_t grinding_blocks;
    uint32_t last_cycle_code10;
    uint32_t last_line_no;
    int32_t  last_depth;
    int32_t  last_retract;
    int32_t  last_target[AXIS_MAX];
    uint32_t generation;
} NC_TURNING_CYCLE_STATUS;

/**
 * @brief Structure for NC thread cycle status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t  active;
    uint8_t  spindle_sync_required;
    uint8_t  reserved0;
    uint8_t  reserved1;
    uint32_t planned_thread_blocks;
    uint32_t executed_thread_blocks;
    uint32_t g32_blocks;
    uint32_t g76_blocks;
    uint32_t g767_blocks;
    uint32_t g78_blocks;
    uint32_t g92_blocks;
    uint32_t variable_lead_blocks;
    uint32_t circular_thread_blocks;
    uint32_t last_thread_code10;
    uint32_t last_line_no;
    int32_t  last_lead;
    int32_t  last_depth;
    int32_t  last_target[AXIS_MAX];
    uint32_t sync_loss_events;
    uint32_t generation;
} NC_THREAD_CYCLE_STATUS;

/**
 * @brief Structure for NC precision status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t  learning_enabled;
    uint8_t  vibration_suppression_enabled;
    uint8_t  preview_control_enabled;
    uint8_t  high_precision_contour_active;
    uint8_t  free_surface_mode_active;
    uint8_t  high_speed_cycle_active;
    uint8_t  learning_memory_window;
    uint8_t  reserved0;
    int32_t  learning_gain_percent;
    uint16_t vibration_notch_freq_hz;
    uint16_t vibration_damping_percent;
    uint16_t preview_lookahead_blocks;
    uint16_t preview_corner_tolerance;
    uint32_t active_mode_code10;
    uint32_t previewed_blocks;
    uint32_t preview_feed_limited_blocks;
    uint32_t advanced_precision_blocks;
    uint32_t hpcc_mode_blocks;
    uint32_t high_speed_cycle_blocks;
    uint32_t learning_updates;
    uint32_t vibration_filtered_samples;
    uint32_t vibration_warnings;
    uint32_t rt_service_ticks;
    uint32_t last_preview_line_no;
    uint32_t last_exec_line_no;
    uint16_t last_preview_override_percent;
    uint16_t reserved1;
    int32_t  last_path_delta;
    int32_t  last_following_error[AXIS_MAX];
    int32_t  max_following_error[AXIS_MAX];
    int32_t  last_learning_correction[AXIS_MAX];
    int32_t  last_vibration_delta[AXIS_MAX];
    uint32_t generation;
} NC_PRECISION_STATUS;

/**
 * @brief Structure for NC synchronization status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t  sync_enabled;
    uint8_t  compound_enabled;
    uint8_t  overlay_enabled;
    uint8_t  double_table_enabled;
    uint8_t  master_axis;
    uint8_t  reserved0;
    uint16_t active_mode_bits;
    uint32_t slave_axis_mask;
    uint32_t overlay_axis_mask;
    uint32_t compound_path_mask;
    uint32_t planned_mode_blocks;
    uint32_t executed_mode_blocks;
    uint32_t synchronized_motion_blocks;
    uint32_t compound_motion_blocks;
    uint32_t overlay_motion_blocks;
    uint32_t double_table_blocks;
    uint32_t mode_switch_events;
    uint32_t mode_cancel_events;
    uint32_t following_warn_events;
    uint32_t last_mode_code10;
    uint32_t last_line_no;
    int32_t  overlay_offset[AXIS_MAX];
    int32_t  last_master_delta;
    int32_t  last_following_error[AXIS_MAX];
    int32_t  max_following_error[AXIS_MAX];
    int32_t  last_output_target[AXIS_MAX];
    uint32_t generation;
} NC_SYNCHRONIZATION_STATUS;

/**
 * @brief Structure for NC rotary MCC status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t  enabled;
    uint8_t  mcc_output_enabled;
    uint8_t  mcc_output_active;
    uint8_t  active_rotary_axis;
    uint32_t configured_axis_mask;
    uint32_t active_axis_mask;
    uint32_t parsed_virtual_speed_blocks;
    uint32_t executed_virtual_speed_blocks;
    uint32_t mcc_on_events;
    uint32_t mcc_off_events;
    uint32_t radius_set_calls;
    uint32_t rejected_calls;
    uint32_t last_g_code10;
    uint32_t last_line_no;
    uint32_t last_mcc_command_bits;
    int32_t  radius[AXIS_MAX];
    int32_t  last_rotary_delta;
    int32_t  last_virtual_linear_delta;
    int32_t  last_axis_target[AXIS_MAX];
    uint32_t generation;
} NC_ROTARY_MCC_STATUS;

/**
 * @brief Structure for NC axis config status.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint8_t  axis_type[AXIS_MAX];
    uint8_t  axis_name[AXIS_MAX];
    uint8_t  coordinate_group[AXIS_MAX];
    uint8_t  reserved0[AXIS_MAX];
    uint32_t linear_axis_mask;
    uint32_t rotary_axis_mask;
    uint32_t diameter_axis_mask;
    uint32_t active_path_axis_mask;
    uint32_t detached_axis_mask;
    uint32_t mirror_axis_mask;
    uint32_t axis_definition_calls;
    uint32_t path_assignment_calls;
    uint32_t detached_set_calls;
    uint32_t diameter_mode_calls;
    uint32_t applied_blocks;
    uint32_t diameter_converted_blocks;
    uint32_t detached_axis_blocks;
    uint32_t path_filtered_blocks;
    uint32_t rejected_calls;
    uint32_t last_line_no;
    uint32_t last_input_axis_mask;
    uint32_t last_output_axis_mask;
    int32_t  last_input_target[AXIS_MAX];
    int32_t  last_output_target[AXIS_MAX];
    uint32_t generation;
} NC_AXIS_CONFIG_STATUS;


/**
 * @brief Status for design-document feature completions added in v20.
 */
/**
 * @brief Status for the v22-v32 implementation backlog batches.
 * @details Tracks 250 coded design-feature entries using fixed-size masks,
 * counters, and last-block snapshots so RT, TS, and API callers can observe the
 * added implementation without dynamic allocation.
 */
typedef struct {
    uint32_t implemented_mask[NC_IMPL_BACKLOG_WORD_COUNT];
    uint32_t configured_mask[NC_IMPL_BACKLOG_WORD_COUNT];
    uint32_t active_category_mask;
    uint32_t feature_hits[NC_IMPL_BACKLOG_FEATURE_COUNT];
    uint32_t category_hits[10];
    uint32_t implemented_count;
    uint32_t configured_count;
    uint32_t ts_adjusted_blocks;
    uint32_t rt_executed_blocks;
    uint32_t api_configured_calls;
    uint32_t synthetic_validation_blocks;
    uint32_t slowdown_applied_blocks;
    uint32_t last_feature_id;
    uint32_t last_category;
    uint32_t last_line_no;
    uint32_t last_g_code10;
    uint32_t last_policy;
    uint32_t last_axis_mask;
    int32_t  last_effective_feed;
    uint16_t active_override_percent;
    uint16_t reserved0;
    int32_t  last_input_target[AXIS_MAX];
    int32_t  last_output_target[AXIS_MAX];
    int32_t  category_accumulator[10];
    uint32_t generation;
} NC_IMPLEMENTATION_BACKLOG_STATUS;

typedef struct {
    uint8_t cylindrical_active;
    uint8_t polar_active;
    uint8_t auto_corner_active;
    uint8_t egb_skip_armed;
    uint32_t active_mode_mask;
    uint32_t nurbs_blocks;
    uint32_t exponential_blocks;
    uint32_t cylindrical_blocks;
    uint32_t polar_blocks;
    uint32_t helical_b_blocks;
    uint32_t auto_corner_override_blocks;
    uint32_t grinding_cycle_blocks;
    uint32_t egb_skip_blocks;
    uint32_t three_d_interference_blocks;
    uint32_t followup_rollover_blocks;
    uint32_t involute_blocks;
    uint32_t high_precision_contour_blocks;
    uint32_t exact_stop_blocks;
    uint32_t rigid_tap_blocks;
    uint32_t thread_chamfer_blocks;
    uint32_t second_reference_blocks;
    uint32_t machine_direct_blocks;
    uint32_t coordinate_shift_blocks;
    uint32_t constant_surface_speed_blocks;
    uint32_t feed_per_rev_blocks;
    uint32_t ts_adjusted_blocks;
    uint32_t rt_executed_blocks;
    uint32_t slowdown_applied_blocks;
    uint16_t active_override_percent;
    uint16_t reserved0;
    uint32_t last_g_code10;
    uint32_t last_line_no;
    uint32_t last_axis_mask;
    int32_t last_effective_feed;
    uint32_t last_spindle_speed;
    int32_t last_rigid_tap_pitch;
    int32_t last_css_diameter;
    int32_t last_input_target[AXIS_MAX];
    int32_t last_output_target[AXIS_MAX];
    int32_t last_clearance[AXIS_MAX];
    int32_t last_following_error[AXIS_MAX];
    uint32_t generation;
} NC_DESIGN_FEATURE_STATUS;

/**
 * @brief Structure for IO trace item.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
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

/**
 * @brief Structure for IO trace buffer.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint32_t write_index;
    uint32_t captured_count;
    uint8_t  frozen;
    uint32_t trigger_cycle;
    uint32_t trigger_alarm_code;
    uint32_t generation;
    IO_TRACE_ITEM items[IO_TRACE_RING_SIZE];
} IO_TRACE_BUFFER;

/**
 * @brief Global shared state variable g_ioIn.
 * @details Exposes the IO_INPUT_AREA storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile IO_INPUT_AREA      g_ioIn;
/**
 * @brief Global shared state variable g_ioOut.
 * @details Exposes the IO_OUTPUT_AREA storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile IO_OUTPUT_AREA     g_ioOut;
/**
 * @brief Global shared state variable g_machineCtx.
 * @details Exposes the MACHINE_CONTEXT storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile MACHINE_CONTEXT    g_machineCtx;
/**
 * @brief Global shared state variable g_uiRequest.
 * @details Exposes the UI_REQUEST_AREA storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile UI_REQUEST_AREA    g_uiRequest;
/**
 * @brief Global shared state variable g_logQueue.
 * @details Exposes the LOG_QUEUE storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile LOG_QUEUE          g_logQueue;
/**
 * @brief Global shared state variable g_prefetchReq.
 * @details Exposes the PREFETCH_REQ_CTRL storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile PREFETCH_REQ_CTRL  g_prefetchReq;
/**
 * @brief Global shared state variable g_prefetchResult.
 * @details Exposes the PREFETCH_RESULT storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile PREFETCH_RESULT    g_prefetchResult;
/**
 * @brief Global shared state variable g_ncProgramRequest.
 * @details Exposes the NC_PROGRAM_REQUEST storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_PROGRAM_REQUEST g_ncProgramRequest;
/**
 * @brief Global shared state variable g_ncProgramStatus.
 * @details Exposes the NC_PROGRAM_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_PROGRAM_STATUS  g_ncProgramStatus;
/**
 * @brief Global shared state variable g_ncProgramBuffer.
 * @details Exposes the NC_EXEC_BUFFER storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_EXEC_BUFFER     g_ncProgramBuffer;
/**
 * @brief Global shared state variable g_ncCoordinateState.
 * @details Exposes the NC_COORDINATE_STATE storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_COORDINATE_STATE g_ncCoordinateState;
/**
 * @brief Global shared state variable g_ncCoordinateTransformStatus.
 * @details Exposes the NC_COORDINATE_TRANSFORM_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_COORDINATE_TRANSFORM_STATUS g_ncCoordinateTransformStatus;
/**
 * @brief Global shared state variable g_ncInterpStatus.
 * @details Exposes the NC_INTERP_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_INTERP_STATUS   g_ncInterpStatus;
/**
 * @brief Global shared state variable g_ncFeedStatus.
 * @details Exposes the NC_FEED_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_FEED_STATUS     g_ncFeedStatus;
/**
 * @brief Global shared state variable g_ncFeatureStatus.
 * @details Exposes the NC_FEATURE_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_FEATURE_STATUS  g_ncFeatureStatus;
/**
 * @brief Global shared state variable g_prestartInterlockStatus.
 * @details Exposes the PRESTART_INTERLOCK_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile PRESTART_INTERLOCK_STATUS g_prestartInterlockStatus;
/**
 * @brief Global shared state variable g_ncAuxStatus.
 * @details Exposes the NC_AUX_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_AUX_STATUS      g_ncAuxStatus;
/**
 * @brief Global shared state variable g_ncCycleStatus.
 * @details Exposes the NC_CYCLE_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_CYCLE_STATUS    g_ncCycleStatus;
/**
 * @brief Global shared state variable g_ncCompensationStatus.
 * @details Exposes the NC_COMPENSATION_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_COMPENSATION_STATUS g_ncCompensationStatus;
/**
 * @brief Global shared state variable g_ncPathControlStatus.
 * @details Exposes the NC_PATH_CONTROL_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_PATH_CONTROL_STATUS g_ncPathControlStatus;
/**
 * @brief Global shared state variable g_ncKinematicsStatus.
 * @details Exposes the NC_KINEMATICS_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_KINEMATICS_STATUS g_ncKinematicsStatus;
/**
 * @brief Global shared state variable g_ncAxisLoadStatus.
 * @details Exposes the NC_AXIS_LOAD_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_AXIS_LOAD_STATUS  g_ncAxisLoadStatus;
/**
 * @brief Global shared state variable g_ncToolLifeStatus.
 * @details Exposes the NC_TOOL_LIFE_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_TOOL_LIFE_STATUS  g_ncToolLifeStatus;
/**
 * @brief Global shared state variable g_ncDiagnosticSnapshot.
 * @details Exposes the NC_DIAGNOSTIC_SNAPSHOT storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_DIAGNOSTIC_SNAPSHOT g_ncDiagnosticSnapshot;
/**
 * @brief Global shared state variable g_ncMotionFilterStatus.
 * @details Exposes the NC_MOTION_FILTER_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_MOTION_FILTER_STATUS g_ncMotionFilterStatus;
/**
 * @brief Global shared state variable g_ncEventRing.
 * @details Exposes the NC_EVENT_RING storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_EVENT_RING    g_ncEventRing;
/**
 * @brief Global shared state variable g_ncBinaryProgramRequest.
 * @details Exposes the NC_BINARY_PROGRAM_REQUEST storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_BINARY_PROGRAM_REQUEST g_ncBinaryProgramRequest;
/**
 * @brief Global shared state variable g_ncBinaryProgramStatus.
 * @details Exposes the NC_BINARY_PROGRAM_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_BINARY_PROGRAM_STATUS  g_ncBinaryProgramStatus;
/**
 * @brief Global shared state variable g_ncCapabilityStatus.
 * @details Exposes the NC_CAPABILITY_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_CAPABILITY_STATUS      g_ncCapabilityStatus;
/**
 * @brief Global shared state variable g_ncInterferenceStatus.
 * @details Exposes the NC_INTERFERENCE_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_INTERFERENCE_STATUS    g_ncInterferenceStatus;
/**
 * @brief Global shared state variable g_ncSafetyMotionStatus.
 * @details Exposes the NC_SAFETY_MOTION_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_SAFETY_MOTION_STATUS   g_ncSafetyMotionStatus;
/**
 * @brief Global shared state variable g_ncTurningCycleStatus.
 * @details Exposes the NC_TURNING_CYCLE_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_TURNING_CYCLE_STATUS   g_ncTurningCycleStatus;
/**
 * @brief Global shared state variable g_ncThreadCycleStatus.
 * @details Exposes the NC_THREAD_CYCLE_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_THREAD_CYCLE_STATUS    g_ncThreadCycleStatus;
/**
 * @brief Global shared state variable g_ncReferenceStatus.
 * @details Exposes the NC_REFERENCE_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_REFERENCE_STATUS       g_ncReferenceStatus;
/**
 * @brief Global shared state variable g_ncPrecisionStatus.
 * @details Exposes the NC_PRECISION_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_PRECISION_STATUS        g_ncPrecisionStatus;
/**
 * @brief Global shared state variable g_ncSpindleStatus.
 * @details Exposes the NC_SPINDLE_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_SPINDLE_STATUS          g_ncSpindleStatus;
/**
 * @brief Global shared state variable g_ncToolManagementStatus.
 * @details Exposes the NC_TOOL_MANAGEMENT_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_TOOL_MANAGEMENT_STATUS  g_ncToolManagementStatus;
/**
 * @brief Global shared state variable g_ncSynchronizationStatus.
 * @details Exposes the NC_SYNCHRONIZATION_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_SYNCHRONIZATION_STATUS g_ncSynchronizationStatus;
/**
 * @brief Global shared state variable g_ncRotaryMccStatus.
 * @details Exposes the NC_ROTARY_MCC_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_ROTARY_MCC_STATUS    g_ncRotaryMccStatus;
/**
 * @brief Global shared state variable g_ncAxisConfigStatus.
 * @details Exposes the NC_AXIS_CONFIG_STATUS storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile NC_AXIS_CONFIG_STATUS    g_ncAxisConfigStatus;
extern volatile NC_IMPLEMENTATION_BACKLOG_STATUS g_ncImplementationBacklogStatus;
extern volatile NC_DESIGN_FEATURE_STATUS g_ncDesignFeatureStatus;
/**
 * @brief Global shared state variable g_ioTraceBuffer.
 * @details Exposes the IO_TRACE_BUFFER storage used by RT, TS, API, and smoke-test code. Access patterns must stay simple and deterministic.
 */
extern volatile IO_TRACE_BUFFER    g_ioTraceBuffer;

/**
 * @brief Handle system shared initialize for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void SystemShared_Initialize(void);
/**
 * @brief Push log queue into the fixed-size ring or queue.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param eventType Input value for event type.
 * @param code Code value being applied or recorded.
 * @param value Numeric value used by this operation.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t LogQueue_Push(uint32_t eventType, uint32_t code, int32_t value);
/**
 * @brief Push NC event into the fixed-size ring or queue.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param code Code value being applied or recorded.
 * @param value0 Input value for value 0.
 * @param value1 Input value for value 1.
 * @param lineNo NC source line number associated with the operation.
 */
void NcEvent_Push(uint32_t code, int32_t value0, int32_t value1, uint32_t lineNo);

#endif /* SYSTEM_SHARED_H */
