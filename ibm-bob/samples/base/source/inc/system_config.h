/* ibm-bob/samples/base/source/inc/system_config.h */
/* Defines shared build-time limits for the IBM BOB baseline sample. */
/* This exists so RT, TS, API, and NC modules use the same fixed sizes. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/inc/nc_program.h, ibm-bob/samples/base/source/src/nc_program.c */
#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#define AXIS_MAX                (4)
#define ANALOG_CH_MAX           (8)
#define LOG_QUEUE_MAX           (64)
#define PREFETCH_TABLE_SIZE     (64)
#define UI_MESSAGE_TEXT_MAX     (96)

#define CYCLE_TICK_MS           (1)
#define SAFE_PRESSURE_LIMIT     (900)
#define READY_PRESSURE_MIN      (100)
#define READY_PRESSURE_MAX      (700)

#define NC_MAX_LOOKAHEAD_LINES      (10000U)
#define NC_FILE_PATH_MAX            (260U)
#define NC_LINE_MAX_LEN             (256U)
#define NC_RT_MAX_BLOCKS_PER_CYCLE  (8U)
#define NC_TS_PARSE_LINES_PER_SLICE (100U)
#define NC_POSITION_SCALE           (10000)
#define NC_INTERP_MIN_TICKS         (1U)
#define NC_INTERP_MAX_TICKS         (100000U)
#define NC_RAPID_FEED_UNITS_PER_MIN (60000)
#define NC_ARC_TOLERANCE            (10)
#define NC_FEED_OVERRIDE_MIN_PERCENT     (0U)
#define NC_FEED_OVERRIDE_MAX_PERCENT     (200U)
#define NC_FEED_OVERRIDE_DEFAULT_PERCENT (100U)
#define NC_FEED_ACCEL_TICKS_DEFAULT      (2U)
#define NC_DWELL_MAX_TICKS               (100000U)
#define NC_WORK_COORD_SYSTEMS            (6U)
#define NC_G10_L_WORK_OFFSET             (2)

#endif /* SYSTEM_CONFIG_H */
