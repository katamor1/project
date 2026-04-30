/* ibm-bob/samples/base/source/src/nc_buffer.c */
/* Owns the fixed-size NC execution ring buffer shared by TS and RT. */
/* This exists so nc_program.c stays focused on state and thread flow. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_program.h, ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/inc/system_shared.h */
#include <string.h>
#include "nc_program.h"

/**
 * @brief Return the current NC buffer count.
 * @return Current count, mask, or generated value.
 */
uint32_t NcBuffer_Count(void)
{
    uint32_t head = g_ncProgramBuffer.head_index;
    uint32_t tail = g_ncProgramBuffer.tail_index;

    if (tail >= head) {
        return tail - head;
    }
    return (NC_MAX_LOOKAHEAD_LINES - head) + tail;
}

/**
 * @brief Clear buffer state.
 */
void NcProgram_ClearBuffer(void)
{
    uint32_t i;

    for (i = 0U; i < NC_MAX_LOOKAHEAD_LINES; ++i) {
        g_ncProgramBuffer.slots[i].valid = 0U;
    }
    g_ncProgramBuffer.head_index = 0U;
    g_ncProgramBuffer.tail_index = 0U;
    g_ncProgramBuffer.eof_reached = 0U;
    g_ncProgramStatus.buffered_blocks = 0U;
}

/**
 * @brief Return whether full is true for the current block or state.
 * @return Non-zero when true or active; zero when false or inactive.
 */
uint8_t NcBuffer_IsFull(void)
{
    uint32_t nextTail =
        (g_ncProgramBuffer.tail_index + 1U) % NC_MAX_LOOKAHEAD_LINES;

    return (uint8_t)(nextTail == g_ncProgramBuffer.head_index);
}

/**
 * @brief Commit block into the active queue or output state.
 * @param pBlock NC execution block to read or update.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcBuffer_CommitBlock(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t index;
    volatile NC_EXEC_BLOCK* pSlot;

    if ((pBlock == 0) || (NcBuffer_IsFull() != 0U)) {
        return -1;
    }

    index = g_ncProgramBuffer.tail_index;
    pSlot = &g_ncProgramBuffer.slots[index];
    pSlot->valid = 0U;
    (void)memcpy((void*)pSlot, pBlock, sizeof(*pBlock));

    g_ncProgramBuffer.producer_generation++;
    pSlot->generation = g_ncProgramBuffer.producer_generation;
    pSlot->valid = 1U;
    g_ncProgramBuffer.tail_index = (index + 1U) % NC_MAX_LOOKAHEAD_LINES;
    g_ncProgramStatus.buffered_blocks = NcBuffer_Count();
    return 0;
}

/**
 * @brief Copy head without consuming it.
 * @param pOutBlock Output pointer receiving block.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcBuffer_PeekHead(NC_EXEC_BLOCK* pOutBlock)
{
    const volatile NC_EXEC_BLOCK* pSlot;

    if ((pOutBlock == 0) || (NcBuffer_Count() == 0U)) {
        return -1;
    }

    pSlot = &g_ncProgramBuffer.slots[g_ncProgramBuffer.head_index];
    if (pSlot->valid == 0U) {
        return -1;
    }

    (void)memcpy(pOutBlock, (const void*)pSlot, sizeof(*pOutBlock));
    return 0;
}

/**
 * @brief Consume head from the active queue.
 */
void NcBuffer_ConsumeHead(void)
{
    if (NcBuffer_Count() == 0U) {
        return;
    }

    g_ncProgramBuffer.slots[g_ncProgramBuffer.head_index].valid = 0U;
    g_ncProgramBuffer.head_index =
        (g_ncProgramBuffer.head_index + 1U) % NC_MAX_LOOKAHEAD_LINES;
    g_ncProgramBuffer.consumer_generation++;
    g_ncProgramStatus.buffered_blocks = NcBuffer_Count();
}
