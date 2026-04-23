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

typedef struct {
    uint32_t sensor_bits;
    uint32_t seq_bits;
    int32_t  analog[ANALOG_CH_MAX];
} IO_INPUT_AREA;

typedef struct {
    uint32_t command_bits;
    int32_t  axis_target[AXIS_MAX];
    uint32_t lamp_bits;
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

extern volatile IO_INPUT_AREA      g_ioIn;
extern volatile IO_OUTPUT_AREA     g_ioOut;
extern volatile MACHINE_CONTEXT    g_machineCtx;
extern volatile UI_REQUEST_AREA    g_uiRequest;
extern volatile LOG_QUEUE          g_logQueue;
extern volatile PREFETCH_REQ_CTRL  g_prefetchReq;
extern volatile PREFETCH_RESULT    g_prefetchResult;

void SystemShared_Initialize(void);
int32_t LogQueue_Push(uint32_t eventType, uint32_t code, int32_t value);

#endif /* SYSTEM_SHARED_H */
