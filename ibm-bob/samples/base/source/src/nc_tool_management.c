/* ibm-bob/samples/base/source/src/nc_tool_management.c */
/* Implements fixed-size NC tool management state for T words and M06 exchange. */
/* This exists so RT only publishes deterministic status and waits for MFIN, while TS parses tool requests. */
#include <string.h>
#include "nc_tool_management.h"
#include "nc_program.h"
/**
 * @brief Return whether valid tool is true for the current block or state.
 * @param toolNo Tool number being updated in tool-life status.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
static uint8_t IsValidTool(uint32_t toolNo)
{
    return (uint8_t)((toolNo > 0U) && (toolNo < NC_TOOL_TABLE_SIZE));
}

/**
 * @brief Return whether valid pocket is true for the current block or state.
 * @param pocketNo Number identifying pocket.
 * @return Non-zero when the helper condition is true; zero when it is false or rejected.
 */
static uint8_t IsValidPocket(uint32_t pocketNo)
{
    return (uint8_t)((pocketNo > 0U) && (pocketNo < NC_TOOL_MAGAZINE_POCKETS));
}

/**
 * @brief Handle recount registered tools for this module.
 */
static void RecountRegisteredTools(void)
{
    uint32_t i;
    uint32_t count = 0U;

    for (i = 1U; i < NC_TOOL_TABLE_SIZE; ++i) {
        if (g_ncToolManagementStatus.tool_to_pocket[i] != 0U) {
            count++;
        }
    }
    g_ncToolManagementStatus.registered_tools = count;
}

/**
 * @brief Handle ensure default table for this module.
 */
static void EnsureDefaultTable(void)
{
    uint32_t i;

    if (g_ncToolManagementStatus.table_initialized != 0U) {
        return;
    }
    (void)memset((void*)g_ncToolManagementStatus.tool_to_pocket,
                 0,
                 sizeof(g_ncToolManagementStatus.tool_to_pocket));
    (void)memset((void*)g_ncToolManagementStatus.pocket_tool,
                 0,
                 sizeof(g_ncToolManagementStatus.pocket_tool));
    for (i = 1U; (i < NC_TOOL_TABLE_SIZE) && (i < NC_TOOL_MAGAZINE_POCKETS); ++i) {
        g_ncToolManagementStatus.tool_to_pocket[i] = i;
        g_ncToolManagementStatus.pocket_tool[i] = i;
    }
    g_ncToolManagementStatus.table_initialized = 1U;
    RecountRegisteredTools();
    g_ncToolManagementStatus.generation++;
}

/**
 * @brief Handle nc tool management reset for this module.
 */
void NcToolManagement_Reset(void)
{
    uint32_t toolToPocket[NC_TOOL_TABLE_SIZE];
    uint32_t pocketTool[NC_TOOL_MAGAZINE_POCKETS];
    uint8_t initialized;
    uint32_t registered;

    EnsureDefaultTable();
    (void)memcpy(toolToPocket,
                 (const void*)g_ncToolManagementStatus.tool_to_pocket,
                 sizeof(toolToPocket));
    (void)memcpy(pocketTool,
                 (const void*)g_ncToolManagementStatus.pocket_tool,
                 sizeof(pocketTool));
    initialized = g_ncToolManagementStatus.table_initialized;
    registered = g_ncToolManagementStatus.registered_tools;

    (void)memset((void*)&g_ncToolManagementStatus, 0, sizeof(g_ncToolManagementStatus));
    (void)memcpy((void*)g_ncToolManagementStatus.tool_to_pocket,
                 toolToPocket,
                 sizeof(toolToPocket));
    (void)memcpy((void*)g_ncToolManagementStatus.pocket_tool,
                 pocketTool,
                 sizeof(pocketTool));
    g_ncToolManagementStatus.table_initialized = initialized;
    g_ncToolManagementStatus.registered_tools = registered;
    g_ncToolManagementStatus.generation++;
}

/**
 * @brief Handle nc tool management set pocket for this module.
 * @param toolNo Tool number being updated in tool-life status.
 * @param pocketNo Number identifying pocket.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcToolManagement_SetPocket(uint32_t toolNo, uint32_t pocketNo)
{
    /* Prepare local state used by the following processing stage. */
    uint32_t oldTool;
    uint32_t oldPocket;

    EnsureDefaultTable();
    if (IsValidTool(toolNo) == 0U) {
        g_ncToolManagementStatus.invalid_tool_events++;
        g_ncToolManagementStatus.generation++;
        return -1;
    }
    if ((pocketNo != 0U) && (IsValidPocket(pocketNo) == 0U)) {
        g_ncToolManagementStatus.no_pocket_events++;
        g_ncToolManagementStatus.generation++;
        return -1;
    }

    oldPocket = g_ncToolManagementStatus.tool_to_pocket[toolNo];
    /* Handle the next conditional branch for this processing stage. */
    if (oldPocket != 0U) {
        g_ncToolManagementStatus.pocket_tool[oldPocket] = 0U;
    }

    if (pocketNo != 0U) {
        oldTool = g_ncToolManagementStatus.pocket_tool[pocketNo];
        if ((oldTool != 0U) && (oldTool != toolNo)) {
            g_ncToolManagementStatus.tool_to_pocket[oldTool] = 0U;
            g_ncToolManagementStatus.duplicate_pocket_events++;
        }
        g_ncToolManagementStatus.pocket_tool[pocketNo] = toolNo;
    }
    g_ncToolManagementStatus.tool_to_pocket[toolNo] = pocketNo;
    RecountRegisteredTools();
    g_ncToolManagementStatus.generation++;
    return 0;
}

/**
 * @brief Handle nc tool management request prepare for this module.
 * @param toolNo Tool number being updated in tool-life status.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcToolManagement_RequestPrepare(uint32_t toolNo)
{
    EnsureDefaultTable();
    if (IsValidTool(toolNo) == 0U) {
        g_ncToolManagementStatus.invalid_tool_events++;
        g_ncToolManagementStatus.generation++;
        return -1;
    }
    if (g_ncToolManagementStatus.tool_to_pocket[toolNo] == 0U) {
        g_ncToolManagementStatus.no_pocket_events++;
        g_ncToolManagementStatus.generation++;
        return -1;
    }

    g_ncToolManagementStatus.requested_tool_no = toolNo;
    g_ncToolManagementStatus.prepared_tool_no = toolNo;
    g_ncToolManagementStatus.prepare_requested = 1U;
    g_ncToolManagementStatus.prepare_events++;
    g_ncToolManagementStatus.generation++;
    NcEvent_Push(NC_EVENT_CODE_TOOL_PREPARE,
                 (int32_t)toolNo,
                 (int32_t)g_ncToolManagementStatus.tool_to_pocket[toolNo],
                 g_ncToolManagementStatus.last_line_no);
    return 0;
}

/**
 * @brief Handle nc tool management on parsed block ts for this module.
 * @param pBlock NC execution block read or updated by the helper.
 */
void NcToolManagement_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock)
{
    if (pBlock == NULL) {
        return;
    }
    EnsureDefaultTable();
    if ((pBlock->aux_flags & NC_AUX_FLAG_TOOL) != 0U) {
        g_ncToolManagementStatus.parsed_t_words++;
        g_ncToolManagementStatus.last_t_code = pBlock->tool_no;
        g_ncToolManagementStatus.requested_tool_no = pBlock->tool_no;
        g_ncToolManagementStatus.last_line_no = pBlock->line_no;
        if (NcToolManagement_RequestPrepare(pBlock->tool_no) != 0) {
            NcEvent_Push(NC_EVENT_CODE_TOOL_MANAGEMENT_WARN,
                         (int32_t)pBlock->tool_no,
                         -1,
                         pBlock->line_no);
        }
    }
    if ((pBlock->aux_flags & NC_AUX_FLAG_M_CODE) != 0U) {
        g_ncToolManagementStatus.last_m_code = pBlock->aux_m_code;
    }
    g_ncToolManagementStatus.generation++;
}

/**
 * @brief Handle nc tool management on block rt for this module.
 * @param pBlock NC execution block read or updated by the helper.
 */
void NcToolManagement_OnBlockRt(const NC_EXEC_BLOCK* pBlock)
{
    /* Prepare local state used by the following processing stage. */
    uint32_t toolNo;
    uint32_t pocket;

    if (pBlock == NULL) {
        return;
    }
    /* Apply the next logical update for this processing stage. */
    EnsureDefaultTable();
    if ((pBlock->aux_flags & NC_AUX_FLAG_TOOL) != 0U) {
        g_ncToolManagementStatus.rt_t_commands++;
        g_ncToolManagementStatus.last_t_code = pBlock->tool_no;
        g_ncToolManagementStatus.requested_tool_no = pBlock->tool_no;
        g_ncToolManagementStatus.last_line_no = pBlock->line_no;
        (void)NcToolManagement_RequestPrepare(pBlock->tool_no);
    }

    if (((pBlock->aux_flags & NC_AUX_FLAG_M_CODE) != 0U) &&
        (pBlock->aux_m_code == 6U)) {
        toolNo = g_ncToolManagementStatus.prepared_tool_no;
        if ((toolNo == 0U) && ((pBlock->aux_flags & NC_AUX_FLAG_TOOL) != 0U)) {
            toolNo = pBlock->tool_no;
        }
        pocket = (toolNo < NC_TOOL_TABLE_SIZE) ?
                 g_ncToolManagementStatus.tool_to_pocket[toolNo] : 0U;
        g_ncToolManagementStatus.exchange_requests++;
        g_ncToolManagementStatus.last_m_code = pBlock->aux_m_code;
        g_ncToolManagementStatus.last_line_no = pBlock->line_no;
        if ((IsValidTool(toolNo) == 0U) || (pocket == 0U)) {
            g_ncToolManagementStatus.invalid_tool_events++;
            NcEvent_Push(NC_EVENT_CODE_TOOL_MANAGEMENT_WARN,
                         (int32_t)toolNo,
                         (int32_t)pocket,
                         pBlock->line_no);
        } else {
            g_ncToolManagementStatus.exchange_in_progress = 1U;
            g_ncToolManagementStatus.mfin_required =
                (uint8_t)(((pBlock->aux_flags & NC_AUX_FLAG_MFIN_WAIT) != 0U) ? 1U : 0U);
            g_ncToolManagementStatus.exchange_wait_cycles = 0U;
            g_ioOut.tool_no = toolNo;
            NcEvent_Push(NC_EVENT_CODE_TOOL_CHANGE_REQUEST,
                         (int32_t)toolNo,
                         (int32_t)pocket,
                         pBlock->line_no);
            if (g_ncToolManagementStatus.mfin_required == 0U) {
                g_ncToolManagementStatus.active_tool_no = toolNo;
                g_ncToolManagementStatus.exchange_completed++;
                g_ncToolManagementStatus.exchange_in_progress = 0U;
            }
        }
        g_ncToolManagementStatus.generation++;
    }
}

/**
 * @brief Handle nc tool management service rt for this module.
 */
void NcToolManagement_ServiceRt(void)
{
    if (g_ncToolManagementStatus.exchange_in_progress == 0U) {
        return;
    }

    if ((g_ioIn.seq_bits & SENSOR_SEQ_MFIN_BIT) != 0U) {
        uint32_t toolNo = g_ncToolManagementStatus.prepared_tool_no;
        uint32_t pocket = (toolNo < NC_TOOL_TABLE_SIZE) ?
                          g_ncToolManagementStatus.tool_to_pocket[toolNo] : 0U;
        g_ncToolManagementStatus.active_tool_no = toolNo;
        g_ncToolManagementStatus.exchange_completed++;
        if (g_ncToolManagementStatus.exchange_wait_cycles >
            g_ncToolManagementStatus.max_exchange_wait_cycles) {
            g_ncToolManagementStatus.max_exchange_wait_cycles =
                g_ncToolManagementStatus.exchange_wait_cycles;
        }
        g_ncToolManagementStatus.exchange_in_progress = 0U;
        g_ncToolManagementStatus.mfin_required = 0U;
        g_ioOut.tool_no = toolNo;
        NcEvent_Push(NC_EVENT_CODE_TOOL_CHANGE_DONE,
                     (int32_t)toolNo,
                     (int32_t)pocket,
                     g_ncToolManagementStatus.last_line_no);
    } else {
        g_ncToolManagementStatus.exchange_wait_cycles++;
        if (g_ncToolManagementStatus.exchange_wait_cycles > NC_TOOL_CHANGE_TIMEOUT_TICKS) {
            g_ncToolManagementStatus.timeout_events++;
            g_ncToolManagementStatus.exchange_in_progress = 0U;
            NcEvent_Push(NC_EVENT_CODE_TOOL_MANAGEMENT_WARN,
                         (int32_t)g_ncToolManagementStatus.prepared_tool_no,
                         (int32_t)g_ncToolManagementStatus.exchange_wait_cycles,
                         g_ncToolManagementStatus.last_line_no);
        }
    }
    g_ncToolManagementStatus.generation++;
}
