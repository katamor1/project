/* ibm-bob/samples/base/source/src/nc_safety_motion.c */
/* Implements RT-safe motion override for power-fail decel, external decel, position switch, and absolute reference state. */
/* This exists so sensor-driven safety overlays stay deterministic in the RT cycle. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_safety_motion.h, ibm-bob/samples/base/source/src/rt_control.c, ibm-bob/samples/base/source/inc/system_shared.h */
#include <string.h>
#include "nc_safety_motion.h"
#include "nc_program.h"

static uint8_t s_prevPositionSwitch;
static uint8_t s_prevPowerFail;
static uint8_t s_prevExternalDecel;
static uint8_t s_prevAbsRefValid;

/**
 * @brief Reset NC safety motion state to its default values.
 */
void NcSafetyMotion_Reset(void)
{
    (void)memset((void*)&g_ncSafetyMotionStatus, 0, sizeof(g_ncSafetyMotionStatus));
    g_ncSafetyMotionStatus.active_override_percent = NC_FEED_OVERRIDE_DEFAULT_PERCENT;
    s_prevPositionSwitch = 0U;
    s_prevPowerFail = 0U;
    s_prevExternalDecel = 0U;
    s_prevAbsRefValid = 0U;
    g_ncSafetyMotionStatus.generation++;
}

/**
 * @brief Update RT from current inputs.
 */
void NcSafetyMotion_UpdateRt(void)
{
    /* Declare local working state for the next processing stage. */
    uint32_t bits = g_ioIn.sensor_bits;
    uint8_t powerFail = (uint8_t)((bits & SENSOR_POWER_FAIL_BIT) != 0U);
    uint8_t externalDecel = (uint8_t)((bits & SENSOR_EXTERNAL_DECEL_BIT) != 0U);
    uint8_t positionSwitch = (uint8_t)((bits & SENSOR_POSITION_SWITCH_BIT) != 0U);
    uint8_t absRefValid = (uint8_t)((bits & SENSOR_ABSOLUTE_REF_VALID_BIT) != 0U);

    /* Update shared status fields for RT, TS, and API readers. */
    g_ncSafetyMotionStatus.powerfail_decel_stop_active = powerFail;
    g_ncSafetyMotionStatus.external_decel_active = externalDecel;
    g_ncSafetyMotionStatus.absolute_reference_valid = absRefValid;
    g_ncSafetyMotionStatus.active_override_percent = NC_FEED_OVERRIDE_DEFAULT_PERCENT;

    /* Validate state and take the early-exit branch when required. */
    if (powerFail != 0U) {
        g_ncSafetyMotionStatus.active_override_percent = NC_SAFETY_POWERFAIL_DECEL_PERCENT;
        if (s_prevPowerFail == 0U) {
            g_ncSafetyMotionStatus.powerfail_events++;
            NcEvent_Push(NC_EVENT_CODE_POWERFAIL_DECEL_STOP, 0, 0,
                         g_ncProgramStatus.exec_line_no);
        }
    } else if (externalDecel != 0U) {
        g_ncSafetyMotionStatus.active_override_percent = NC_SAFETY_EXTERNAL_DECEL_PERCENT;
        if (s_prevExternalDecel == 0U) {
            g_ncSafetyMotionStatus.external_decel_events++;
            NcEvent_Push(NC_EVENT_CODE_EXTERNAL_DECEL, 0, 0,
                         g_ncProgramStatus.exec_line_no);
        }
    }
    if ((positionSwitch != 0U) && (s_prevPositionSwitch == 0U)) {
        g_ncSafetyMotionStatus.position_switch_latched = 1U;
        g_ncSafetyMotionStatus.position_switch_events++;
        NcEvent_Push(NC_EVENT_CODE_POSITION_SWITCH_LATCH, 0, 0,
                     g_ncProgramStatus.exec_line_no);
    }
    if ((absRefValid == 0U) && (s_prevAbsRefValid != 0U)) {
        g_ncSafetyMotionStatus.absolute_ref_lost_events++;
    }

    /* Apply the next logical update for this processing stage. */
    s_prevPowerFail = powerFail;
    s_prevExternalDecel = externalDecel;
    s_prevPositionSwitch = positionSwitch;
    s_prevAbsRefValid = absRefValid;
    g_ncSafetyMotionStatus.generation++;
}

/**
 * @brief Apply NC safety motion RT-side settings to one executing block.
 * @param pBlock NC execution block to read or update.
 */
void NcSafetyMotion_ApplyBlockRt(NC_EXEC_BLOCK* pBlock)
{
    if (pBlock == NULL) {
        return;
    }
    if (g_ncSafetyMotionStatus.active_override_percent < pBlock->feed_override_percent) {
        pBlock->feed_override_percent = g_ncSafetyMotionStatus.active_override_percent;
        g_ncSafetyMotionStatus.last_line_no = pBlock->line_no;
        g_ncSafetyMotionStatus.generation++;
    }
    if ((g_ncSafetyMotionStatus.powerfail_decel_stop_active != 0U) &&
        (pBlock->interp_ticks < 5U)) {
        pBlock->interp_ticks = 5U;
        pBlock->decel_ticks = 5U;
    }
}
