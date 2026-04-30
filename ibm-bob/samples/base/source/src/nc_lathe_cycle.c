/* ibm-bob/samples/base/source/src/nc_lathe_cycle.c */
/* Implements lightweight turning/grinding/thread cycle tracking for IBM-Bob NC execution. */
/* This exists so design-doc functions that are too geometry-heavy for the sample still have RT-safe observable behavior. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_lathe_cycle.h, ibm-bob/samples/base/source/src/nc_parser_modal.c, ibm-bob/samples/base/source/src/nc_program.c */
#include <string.h>
#include "nc_lathe_cycle.h"
#include "nc_program.h"
#include "system_config.h"

#define G(whole)       NC_G_CODE_WHOLE(whole)
#define GD(whole, dec) NC_G_CODE10((whole), (dec))
/**
 * @brief Return whether turning cycle is true for the current block or state.
 * @param code G-code or M-code value being tested or applied.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
static uint8_t IsTurningCycle(uint16_t code)
{
    return (uint8_t)((code == G(70)) || (code == G(71)) ||
                     (code == G(72)) || (code == G(73)) ||
                     (code == G(74)) || (code == G(75)) ||
                     (code == G(90)) || (code == G(94)));
}

/**
 * @brief Return whether grinding cycle is true for the current block or state.
 * @param code G-code or M-code value being tested or applied.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
static uint8_t IsGrindingCycle(uint16_t code)
{
    return (uint8_t)(((code >= GD(70, 7)) && (code <= GD(76, 7))) ||
                     (code == G(77)) || (code == G(78)) || (code == G(79)));
}

/**
 * @brief Return whether thread cycle is true for the current block or state.
 * @param code G-code or M-code value being tested or applied.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
static uint8_t IsThreadCycle(uint16_t code)
{
    return (uint8_t)((code == G(32)) || (code == G(76)) ||
                     (code == GD(76, 7)) || (code == G(78)) ||
                     (code == G(92)));
}

/**
 * @brief Copy last axes between fixed-size buffers.
 * @param pDst Destination axis buffer updated by the helper.
 * @param pSrc Source axis buffer copied by the helper.
 */
static void CopyLastAxes(volatile int32_t* pDst, const int32_t* pSrc)
{
    uint32_t i;
    for (i = 0U; i < AXIS_MAX; ++i) {
        pDst[i] = pSrc[i];
    }
}

/**
 * @brief Handle nc lathe cycle reset for this module.
 */
void NcLatheCycle_Reset(void)
{
    (void)memset((void*)&g_ncTurningCycleStatus, 0, sizeof(g_ncTurningCycleStatus));
    (void)memset((void*)&g_ncThreadCycleStatus, 0, sizeof(g_ncThreadCycleStatus));
    g_ncTurningCycleStatus.radius_mode_active = 1U;
}

/**
 * @brief Handle nc lathe cycle set diameter mode for this module.
 * @param enabled Non-zero to enable the mode; zero to disable it.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcLatheCycle_SetDiameterMode(uint8_t enabled)
{
    g_ncTurningCycleStatus.diameter_mode_active = (enabled != 0U) ? 1U : 0U;
    g_ncTurningCycleStatus.radius_mode_active = (enabled == 0U) ? 1U : 0U;
    g_ncTurningCycleStatus.generation++;
    return 0;
}

/**
 * @brief Update turning ts from current inputs.
 * @param pBlock NC execution block read or updated by the helper.
 * @param code G-code or M-code value being tested or applied.
 */
static void UpdateTurningTs(const NC_EXEC_BLOCK* pBlock, uint16_t code)
{
    /* Apply the next logical update for this processing stage. */
    g_ncTurningCycleStatus.planned_cycle_blocks++;
    g_ncTurningCycleStatus.last_cycle_code10 = code;
    g_ncTurningCycleStatus.last_line_no = pBlock->line_no;
    g_ncTurningCycleStatus.last_depth = pBlock->q_value;
    g_ncTurningCycleStatus.last_retract = pBlock->r_value;
    CopyLastAxes(g_ncTurningCycleStatus.last_target, pBlock->axis_target);
    if (code == G(70)) { g_ncTurningCycleStatus.finishing_blocks++; }
    else if (code == G(71)) { g_ncTurningCycleStatus.roughing_blocks++; }
    else if (code == G(72)) { g_ncTurningCycleStatus.facing_blocks++; }
    else if (code == G(73)) { g_ncTurningCycleStatus.pattern_repeat_blocks++; }
    else if ((code == G(74)) || (code == G(75))) { g_ncTurningCycleStatus.grooving_blocks++; }
    else if (code == G(90)) { g_ncTurningCycleStatus.linear_taper_blocks++; }
    else if (code == G(94)) { g_ncTurningCycleStatus.end_face_blocks++; }
    /* Apply the next logical update for this processing stage. */
    g_ncTurningCycleStatus.generation++;
}

/**
 * @brief Update grinding ts from current inputs.
 * @param pBlock NC execution block read or updated by the helper.
 * @param code G-code or M-code value being tested or applied.
 */
static void UpdateGrindingTs(const NC_EXEC_BLOCK* pBlock, uint16_t code)
{
    g_ncTurningCycleStatus.planned_cycle_blocks++;
    g_ncTurningCycleStatus.grinding_blocks++;
    g_ncTurningCycleStatus.last_cycle_code10 = code;
    g_ncTurningCycleStatus.last_line_no = pBlock->line_no;
    g_ncTurningCycleStatus.last_depth = pBlock->q_value;
    g_ncTurningCycleStatus.last_retract = pBlock->r_value;
    CopyLastAxes(g_ncTurningCycleStatus.last_target, pBlock->axis_target);
    g_ncTurningCycleStatus.generation++;
}

/**
 * @brief Update thread ts from current inputs.
 * @param pBlock NC execution block read or updated by the helper.
 * @param code G-code or M-code value being tested or applied.
 */
static void UpdateThreadTs(const NC_EXEC_BLOCK* pBlock, uint16_t code)
{
    /* Apply the next logical update for this processing stage. */
    g_ncThreadCycleStatus.planned_thread_blocks++;
    g_ncThreadCycleStatus.last_thread_code10 = code;
    g_ncThreadCycleStatus.last_line_no = pBlock->line_no;
    g_ncThreadCycleStatus.last_lead = pBlock->feed;
    g_ncThreadCycleStatus.last_depth = pBlock->q_value;
    g_ncThreadCycleStatus.spindle_sync_required = 1U;
    CopyLastAxes(g_ncThreadCycleStatus.last_target, pBlock->axis_target);
    /* Handle the next conditional branch for this processing stage. */
    if (code == G(32)) { g_ncThreadCycleStatus.g32_blocks++; }
    else if (code == G(76)) { g_ncThreadCycleStatus.g76_blocks++; }
    else if (code == GD(76, 7)) { g_ncThreadCycleStatus.g767_blocks++; }
    else if (code == G(78)) { g_ncThreadCycleStatus.g78_blocks++; }
    else if (code == G(92)) { g_ncThreadCycleStatus.g92_blocks++; }
    if ((pBlock->modal_flags & NC_ARC_FLAG_IJK) != 0U) {
        g_ncThreadCycleStatus.circular_thread_blocks++;
    }
    if ((pBlock->feature_flags & NC_FEATURE_FLAG_PARAM_Q) != 0U) {
        g_ncThreadCycleStatus.variable_lead_blocks++;
    }
    g_ncThreadCycleStatus.generation++;
}

/**
 * @brief Handle nc lathe cycle on parsed block ts for this module.
 * @param pBlock NC execution block read or updated by the helper.
 */
void NcLatheCycle_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock)
{
    uint16_t code;
    if (pBlock == 0) {
        return;
    }
    code = (pBlock->cycle_code10 != 0U) ? pBlock->cycle_code10 : pBlock->g_code10;
    if (IsTurningCycle(code) != 0U) {
        UpdateTurningTs(pBlock, code);
    }
    if (IsGrindingCycle(code) != 0U) {
        UpdateGrindingTs(pBlock, code);
    }
    if ((IsThreadCycle(code) != 0U) ||
        ((pBlock->feature_flags & NC_FEATURE_FLAG_THREAD) != 0U)) {
        UpdateThreadTs(pBlock, code);
    }
}

/**
 * @brief Handle nc lathe cycle on block rt for this module.
 * @param pBlock NC execution block read or updated by the helper.
 */
void NcLatheCycle_OnBlockRt(const NC_EXEC_BLOCK* pBlock)
{
    uint16_t code;
    if (pBlock == 0) {
        return;
    }
    code = (pBlock->cycle_code10 != 0U) ? pBlock->cycle_code10 : pBlock->g_code10;
    if ((IsTurningCycle(code) != 0U) || (IsGrindingCycle(code) != 0U)) {
        g_ncTurningCycleStatus.active = 1U;
        g_ncTurningCycleStatus.executed_cycle_blocks++;
        g_ncTurningCycleStatus.last_cycle_code10 = code;
        g_ncTurningCycleStatus.last_line_no = pBlock->line_no;
        g_ncTurningCycleStatus.generation++;
        NcEvent_Push(NC_EVENT_CODE_LATHE_CYCLE_EXECUTED,
                     (int32_t)code,
                     (int32_t)pBlock->line_no,
                     pBlock->line_no);
    }
    if ((IsThreadCycle(code) != 0U) ||
        ((pBlock->feature_flags & NC_FEATURE_FLAG_THREAD) != 0U)) {
        g_ncThreadCycleStatus.active = 1U;
        g_ncThreadCycleStatus.executed_thread_blocks++;
        g_ncThreadCycleStatus.last_thread_code10 = code;
        g_ncThreadCycleStatus.last_line_no = pBlock->line_no;
        g_ncThreadCycleStatus.generation++;
        NcEvent_Push(NC_EVENT_CODE_THREAD_CYCLE_EXECUTED,
                     (int32_t)code,
                     (int32_t)pBlock->feed,
                     pBlock->line_no);
    }
}
