/* ibm-bob/samples/base/source/src/nc_safety_motion.c */
/* Implements RT-safe motion override for power-fail decel, external decel, position switch, and absolute reference state. */
#include <string.h>
#include "nc_safety_motion.h"
#include "nc_program.h"

static uint8_t s_prevPositionSwitch;
static uint8_t s_prevPowerFail;
static uint8_t s_prevExternalDecel;
static uint8_t s_prevAbsRefValid;

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

void NcSafetyMotion_UpdateRt(void)
{
    uint32_t bits = g_ioIn.sensor_bits;
    uint8_t powerFail = (uint8_t)((bits & SENSOR_POWER_FAIL_BIT) != 0U);
    uint8_t externalDecel = (uint8_t)((bits & SENSOR_EXTERNAL_DECEL_BIT) != 0U);
    uint8_t positionSwitch = (uint8_t)((bits & SENSOR_POSITION_SWITCH_BIT) != 0U);
    uint8_t absRefValid = (uint8_t)((bits & SENSOR_ABSOLUTE_REF_VALID_BIT) != 0U);

    g_ncSafetyMotionStatus.powerfail_decel_stop_active = powerFail;
    g_ncSafetyMotionStatus.external_decel_active = externalDecel;
    g_ncSafetyMotionStatus.absolute_reference_valid = absRefValid;
    g_ncSafetyMotionStatus.active_override_percent = NC_FEED_OVERRIDE_DEFAULT_PERCENT;

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

    s_prevPowerFail = powerFail;
    s_prevExternalDecel = externalDecel;
    s_prevPositionSwitch = positionSwitch;
    s_prevAbsRefValid = absRefValid;
    g_ncSafetyMotionStatus.generation++;
}

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
