/* ibm-bob/samples/base/source/src/nc_capability.c */
/* Implements NC catalog/coverage status used by G-code, M-code, and address-code design docs. */
#include <string.h>
#include "nc_capability.h"
#include "nc_program.h"

static uint32_t Popcount32(uint32_t value)
{
    uint32_t count = 0U;
    while (value != 0U) {
        count += (value & 1U);
        value >>= 1U;
    }
    return count;
}

static uint32_t AxisMaskToAddressMask(uint32_t axisMask)
{
    uint32_t mask = 0U;
    if ((axisMask & 0x01UL) != 0U) { mask |= (1UL << ('X' - 'A')); }
    if ((axisMask & 0x02UL) != 0U) { mask |= (1UL << ('Y' - 'A')); }
    if ((axisMask & 0x04UL) != 0U) { mask |= (1UL << ('Z' - 'A')); }
    if ((axisMask & 0x08UL) != 0U) { mask |= (1UL << ('A' - 'A')); }
    return mask;
}

void NcCapability_Reset(void)
{
    (void)memset((void*)&g_ncCapabilityStatus, 0, sizeof(g_ncCapabilityStatus));
    g_ncCapabilityStatus.generation++;
}

void NcCapability_RecordUnsupported(uint8_t kind, uint32_t code)
{
    if (kind == NC_CAPABILITY_KIND_G) {
        g_ncCapabilityStatus.unsupported_g_events++;
        g_ncCapabilityStatus.last_g_code10 = code;
    } else if (kind == NC_CAPABILITY_KIND_M) {
        g_ncCapabilityStatus.unsupported_m_events++;
        g_ncCapabilityStatus.last_m_code = code;
    } else if (kind == NC_CAPABILITY_KIND_ADDRESS) {
        g_ncCapabilityStatus.unsupported_address_events++;
        if (code < 26U) {
            g_ncCapabilityStatus.last_address_mask |= (1UL << code);
        }
    }
    NcEvent_Push(NC_EVENT_CODE_CAPABILITY_UNSUPPORTED,
                 (int32_t)kind,
                 (int32_t)code,
                 g_ncProgramStatus.read_line_no);
    g_ncCapabilityStatus.generation++;
}

void NcCapability_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t addrMask;

    if (pBlock == NULL) {
        return;
    }

    g_ncCapabilityStatus.parsed_blocks++;
    addrMask = AxisMaskToAddressMask(pBlock->axis_mask);
    if (pBlock->feed != 0) { addrMask |= (1UL << ('F' - 'A')); }
    if ((pBlock->aux_flags & NC_AUX_FLAG_SPINDLE) != 0U) { addrMask |= (1UL << ('S' - 'A')); }
    if ((pBlock->aux_flags & NC_AUX_FLAG_TOOL) != 0U) { addrMask |= (1UL << ('T' - 'A')); }
    if ((pBlock->aux_flags & NC_AUX_FLAG_M_CODE) != 0U) { addrMask |= (1UL << ('M' - 'A')); }
    if ((pBlock->feature_flags & NC_FEATURE_FLAG_PARAM_P) != 0U) { addrMask |= (1UL << ('P' - 'A')); }
    if ((pBlock->feature_flags & NC_FEATURE_FLAG_PARAM_Q) != 0U) { addrMask |= (1UL << ('Q' - 'A')); }
    if ((pBlock->feature_flags & NC_FEATURE_FLAG_PARAM_R) != 0U) { addrMask |= (1UL << ('R' - 'A')); }
    if (pBlock->h_offset_no != 0U) { addrMask |= (1UL << ('H' - 'A')); }
    if (pBlock->d_offset_no != 0U) { addrMask |= (1UL << ('D' - 'A')); }
    if (pBlock->cycle_repeat != 0U) { addrMask |= (1UL << ('K' - 'A')); }

    g_ncCapabilityStatus.address_words_seen += Popcount32(addrMask);
    g_ncCapabilityStatus.last_address_mask = addrMask;

    if (pBlock->g_code10 != 0U) {
        uint32_t index = g_ncCapabilityStatus.g_history_index % NC_CAPABILITY_G_HISTORY_SIZE;
        g_ncCapabilityStatus.g_history[index] = pBlock->g_code10;
        g_ncCapabilityStatus.g_history_index++;
        g_ncCapabilityStatus.last_g_code10 = pBlock->g_code10;
        g_ncCapabilityStatus.supported_g_blocks++;
    }
    if ((pBlock->aux_flags & NC_AUX_FLAG_M_CODE) != 0U) {
        uint32_t index = g_ncCapabilityStatus.m_history_index % NC_CAPABILITY_M_HISTORY_SIZE;
        g_ncCapabilityStatus.m_history[index] = pBlock->aux_m_code;
        g_ncCapabilityStatus.m_history_index++;
        g_ncCapabilityStatus.last_m_code = pBlock->aux_m_code;
        g_ncCapabilityStatus.supported_m_blocks++;
    }
    g_ncCapabilityStatus.generation++;
}
