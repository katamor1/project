/* ibm-bob/samples/base/source/src/nc_feed.c */
/* Calculates NC feed ticks, override, dwell timing, and profile ratio. */
/* This exists so feed behavior is explicit and separate from interpolation. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_feed.h, ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/src/control_api.c */
#include <math.h>
#include <string.h>
#include "nc_feed.h"

static uint16_t s_overridePercent = NC_FEED_OVERRIDE_DEFAULT_PERCENT;

static uint32_t ClampTicks(double ticks)
{
    if (ticks < (double)NC_INTERP_MIN_TICKS) {
        ticks = (double)NC_INTERP_MIN_TICKS;
    }
    if (ticks > (double)NC_INTERP_MAX_TICKS) {
        ticks = (double)NC_INTERP_MAX_TICKS;
    }
    return (uint32_t)ticks;
}

static void SetProfile(NC_EXEC_BLOCK* pBlock)
{
    uint32_t edge = NC_FEED_ACCEL_TICKS_DEFAULT;

    pBlock->feed_profile = NC_FEED_PROFILE_TRAPEZOID;
    pBlock->feed_override_percent = s_overridePercent;
    if (pBlock->interp_ticks <= 1U) {
        pBlock->accel_ticks = 0U;
        pBlock->decel_ticks = 0U;
        return;
    }
    if ((edge * 2U) >= pBlock->interp_ticks) {
        edge = pBlock->interp_ticks / 2U;
    }
    pBlock->accel_ticks = edge;
    pBlock->decel_ticks = edge;
}

void NcFeed_Reset(void)
{
    s_overridePercent = NC_FEED_OVERRIDE_DEFAULT_PERCENT;
    (void)memset((void*)&g_ncFeedStatus, 0, sizeof(g_ncFeedStatus));
    g_ncFeedStatus.state = NC_FEED_CTRL_IDLE;
    g_ncFeedStatus.feed_override_percent = s_overridePercent;
}

void NcFeed_CancelRt(void)
{
    g_ncFeedStatus.state = NC_FEED_CTRL_IDLE;
    g_ncFeedStatus.generation++;
}

int32_t NcFeed_SetOverride(uint16_t percent)
{
    if (percent > NC_FEED_OVERRIDE_MAX_PERCENT) {
        return -1;
    }
    s_overridePercent = percent;
    g_ncFeedStatus.feed_override_percent = percent;
    g_ncFeedStatus.generation++;
    return 0;
}

int32_t NcFeed_PrepareMotionTs(NC_EXEC_BLOCK* pBlock,
                               double pathLengthUnits,
                               NC_ERROR_CODE* pOutError)
{
    double feedUnitsPerMin;
    double ticks;

    if ((pBlock == 0) || (pOutError == 0)) {
        return -1;
    }
    pBlock->feed_override_percent = s_overridePercent;
    if (s_overridePercent == 0U) {
        pBlock->interp_ticks = NC_INTERP_MIN_TICKS;
        SetProfile(pBlock);
        return 0;
    }

    if (pBlock->motion_type == NC_MOTION_RAPID) {
        feedUnitsPerMin = (double)NC_RAPID_FEED_UNITS_PER_MIN;
    } else {
        feedUnitsPerMin = (double)pBlock->feed / (double)NC_POSITION_SCALE;
        if (pBlock->feed_mode == NC_FEED_MODE_PER_REV) {
            if (pBlock->spindle_speed == 0U) {
                *pOutError = NC_ERR_FEED;
                return -1;
            }
            feedUnitsPerMin *= (double)pBlock->spindle_speed;
        }
    }
    feedUnitsPerMin *= ((double)s_overridePercent / 100.0);
    if ((feedUnitsPerMin <= 0.0) || (pathLengthUnits < 0.0)) {
        *pOutError = NC_ERR_FEED;
        return -1;
    }

    ticks = ceil((pathLengthUnits * 60000.0) /
                 (feedUnitsPerMin * (double)CYCLE_TICK_MS));
    pBlock->interp_ticks = ClampTicks(ticks);
    SetProfile(pBlock);
    return 0;
}

int32_t NcFeed_PrepareDwellTs(NC_EXEC_BLOCK* pBlock,
                              uint32_t dwellTicks,
                              NC_ERROR_CODE* pOutError)
{
    if ((pBlock == 0) || (pOutError == 0) ||
        (dwellTicks > NC_DWELL_MAX_TICKS)) {
        if (pOutError != 0) {
            *pOutError = NC_ERR_RANGE;
        }
        return -1;
    }
    pBlock->interp_ticks = (dwellTicks == 0U) ? 1U : dwellTicks;
    pBlock->feed_profile = NC_FEED_PROFILE_DWELL;
    pBlock->feed_override_percent = s_overridePercent;
    return 0;
}

int32_t NcFeed_BeginBlockRt(const NC_EXEC_BLOCK* pBlock)
{
    if (pBlock == 0) {
        return -1;
    }
    g_ncFeedStatus.line_no = pBlock->line_no;
    g_ncFeedStatus.active_tick = 0U;
    g_ncFeedStatus.active_total_ticks = pBlock->interp_ticks;
    g_ncFeedStatus.feed_profile = pBlock->feed_profile;
    g_ncFeedStatus.feed_override_percent = pBlock->feed_override_percent;
    if ((pBlock->motion_type != NC_MOTION_DWELL) &&
        (pBlock->feed_override_percent == 0U)) {
        g_ncFeedStatus.state = NC_FEED_CTRL_HOLD;
        g_ncFeedStatus.generation++;
        return -2;
    }
    g_ncFeedStatus.state = (pBlock->motion_type == NC_MOTION_DWELL) ?
                           NC_FEED_CTRL_DWELL : NC_FEED_CTRL_ACTIVE;
    g_ncFeedStatus.generation++;
    return 0;
}

void NcFeed_UpdateRt(const NC_EXEC_BLOCK* pBlock, uint32_t tick, uint8_t done)
{
    if (pBlock == 0) {
        return;
    }
    g_ncFeedStatus.active_tick = tick;
    if (done != 0U) {
        if (pBlock->motion_type == NC_MOTION_DWELL) {
            g_ncFeedStatus.completed_dwell_ticks += pBlock->interp_ticks;
        }
        g_ncFeedStatus.state = NC_FEED_CTRL_COMPLETED;
        g_ncFeedStatus.generation++;
    }
}

double NcFeed_ProfileRatioRt(const NC_EXEC_BLOCK* pBlock, uint32_t tick)
{
    double p;

    if ((pBlock == 0) || (pBlock->interp_ticks == 0U)) {
        return 1.0;
    }
    if (tick >= pBlock->interp_ticks) {
        return 1.0;
    }
    p = (double)tick / (double)pBlock->interp_ticks;
    if ((pBlock->accel_ticks == 0U) && (pBlock->decel_ticks == 0U)) {
        return p;
    }
    return (p < 0.5) ? (2.0 * p * p) : (1.0 - (2.0 * (1.0 - p) * (1.0 - p)));
}
