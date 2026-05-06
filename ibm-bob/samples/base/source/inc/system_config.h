/* ibm-bob/samples/base/source/inc/system_config.h */
/* Defines shared build-time limits for the IBM BOB baseline sample. */
/* This exists so RT, TS, API, and NC modules use the same fixed sizes. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/inc/nc_program.h, ibm-bob/samples/base/source/src/nc_program.c */
#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

/**
 * @brief Definition value for axis max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define AXIS_MAX                (4)
/**
 * @brief Definition value for analog ch max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define ANALOG_CH_MAX           (8)
/**
 * @brief Definition value for log queue max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define LOG_QUEUE_MAX           (64)
/**
 * @brief Definition value for prefetch table size.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define PREFETCH_TABLE_SIZE     (64)
/**
 * @brief Definition value for UI message text max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define UI_MESSAGE_TEXT_MAX     (96)

/**
 * @brief Definition value for cycle tick MS.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define CYCLE_TICK_MS           (1)
/**
 * @brief Definition value for safe pressure limit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define SAFE_PRESSURE_LIMIT     (900)
/**
 * @brief Definition value for ready pressure min.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define READY_PRESSURE_MIN      (100)
/**
 * @brief Definition value for ready pressure max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define READY_PRESSURE_MAX      (700)

/**
 * @brief Definition value for NC max lookahead lines.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MAX_LOOKAHEAD_LINES      (10000U)
/**
 * @brief Definition value for NC file path max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FILE_PATH_MAX            (260U)
/**
 * @brief Definition value for NC line max len.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_LINE_MAX_LEN             (256U)
/**
 * @brief Definition value for NC RT max blocks per cycle.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_RT_MAX_BLOCKS_PER_CYCLE  (8U)
/**
 * @brief Definition value for NC TS parse lines per slice.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TS_PARSE_LINES_PER_SLICE (100U)
/**
 * @brief Definition value for NC position scale.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_POSITION_SCALE           (10000)
/**
 * @brief Definition value for NC interp min ticks.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_INTERP_MIN_TICKS         (1U)
/**
 * @brief Definition value for NC interp max ticks.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_INTERP_MAX_TICKS         (100000U)
/**
 * @brief Definition value for NC rapid feed units per min.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_RAPID_FEED_UNITS_PER_MIN (60000)
/**
 * @brief Definition value for NC arc tolerance.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ARC_TOLERANCE            (10)
/**
 * @brief Definition value for NC feed override min percent.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEED_OVERRIDE_MIN_PERCENT     (0U)
/**
 * @brief Definition value for NC feed override max percent.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEED_OVERRIDE_MAX_PERCENT     (200U)
/**
 * @brief Definition value for NC feed override default percent.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEED_OVERRIDE_DEFAULT_PERCENT (100U)
/**
 * @brief Definition value for NC feed accel ticks default.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEED_ACCEL_TICKS_DEFAULT      (2U)
/**
 * @brief Definition value for NC dwell max ticks.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_DWELL_MAX_TICKS               (100000U)
/**
 * @brief Definition value for NC work coord systems.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_WORK_COORD_SYSTEMS            (6U)
/**
 * @brief Definition value for NC g10 l work offset.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_G10_L_WORK_OFFSET             (2)

/**
 * @brief Definition value for IO trace ring size.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define IO_TRACE_RING_SIZE                (32U)
/**
 * @brief Definition value for sensor seq MFIN bit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define SENSOR_SEQ_MFIN_BIT               (0x00000001UL)
/**
 * @brief Definition value for sensor seq skip bit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define SENSOR_SEQ_SKIP_BIT               (0x00000002UL)
/**
 * @brief Definition value for NC canned max segments.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CANNED_MAX_SEGMENTS            (512U)
/**
 * @brief Definition value for NC cycle repeat max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CYCLE_REPEAT_MAX               (9999U)
/**
 * @brief Definition value for NC cycle chip break retract.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CYCLE_CHIP_BREAK_RETRACT        (1000)
/**
 * @brief Definition value for NC tap min spindle RPM.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TAP_MIN_SPINDLE_RPM             (1U)
/**
 * @brief Definition value for NC tool offset table size.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_OFFSET_TABLE_SIZE          (32U)
/**
 * @brief Definition value for NC arc segment max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ARC_SEGMENT_MAX                 (64U)
/**
 * @brief Definition value for NC lookahead corner slow percent.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_LOOKAHEAD_CORNER_SLOW_PERCENT   (60U)
/**
 * @brief Definition value for NC lookahead corner warn deg.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_LOOKAHEAD_CORNER_WARN_DEG        (35U)
/**
 * @brief Definition value for NC g08 standard tolerance.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_G08_STANDARD_TOLERANCE           (100)
/**
 * @brief Definition value for NC g08 high precision tolerance.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_G08_HIGH_PRECISION_TOLERANCE     (20)
/**
 * @brief Definition value for NC HPCC min lookahead blocks.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_HPCC_MIN_LOOKAHEAD_BLOCKS        (3U)
/**
 * @brief Definition value for NC HPCC corner tolerance.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_HPCC_CORNER_TOLERANCE            (50)
/**
 * @brief Definition value for NC HPCC feed override percent.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_HPCC_FEED_OVERRIDE_PERCENT       (80U)
/**
 * @brief Definition value for NC angle scale.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ANGLE_SCALE                     (10000)
/**
 * @brief Definition value for NC axis load warn threshold.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AXIS_LOAD_WARN_THRESHOLD         (600)
/**
 * @brief Definition value for NC axis load limit threshold.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AXIS_LOAD_LIMIT_THRESHOLD        (800)
/**
 * @brief Definition value for NC axis load critical threshold.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AXIS_LOAD_CRITICAL_THRESHOLD     (920)
/**
 * @brief Definition value for NC axis load filter div.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AXIS_LOAD_FILTER_DIV             (4)
/**
 * @brief Definition value for NC tool life table size.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_LIFE_TABLE_SIZE             (32U)
/**
 * @brief Definition value for NC tool life default limit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_LIFE_DEFAULT_LIMIT          (1000U)
/**
 * @brief Definition value for NC tool life warning percent.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_LIFE_WARNING_PERCENT        (80U)

/**
 * @brief Definition value for NC motion filter velocity window max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MOTION_FILTER_VELOCITY_WINDOW_MAX (8U)
/**
 * @brief Definition value for NC motion filter accel window max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MOTION_FILTER_ACCEL_WINDOW_MAX    (8U)
/**
 * @brief Definition value for NC motion filter FIR taps.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MOTION_FILTER_FIR_TAPS            (5U)
/**
 * @brief Definition value for NC motion filter default velocity window.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MOTION_FILTER_DEFAULT_VELOCITY_WINDOW (3U)
/**
 * @brief Definition value for NC motion filter default accel window.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MOTION_FILTER_DEFAULT_ACCEL_WINDOW    (5U)
/**
 * @brief Definition value for NC event ring size.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_RING_SIZE                   (64U)
/**
 * @brief Definition value for NC motion filter axis limit max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MOTION_FILTER_AXIS_LIMIT_MAX       (1000000)
/**
 * @brief Definition value for NC motion filter default max velocity.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MOTION_FILTER_DEFAULT_MAX_VELOCITY (800)
/**
 * @brief Definition value for NC motion filter default max accel.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MOTION_FILTER_DEFAULT_MAX_ACCEL    (240)
/**
 * @brief Definition value for NC motion filter endpoint window.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MOTION_FILTER_ENDPOINT_WINDOW      (4U)
/**
 * @brief Definition value for NC motion filter endpoint tolerance.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MOTION_FILTER_ENDPOINT_TOLERANCE   (2)

/**
 * @brief Definition value for NC binary program max blocks.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_BINARY_PROGRAM_MAX_BLOCKS        (256U)
/**
 * @brief Definition value for NC capability G history size.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CAPABILITY_G_HISTORY_SIZE        (32U)
/**
 * @brief Definition value for NC capability M history size.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CAPABILITY_M_HISTORY_SIZE        (16U)
/**
 * @brief Definition value for NC interference axis limit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_INTERFERENCE_AXIS_LIMIT          (2000000)
/**
 * @brief Definition value for NC interference min clearance.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_INTERFERENCE_MIN_CLEARANCE       (500)
/**
 * @brief Definition value for NC interference slowdown percent.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_INTERFERENCE_SLOWDOWN_PERCENT    (50U)
/**
 * @brief Definition value for NC safety external decel percent.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SAFETY_EXTERNAL_DECEL_PERCENT    (35U)
/**
 * @brief Definition value for NC safety powerfail decel percent.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SAFETY_POWERFAIL_DECEL_PERCENT   (10U)
/**
 * @brief Definition value for sensor power fail bit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define SENSOR_POWER_FAIL_BIT               (0x00000100UL)
/**
 * @brief Definition value for sensor external decel bit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define SENSOR_EXTERNAL_DECEL_BIT           (0x00000200UL)
/**
 * @brief Definition value for sensor position switch bit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define SENSOR_POSITION_SWITCH_BIT          (0x00000400UL)
/**
 * @brief Definition value for sensor absolute ref valid bit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define SENSOR_ABSOLUTE_REF_VALID_BIT       (0x00000800UL)
/**
 * @brief Definition value for NC event code binary load ready.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_BINARY_LOAD_READY      (8401U)
/**
 * @brief Definition value for NC event code capability unsupported.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_CAPABILITY_UNSUPPORTED (8501U)
/**
 * @brief Definition value for NC event code interference warn.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_INTERFERENCE_WARN      (8601U)
/**
 * @brief Definition value for NC event code external decel.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_EXTERNAL_DECEL         (8701U)
/**
 * @brief Definition value for NC event code powerfail decel stop.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_POWERFAIL_DECEL_STOP   (8702U)
/**
 * @brief Definition value for NC event code position switch latch.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_POSITION_SWITCH_LATCH  (8703U)
/**
 * @brief Definition value for NC event code lathe cycle executed.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_LATHE_CYCLE_EXECUTED    (8801U)
/**
 * @brief Definition value for NC event code thread cycle executed.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_THREAD_CYCLE_EXECUTED   (8802U)

/**
 * @brief Definition value for sensor reference mark base bit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define SENSOR_REFERENCE_MARK_BASE_BIT     (0x00001000UL)
/**
 * @brief Definition value for NC reference default rollover limit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_REFERENCE_DEFAULT_ROLLOVER_LIMIT (360000000)
/**
 * @brief Definition value for NC one direction approach default.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ONE_DIRECTION_APPROACH_DEFAULT   (1000)
/**
 * @brief Definition value for NC event code reference return.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_REFERENCE_RETURN       (8901U)
/**
 * @brief Definition value for NC event code reference mark capture.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_REFERENCE_MARK_CAPTURE (8902U)
/**
 * @brief Definition value for NC event code reference rollover.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_REFERENCE_ROLLOVER     (8903U)
/**
 * @brief Definition value for NC event code one direction positioning.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_ONE_DIRECTION_POSITIONING (8904U)

/**
 * @brief Definition value for NC precision learning memory max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_LEARNING_MEMORY_MAX       (8U)
/**
 * @brief Definition value for NC precision learning memory default.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_LEARNING_MEMORY_DEFAULT   (4U)
/**
 * @brief Definition value for NC precision learning gain default.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_LEARNING_GAIN_DEFAULT     (25)
/**
 * @brief Definition value for NC precision learning gain max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_LEARNING_GAIN_MAX         (200)
/**
 * @brief Definition value for NC precision vibration notch default hz.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_VIBRATION_NOTCH_DEFAULT_HZ (120U)
/**
 * @brief Definition value for NC precision vibration freq max hz.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_VIBRATION_FREQ_MAX_HZ     (2000U)
/**
 * @brief Definition value for NC precision vibration damping default.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_VIBRATION_DAMPING_DEFAULT (30U)
/**
 * @brief Definition value for NC precision vibration damping max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_VIBRATION_DAMPING_MAX     (100U)
/**
 * @brief Definition value for NC precision preview lookahead default.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_PREVIEW_LOOKAHEAD_DEFAULT (16U)
/**
 * @brief Definition value for NC precision preview lookahead max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_PREVIEW_LOOKAHEAD_MAX     (256U)
/**
 * @brief Definition value for NC precision preview corner tolerance default.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_PREVIEW_CORNER_TOLERANCE_DEFAULT (50U)
/**
 * @brief Definition value for NC precision preview corner tolerance max.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_PREVIEW_CORNER_TOLERANCE_MAX     (10000U)
/**
 * @brief Definition value for NC precision axis target limit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_AXIS_TARGET_LIMIT         (2000000000)
/**
 * @brief Definition value for NC precision vibration warn delta.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_PRECISION_VIBRATION_WARN_DELTA      (50000)
/**
 * @brief Definition value for NC event code precision vibration warn.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_PRECISION_VIBRATION_WARN (9001U)
/**
 * @brief Definition value for NC event code precision learning update.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_PRECISION_LEARNING_UPDATE (9002U)
/**
 * @brief Definition value for NC event code precision preview limit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_PRECISION_PREVIEW_LIMIT  (9003U)

/**
 * @brief Definition value for NC spindle default max RPM.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SPINDLE_DEFAULT_MAX_RPM        (12000U)
/**
 * @brief Definition value for NC spindle default min RPM.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SPINDLE_DEFAULT_MIN_RPM        (30U)
/**
 * @brief Definition value for NC spindle absolute max RPM.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SPINDLE_ABSOLUTE_MAX_RPM       (60000U)
/**
 * @brief Definition value for NC spindle CSS min diameter.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SPINDLE_CSS_MIN_DIAMETER       (NC_POSITION_SCALE)
/**
 * @brief Definition value for NC spindle orient timeout ticks.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SPINDLE_ORIENT_TIMEOUT_TICKS   (2000U)
/**
 * @brief Definition value for sensor spindle orient done bit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define SENSOR_SPINDLE_ORIENT_DONE_BIT    (0x00010000UL)
/**
 * @brief Definition value for NC event code spindle start.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_SPINDLE_START       (9101U)
/**
 * @brief Definition value for NC event code spindle stop.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_SPINDLE_STOP        (9102U)
/**
 * @brief Definition value for NC event code spindle orient.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_SPINDLE_ORIENT      (9103U)
/**
 * @brief Definition value for NC event code spindle orient done.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_SPINDLE_ORIENT_DONE (9104U)
/**
 * @brief Definition value for NC event code spindle orient timeout.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_SPINDLE_ORIENT_TIMEOUT (9105U)

/**
 * @brief Definition value for NC tool table size.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_TABLE_SIZE                (64U)
/**
 * @brief Definition value for NC tool magazine pockets.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_MAGAZINE_POCKETS          (32U)
/**
 * @brief Definition value for NC tool change timeout ticks.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_TOOL_CHANGE_TIMEOUT_TICKS      (3000U)
/**
 * @brief Definition value for NC event code tool prepare.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_TOOL_PREPARE        (9201U)
/**
 * @brief Definition value for NC event code tool change request.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_TOOL_CHANGE_REQUEST (9202U)
/**
 * @brief Definition value for NC event code tool change done.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_TOOL_CHANGE_DONE    (9203U)
/**
 * @brief Definition value for NC event code tool management warn.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_TOOL_MANAGEMENT_WARN (9204U)

/**
 * @brief Definition value for NC sync default master axis.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SYNC_DEFAULT_MASTER_AXIS        (0U)
/**
 * @brief Definition value for NC sync default slave mask.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SYNC_DEFAULT_SLAVE_MASK         (0x00000002UL)
/**
 * @brief Definition value for NC sync following warn threshold.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SYNC_FOLLOWING_WARN_THRESHOLD   (2000)
/**
 * @brief Definition value for NC event code sync mode change.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_SYNC_MODE_CHANGE     (9301U)
/**
 * @brief Definition value for NC event code sync motion applied.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_SYNC_MOTION_APPLIED  (9302U)
/**
 * @brief Definition value for NC event code sync loss warn.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_SYNC_LOSS_WARN       (9303U)
/**
 * @brief Definition value for NC event code overlay applied.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_OVERLAY_APPLIED      (9304U)

/**
 * @brief Definition value for NC rotary MCC default radius.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ROTARY_MCC_DEFAULT_RADIUS        (50000)
/**
 * @brief Definition value for NC rotary MCC min radius.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ROTARY_MCC_MIN_RADIUS            (100)
/**
 * @brief Definition value for NC rotary MCC max radius.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ROTARY_MCC_MAX_RADIUS            (100000000)
/**
 * @brief Definition value for NC rotary MCC command bit.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ROTARY_MCC_COMMAND_BIT           (0x00000010UL)
/**
 * @brief Definition value for NC rotary MCC default axis mask.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ROTARY_MCC_DEFAULT_AXIS_MASK     (0x00000008UL)
/**
 * @brief Definition value for NC event code rotary MCC mode.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_ROTARY_MCC_MODE       (9401U)
/**
 * @brief Definition value for NC event code rotary MCC applied.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_ROTARY_MCC_APPLIED    (9402U)
/**
 * @brief Definition value for NC event code rotary MCC output.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_ROTARY_MCC_OUTPUT     (9403U)

/**
 * @brief Definition value for NC axis type linear.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AXIS_TYPE_LINEAR             (0U)
/**
 * @brief Definition value for NC axis type rotary.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AXIS_TYPE_ROTARY             (1U)
/**
 * @brief Definition value for NC axis config default path mask.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AXIS_CONFIG_DEFAULT_PATH_MASK ((1UL << AXIS_MAX) - 1UL)
/**
 * @brief Definition value for NC event code axis config change.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_AXIS_CONFIG_CHANGE (9501U)
/**
 * @brief Definition value for NC event code axis config applied.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_AXIS_CONFIG_APPLIED (9502U)
/**
 * @brief Definition value for NC event code axis detached.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_AXIS_DETACHED      (9503U)
/**
 * @brief Definition value for NC event code axis diameter mode.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_EVENT_CODE_AXIS_DIAMETER_MODE (9504U)

/* Design-document feature completion defaults (v20). */
#define NC_DESIGN_AUTO_CORNER_OVERRIDE_PERCENT (65U)
#define NC_DESIGN_ADVANCED_MIN_TICKS           (4U)
#define NC_DESIGN_MODE_NURBS                  (0x00000001UL)
#define NC_DESIGN_MODE_EXPONENTIAL            (0x00000002UL)
#define NC_DESIGN_MODE_CYLINDRICAL            (0x00000004UL)
#define NC_DESIGN_MODE_POLAR                  (0x00000008UL)
#define NC_DESIGN_MODE_HELICAL_B              (0x00000010UL)
#define NC_DESIGN_MODE_AUTO_CORNER            (0x00000020UL)
#define NC_DESIGN_MODE_GRINDING               (0x00000040UL)
#define NC_DESIGN_MODE_EGB_SKIP               (0x00000080UL)
#define NC_DESIGN_MODE_3D_INTERFERENCE        (0x00000100UL)
#define NC_DESIGN_MODE_FOLLOWUP_ROLLOVER      (0x00000200UL)
/* Additional design-document feature completion bits (v21). */
#define NC_DESIGN_MODE_INVOLUTE               (0x00000400UL)
#define NC_DESIGN_MODE_HP_CONTOUR             (0x00000800UL)
#define NC_DESIGN_MODE_EXACT_STOP             (0x00001000UL)
#define NC_DESIGN_MODE_RIGID_TAP              (0x00002000UL)
#define NC_DESIGN_MODE_THREAD_CHAMFER         (0x00004000UL)
#define NC_DESIGN_MODE_SECOND_REFERENCE       (0x00008000UL)
#define NC_DESIGN_MODE_MACHINE_DIRECT         (0x00010000UL)
#define NC_DESIGN_MODE_COORD_SHIFT            (0x00020000UL)
#define NC_DESIGN_MODE_CSS_SPINDLE            (0x00040000UL)
#define NC_DESIGN_MODE_FEED_PER_REV           (0x00080000UL)
#define NC_DESIGN_INVOLUTE_MIN_TICKS          (6U)
#define NC_DESIGN_RIGID_TAP_MIN_TICKS         (8U)
#define NC_DESIGN_EXACT_STOP_PERCENT          (55U)
#define NC_DESIGN_HP_CONTOUR_PERCENT          (75U)
#define NC_IMPL_BACKLOG_FEATURE_COUNT          (260U)
#define NC_IMPL_BACKLOG_WORD_COUNT             (9U)
#define NC_IMPL_BACKLOG_DEFAULT_OVERRIDE       (92U)
#define NC_IMPL_BACKLOG_MIN_TICKS              (3U)
#define NC_IMPL_BACKLOG_MAX_POLICY             (7U)
#define NC_IMPL_BACKLOG_V23_FIRST_FEATURE       (100U)
#define NC_IMPL_BACKLOG_V23_ADDED_FEATURES      (20U)
#define NC_IMPL_BACKLOG_V24_FIRST_FEATURE       (120U)
#define NC_IMPL_BACKLOG_V24_ADDED_FEATURES      (20U)
#define NC_IMPL_BACKLOG_V25_FIRST_FEATURE       (140U)
#define NC_IMPL_BACKLOG_V25_ADDED_FEATURES      (20U)
#define NC_IMPL_BACKLOG_V26_FIRST_FEATURE       (160U)
#define NC_IMPL_BACKLOG_V26_ADDED_FEATURES      (20U)
#define NC_IMPL_BACKLOG_V27_FIRST_FEATURE       (180U)
#define NC_IMPL_BACKLOG_V27_ADDED_FEATURES      (20U)
#define NC_IMPL_BACKLOG_V28_FIRST_FEATURE       (200U)
#define NC_IMPL_BACKLOG_V28_ADDED_FEATURES      (10U)
#define NC_IMPL_BACKLOG_V29_FIRST_FEATURE       (210U)
#define NC_IMPL_BACKLOG_V29_ADDED_FEATURES      (10U)
#define NC_IMPL_BACKLOG_V30_FIRST_FEATURE       (220U)
#define NC_IMPL_BACKLOG_V30_ADDED_FEATURES      (10U)
#define NC_IMPL_BACKLOG_V31_FIRST_FEATURE       (230U)
#define NC_IMPL_BACKLOG_V31_ADDED_FEATURES      (10U)
#define NC_IMPL_BACKLOG_V32_FIRST_FEATURE       (240U)
#define NC_IMPL_BACKLOG_V32_ADDED_FEATURES      (10U)
#define NC_IMPL_BACKLOG_V29_FIRST_FEATURE       (210U)
#define NC_IMPL_BACKLOG_V29_ADDED_FEATURES      (10U)
#define NC_IMPL_BACKLOG_V30_FIRST_FEATURE       (220U)
#define NC_IMPL_BACKLOG_V30_ADDED_FEATURES      (10U)
#define NC_IMPL_BACKLOG_V31_FIRST_FEATURE       (230U)
#define NC_IMPL_BACKLOG_V31_ADDED_FEATURES      (10U)
#define NC_IMPL_BACKLOG_V23_ROTARY_DEG_180      (180000)
#define NC_IMPL_BACKLOG_V23_ROTARY_DEG_360      (360000)
#define NC_EVENT_CODE_DESIGN_FEATURE_APPLIED  (9601U)
#define NC_EVENT_CODE_DESIGN_FEATURE_WARNING  (9602U)

#endif /* SYSTEM_CONFIG_H */
