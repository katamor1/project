/* ibm-bob/samples/base/source/src/nc_synchronization.c */
/* Implements synchronization, compound, overlay, and double-table status handling. */
/* This exists so multi-axis mode design documents are represented by deterministic fixed-size RT code. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_synchronization.h, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/inc/control_api.h */
#include <string.h>
#include "nc_synchronization.h"
#include "nc_program.h"
#include "system_config.h"

#define G(whole, dec) NC_G_CODE10((whole), (dec))
#define MODE_SYNC_BIT      (0x0001U)
#define MODE_COMPOUND_BIT  (0x0002U)
#define MODE_OVERLAY_BIT   (0x0004U)
#define MODE_DOUBLE_TABLE  (0x0008U)

static int32_t Abs32(int32_t value)
{
    return (value < 0) ? -value : value;
}

static void UpdateModeBits(void)
{
    uint16_t bits = 0U;

    if (g_ncSynchronizationStatus.sync_enabled != 0U) {
        bits |= MODE_SYNC_BIT;
    }
    if (g_ncSynchronizationStatus.compound_enabled != 0U) {
        bits |= MODE_COMPOUND_BIT;
    }
    if (g_ncSynchronizationStatus.overlay_enabled != 0U) {
        bits |= MODE_OVERLAY_BIT;
    }
    if (g_ncSynchronizationStatus.double_table_enabled != 0U) {
        bits |= MODE_DOUBLE_TABLE;
    }
    g_ncSynchronizationStatus.active_mode_bits = bits;
}

void NcSynchronization_Reset(void)
{
    uint8_t masterAxis = g_ncSynchronizationStatus.master_axis;
    uint32_t slaveMask = g_ncSynchronizationStatus.slave_axis_mask;
    uint32_t overlayMask = g_ncSynchronizationStatus.overlay_axis_mask;
    uint32_t compoundMask = g_ncSynchronizationStatus.compound_path_mask;
    uint8_t doubleTable = g_ncSynchronizationStatus.double_table_enabled;
    int32_t overlayOffset[AXIS_MAX];
    uint8_t axis;

    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        overlayOffset[axis] = g_ncSynchronizationStatus.overlay_offset[axis];
    }

    (void)memset((void*)&g_ncSynchronizationStatus, 0, sizeof(g_ncSynchronizationStatus));
    g_ncSynchronizationStatus.master_axis = (masterAxis < AXIS_MAX) ?
        masterAxis : NC_SYNC_DEFAULT_MASTER_AXIS;
    g_ncSynchronizationStatus.slave_axis_mask = (slaveMask != 0U) ?
        slaveMask : NC_SYNC_DEFAULT_SLAVE_MASK;
    g_ncSynchronizationStatus.overlay_axis_mask = overlayMask;
    g_ncSynchronizationStatus.compound_path_mask = compoundMask;
    g_ncSynchronizationStatus.double_table_enabled = doubleTable;
    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        g_ncSynchronizationStatus.overlay_offset[axis] = overlayOffset[axis];
    }
    UpdateModeBits();
    g_ncSynchronizationStatus.generation++;
}

int32_t NcSynchronization_SetMasterSlave(uint8_t masterAxis, uint32_t slaveAxisMask)
{
    uint32_t validMask = (AXIS_MAX >= 32U) ? 0xffffffffUL : ((1UL << AXIS_MAX) - 1UL);

    if (masterAxis >= AXIS_MAX) {
        return -1;
    }
    slaveAxisMask &= validMask;
    slaveAxisMask &= ~(1UL << masterAxis);
    if (slaveAxisMask == 0U) {
        return -1;
    }
    g_ncSynchronizationStatus.master_axis = masterAxis;
    g_ncSynchronizationStatus.slave_axis_mask = slaveAxisMask;
    g_ncSynchronizationStatus.generation++;
    return 0;
}

int32_t NcSynchronization_SetOverlayAxis(uint8_t axis, int32_t offset)
{
    if (axis >= AXIS_MAX) {
        return -1;
    }
    g_ncSynchronizationStatus.overlay_offset[axis] = offset;
    if (offset == 0) {
        g_ncSynchronizationStatus.overlay_axis_mask &= ~(1UL << axis);
    } else {
        g_ncSynchronizationStatus.overlay_axis_mask |= (1UL << axis);
    }
    g_ncSynchronizationStatus.generation++;
    return 0;
}

int32_t NcSynchronization_SetCompoundPathMask(uint32_t pathMask)
{
    uint32_t validMask = (AXIS_MAX >= 32U) ? 0xffffffffUL : ((1UL << AXIS_MAX) - 1UL);

    g_ncSynchronizationStatus.compound_path_mask = pathMask & validMask;
    g_ncSynchronizationStatus.generation++;
    return 0;
}

int32_t NcSynchronization_SetDoubleTable(uint8_t enabled, uint32_t slaveAxisMask)
{
    uint32_t validMask = (AXIS_MAX >= 32U) ? 0xffffffffUL : ((1UL << AXIS_MAX) - 1UL);

    g_ncSynchronizationStatus.double_table_enabled = (enabled != 0U) ? 1U : 0U;
    g_ncSynchronizationStatus.slave_axis_mask = slaveAxisMask & validMask;
    UpdateModeBits();
    g_ncSynchronizationStatus.generation++;
    return 0;
}

static uint8_t IsModeCommand(uint32_t code10)
{
    return (uint8_t)((code10 == G(51, 4)) || (code10 == G(50, 4)) ||
                     (code10 == G(51, 5)) || (code10 == G(50, 5)) ||
                     (code10 == G(51, 6)) || (code10 == G(50, 6)));
}

void NcSynchronization_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock)
{
    if (pBlock == 0) {
        return;
    }
    if (IsModeCommand(pBlock->g_code10) != 0U) {
        g_ncSynchronizationStatus.planned_mode_blocks++;
        g_ncSynchronizationStatus.last_mode_code10 = pBlock->g_code10;
        g_ncSynchronizationStatus.last_line_no = pBlock->line_no;
        g_ncSynchronizationStatus.generation++;
    }
}

static void ApplyModeCommandRt(const NC_EXEC_BLOCK* pBlock)
{
    uint8_t modeChanged = 0U;

    if (pBlock->g_code10 == G(51, 4)) {
        g_ncSynchronizationStatus.sync_enabled = 1U;
        modeChanged = 1U;
    } else if (pBlock->g_code10 == G(50, 4)) {
        g_ncSynchronizationStatus.sync_enabled = 0U;
        modeChanged = 2U;
    } else if (pBlock->g_code10 == G(51, 5)) {
        g_ncSynchronizationStatus.compound_enabled = 1U;
        modeChanged = 1U;
    } else if (pBlock->g_code10 == G(50, 5)) {
        g_ncSynchronizationStatus.compound_enabled = 0U;
        modeChanged = 2U;
    } else if (pBlock->g_code10 == G(51, 6)) {
        g_ncSynchronizationStatus.overlay_enabled = 1U;
        modeChanged = 1U;
    } else if (pBlock->g_code10 == G(50, 6)) {
        g_ncSynchronizationStatus.overlay_enabled = 0U;
        modeChanged = 2U;
    } else {
        return;
    }

    if (modeChanged == 1U) {
        g_ncSynchronizationStatus.mode_switch_events++;
    } else {
        g_ncSynchronizationStatus.mode_cancel_events++;
    }
    g_ncSynchronizationStatus.executed_mode_blocks++;
    g_ncSynchronizationStatus.last_mode_code10 = pBlock->g_code10;
    g_ncSynchronizationStatus.last_line_no = pBlock->line_no;
    UpdateModeBits();
    NcEvent_Push(NC_EVENT_CODE_SYNC_MODE_CHANGE,
                 (int32_t)pBlock->g_code10,
                 (int32_t)g_ncSynchronizationStatus.active_mode_bits,
                 pBlock->line_no);
}

static void TrackFollowingError(uint8_t axis, int32_t error, uint32_t lineNo)
{
    g_ncSynchronizationStatus.last_following_error[axis] = error;
    if (Abs32(error) > Abs32(g_ncSynchronizationStatus.max_following_error[axis])) {
        g_ncSynchronizationStatus.max_following_error[axis] = error;
    }
    if (Abs32(error) > NC_SYNC_FOLLOWING_WARN_THRESHOLD) {
        g_ncSynchronizationStatus.following_warn_events++;
        NcEvent_Push(NC_EVENT_CODE_SYNC_LOSS_WARN, (int32_t)axis, error, lineNo);
    }
}

static void ApplyMasterSlaveRt(NC_EXEC_BLOCK* pBlock)
{
    uint8_t master = g_ncSynchronizationStatus.master_axis;
    int32_t masterDelta;
    uint32_t slaveMask;
    uint8_t axis;
    uint8_t applied = 0U;

    if ((g_ncSynchronizationStatus.sync_enabled == 0U) &&
        (g_ncSynchronizationStatus.double_table_enabled == 0U)) {
        return;
    }
    if (master >= AXIS_MAX) {
        return;
    }
    if ((pBlock->axis_mask & (1UL << master)) == 0U) {
        return;
    }

    masterDelta = pBlock->axis_target[master] - pBlock->axis_start[master];
    slaveMask = g_ncSynchronizationStatus.slave_axis_mask;
    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        if ((slaveMask & (1UL << axis)) == 0U) {
            continue;
        }
        if ((pBlock->axis_mask & (1UL << axis)) == 0U) {
            pBlock->axis_target[axis] = pBlock->axis_start[axis] + masterDelta;
            pBlock->axis_mask |= (1UL << axis);
            applied = 1U;
        }
        TrackFollowingError(axis,
                            pBlock->axis_target[axis] -
                            (pBlock->axis_start[axis] + masterDelta),
                            pBlock->line_no);
    }

    if (applied != 0U) {
        g_ncSynchronizationStatus.synchronized_motion_blocks++;
        g_ncSynchronizationStatus.last_master_delta = masterDelta;
        NcEvent_Push(NC_EVENT_CODE_SYNC_MOTION_APPLIED,
                     (int32_t)master,
                     masterDelta,
                     pBlock->line_no);
    }
}

static void ApplyOverlayRt(NC_EXEC_BLOCK* pBlock)
{
    uint8_t axis;
    uint8_t applied = 0U;

    if (g_ncSynchronizationStatus.overlay_enabled == 0U) {
        return;
    }
    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        if ((g_ncSynchronizationStatus.overlay_axis_mask & (1UL << axis)) == 0U) {
            continue;
        }
        pBlock->axis_target[axis] += g_ncSynchronizationStatus.overlay_offset[axis];
        pBlock->axis_mask |= (1UL << axis);
        applied = 1U;
    }
    if (applied != 0U) {
        g_ncSynchronizationStatus.overlay_motion_blocks++;
        NcEvent_Push(NC_EVENT_CODE_OVERLAY_APPLIED,
                     (int32_t)g_ncSynchronizationStatus.overlay_axis_mask,
                     0,
                     pBlock->line_no);
    }
}

static void TrackCompoundRt(const NC_EXEC_BLOCK* pBlock)
{
    if (g_ncSynchronizationStatus.compound_enabled == 0U) {
        return;
    }
    if ((pBlock->axis_mask & g_ncSynchronizationStatus.compound_path_mask) != 0U) {
        g_ncSynchronizationStatus.compound_motion_blocks++;
    }
}

void NcSynchronization_ApplyBlockRt(NC_EXEC_BLOCK* pBlock)
{
    uint8_t axis;

    if (pBlock == 0) {
        return;
    }

    ApplyModeCommandRt(pBlock);
    if ((pBlock->axis_mask == 0U) ||
        ((pBlock->motion_type != NC_MOTION_RAPID) &&
         (pBlock->motion_type != NC_MOTION_LINEAR) &&
         (pBlock->motion_type != NC_MOTION_ARC_CW) &&
         (pBlock->motion_type != NC_MOTION_ARC_CCW) &&
         (pBlock->motion_type != NC_MOTION_ADVANCED_INTERP))) {
        g_ncSynchronizationStatus.generation++;
        return;
    }

    ApplyMasterSlaveRt(pBlock);
    ApplyOverlayRt(pBlock);
    TrackCompoundRt(pBlock);

    if (g_ncSynchronizationStatus.double_table_enabled != 0U) {
        g_ncSynchronizationStatus.double_table_blocks++;
    }
    g_ncSynchronizationStatus.last_line_no = pBlock->line_no;
    for (axis = 0U; axis < AXIS_MAX; ++axis) {
        g_ncSynchronizationStatus.last_output_target[axis] = pBlock->axis_target[axis];
    }
    g_ncSynchronizationStatus.generation++;
}
