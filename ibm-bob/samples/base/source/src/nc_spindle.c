/* ibm-bob/samples/base/source/src/nc_spindle.c */
/* Implements spindle M/S/G96/G97 state tracking with fixed-size shared memory. */
/* This exists so spindle speed, direction, CSS, and orientation behavior are observable from API. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_spindle.h, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/src/rt_control.c */
#include <string.h>
#include "nc_spindle.h"
#include "nc_program.h"
#include "system_config.h"

#define G(whole)       NC_G_CODE_WHOLE(whole)
#define M_SPINDLE_CW   (3U)
#define M_SPINDLE_CCW  (4U)
#define M_SPINDLE_STOP (5U)
#define M_SPINDLE_ORIENT (19U)

static uint32_t s_orient_start_cycle;

static int32_t Abs32(int32_t value)
{
    return (value < 0) ? -value : value;
}

static uint32_t ClampRpm(uint32_t rpm)
{
    uint32_t clamped = rpm;

    if ((g_ncSpindleStatus.max_rpm != 0U) && (clamped > g_ncSpindleStatus.max_rpm)) {
        clamped = g_ncSpindleStatus.max_rpm;
        g_ncSpindleStatus.speed_clamp_events++;
        g_ncSpindleStatus.clamped = 1U;
    }
    if ((clamped != 0U) && (clamped < g_ncSpindleStatus.min_rpm)) {
        clamped = g_ncSpindleStatus.min_rpm;
        g_ncSpindleStatus.speed_clamp_events++;
        g_ncSpindleStatus.clamped = 1U;
    }
    return clamped;
}

static uint32_t EstimateCssRpm(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t surface = g_ncSpindleStatus.css_surface_speed;
    int32_t diameter = 0;
    uint64_t rpm;

    if (surface == 0U) {
        surface = pBlock->spindle_speed;
    }
    diameter = Abs32(pBlock->axis_target[0]);
    if (diameter < NC_SPINDLE_CSS_MIN_DIAMETER) {
        diameter = NC_SPINDLE_CSS_MIN_DIAMETER;
    }

    /* Simplified integer CSS estimate: rpm ~= surface_speed * scale * 1000 / diameter. */
    rpm = ((uint64_t)surface * (uint64_t)NC_POSITION_SCALE * 1000ULL) /
          (uint64_t)diameter;
    if (rpm > 0xffffffffULL) {
        rpm = 0xffffffffULL;
    }
    return (uint32_t)rpm;
}

void NcSpindle_Reset(void)
{
    (void)memset((void*)&g_ncSpindleStatus, 0, sizeof(g_ncSpindleStatus));
    g_ncSpindleStatus.max_rpm = NC_SPINDLE_DEFAULT_MAX_RPM;
    g_ncSpindleStatus.min_rpm = NC_SPINDLE_DEFAULT_MIN_RPM;
    g_ncSpindleStatus.orient_timeout_ticks = NC_SPINDLE_ORIENT_TIMEOUT_TICKS;
    s_orient_start_cycle = 0U;
    g_ncSpindleStatus.generation++;
}

int32_t NcSpindle_SetSpeedLimit(uint32_t minRpm, uint32_t maxRpm)
{
    if ((maxRpm == 0U) || (minRpm > maxRpm) ||
        (maxRpm > NC_SPINDLE_ABSOLUTE_MAX_RPM)) {
        return -1;
    }
    g_ncSpindleStatus.min_rpm = minRpm;
    g_ncSpindleStatus.max_rpm = maxRpm;
    g_ncSpindleStatus.generation++;
    return 0;
}

int32_t NcSpindle_SetOrientWindow(uint16_t timeoutTicks)
{
    if (timeoutTicks == 0U) {
        return -1;
    }
    g_ncSpindleStatus.orient_timeout_ticks = timeoutTicks;
    g_ncSpindleStatus.generation++;
    return 0;
}

void NcSpindle_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock)
{
    if (pBlock == 0) {
        return;
    }

    if (pBlock->g_code10 == (uint16_t)G(96)) {
        g_ncSpindleStatus.css_blocks++;
        g_ncSpindleStatus.css_mode_active = 1U;
        g_ncSpindleStatus.last_g_code10 = pBlock->g_code10;
    } else if (pBlock->g_code10 == (uint16_t)G(97)) {
        g_ncSpindleStatus.fixed_rpm_blocks++;
        g_ncSpindleStatus.css_mode_active = 0U;
        g_ncSpindleStatus.last_g_code10 = pBlock->g_code10;
    }

    if ((pBlock->aux_flags & NC_AUX_FLAG_SPINDLE) != 0U) {
        g_ncSpindleStatus.commanded_rpm = pBlock->spindle_speed;
        if (g_ncSpindleStatus.css_mode_active != 0U) {
            g_ncSpindleStatus.css_surface_speed = pBlock->spindle_speed;
        }
        g_ncSpindleStatus.s_word_blocks++;
        g_ncSpindleStatus.last_s_code = pBlock->spindle_speed;
        g_ncSpindleStatus.last_line_no = pBlock->line_no;
    }

    if ((pBlock->aux_flags & NC_AUX_FLAG_M_CODE) != 0U) {
        uint32_t m = pBlock->aux_m_code;
        if ((m == M_SPINDLE_CW) || (m == M_SPINDLE_CCW) ||
            (m == M_SPINDLE_STOP) || (m == M_SPINDLE_ORIENT)) {
            g_ncSpindleStatus.m_word_blocks++;
            g_ncSpindleStatus.last_m_code = m;
            g_ncSpindleStatus.last_line_no = pBlock->line_no;
        }
    }
    g_ncSpindleStatus.generation++;
}

void NcSpindle_OnBlockRt(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t rpm = g_ncSpindleStatus.commanded_rpm;

    if (pBlock == 0) {
        return;
    }

    if (pBlock->g_code10 == (uint16_t)G(96)) {
        g_ncSpindleStatus.css_mode_active = 1U;
        g_ncSpindleStatus.last_g_code10 = pBlock->g_code10;
    } else if (pBlock->g_code10 == (uint16_t)G(97)) {
        g_ncSpindleStatus.css_mode_active = 0U;
        g_ncSpindleStatus.last_g_code10 = pBlock->g_code10;
    }

    if ((pBlock->aux_flags & NC_AUX_FLAG_SPINDLE) != 0U) {
        g_ncSpindleStatus.commanded_rpm = pBlock->spindle_speed;
        g_ncSpindleStatus.last_s_code = pBlock->spindle_speed;
        if (g_ncSpindleStatus.css_mode_active != 0U) {
            g_ncSpindleStatus.css_surface_speed = pBlock->spindle_speed;
            rpm = EstimateCssRpm(pBlock);
        } else {
            rpm = pBlock->spindle_speed;
        }
        g_ncSpindleStatus.output_rpm = ClampRpm(rpm);
        g_ioOut.spindle_speed = g_ncSpindleStatus.output_rpm;
    }

    if ((pBlock->aux_flags & NC_AUX_FLAG_M_CODE) != 0U) {
        uint32_t m = pBlock->aux_m_code;
        g_ncSpindleStatus.last_m_code = m;
        if (m == M_SPINDLE_CW) {
            if (g_ncSpindleStatus.direction != NC_SPINDLE_DIR_CW) {
                g_ncSpindleStatus.direction_change_events++;
            }
            g_ncSpindleStatus.enabled = 1U;
            g_ncSpindleStatus.running = 1U;
            g_ncSpindleStatus.direction = NC_SPINDLE_DIR_CW;
            g_ncSpindleStatus.start_events++;
            NcEvent_Push(NC_EVENT_CODE_SPINDLE_START,
                         (int32_t)g_ncSpindleStatus.output_rpm,
                         (int32_t)NC_SPINDLE_DIR_CW,
                         pBlock->line_no);
        } else if (m == M_SPINDLE_CCW) {
            if (g_ncSpindleStatus.direction != NC_SPINDLE_DIR_CCW) {
                g_ncSpindleStatus.direction_change_events++;
            }
            g_ncSpindleStatus.enabled = 1U;
            g_ncSpindleStatus.running = 1U;
            g_ncSpindleStatus.direction = NC_SPINDLE_DIR_CCW;
            g_ncSpindleStatus.start_events++;
            NcEvent_Push(NC_EVENT_CODE_SPINDLE_START,
                         (int32_t)g_ncSpindleStatus.output_rpm,
                         (int32_t)NC_SPINDLE_DIR_CCW,
                         pBlock->line_no);
        } else if (m == M_SPINDLE_STOP) {
            g_ncSpindleStatus.running = 0U;
            g_ncSpindleStatus.direction = NC_SPINDLE_DIR_STOP;
            g_ncSpindleStatus.output_rpm = 0U;
            g_ioOut.spindle_speed = 0U;
            g_ncSpindleStatus.stop_events++;
            NcEvent_Push(NC_EVENT_CODE_SPINDLE_STOP, 0, 0, pBlock->line_no);
        } else if (m == M_SPINDLE_ORIENT) {
            g_ncSpindleStatus.orient_requested = 1U;
            g_ncSpindleStatus.orient_completed = 0U;
            g_ncSpindleStatus.orient_elapsed_ticks = 0U;
            s_orient_start_cycle = g_machineCtx.cycle_count;
            g_ncSpindleStatus.orientation_events++;
            NcEvent_Push(NC_EVENT_CODE_SPINDLE_ORIENT,
                         (int32_t)g_ncSpindleStatus.output_rpm,
                         0,
                         pBlock->line_no);
        }
    }

    if ((pBlock->motion_type == NC_MOTION_THREAD) ||
        ((pBlock->cycle_flags & NC_CYCLE_FLAG_TAP) != 0U)) {
        g_ncSpindleStatus.sync_required_blocks++;
    }
    g_ncSpindleStatus.last_line_no = pBlock->line_no;
    g_ncSpindleStatus.generation++;
}

void NcSpindle_ServiceRt(void)
{
    if (g_ncSpindleStatus.orient_requested == 0U) {
        return;
    }
    g_ncSpindleStatus.orient_elapsed_ticks =
        (uint16_t)(g_machineCtx.cycle_count - s_orient_start_cycle);
    if ((g_ioIn.seq_bits & SENSOR_SPINDLE_ORIENT_DONE_BIT) != 0U) {
        g_ncSpindleStatus.orient_completed = 1U;
        g_ncSpindleStatus.orient_requested = 0U;
        g_ncSpindleStatus.orient_timeout_events = 0U;
        NcEvent_Push(NC_EVENT_CODE_SPINDLE_ORIENT_DONE,
                     (int32_t)g_ncSpindleStatus.orient_elapsed_ticks,
                     0,
                     g_ncSpindleStatus.last_line_no);
    } else if (g_ncSpindleStatus.orient_elapsed_ticks >=
               g_ncSpindleStatus.orient_timeout_ticks) {
        g_ncSpindleStatus.orient_requested = 0U;
        g_ncSpindleStatus.orient_timeout_events++;
        NcEvent_Push(NC_EVENT_CODE_SPINDLE_ORIENT_TIMEOUT,
                     (int32_t)g_ncSpindleStatus.orient_elapsed_ticks,
                     0,
                     g_ncSpindleStatus.last_line_no);
    }
    g_ncSpindleStatus.generation++;
}
