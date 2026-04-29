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
    NC_ERR_UNIT           = 14
} NC_ERROR_CODE;

typedef enum {
    NC_MOTION_NONE      = 0,
    NC_MOTION_RAPID     = 1,
    NC_MOTION_LINEAR    = 2,
    NC_MOTION_AUX_ONLY  = 3,
    NC_MOTION_ARC_CW    = 4,
    NC_MOTION_ARC_CCW   = 5,
    NC_MOTION_DWELL     = 6
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

void SystemShared_Initialize(void);
int32_t LogQueue_Push(uint32_t eventType, uint32_t code, int32_t value);

#endif /* SYSTEM_SHARED_H */
