/* ibm-bob/samples/base/source/inc/nc_cycle.h */
/* Declares fixed canned-cycle parser and RT segment planning helpers. */
/* This exists so fixed-cycle details stay out of interpolation and modal parsing. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_cycle.c, ibm-bob/samples/base/source/src/nc_interpolation.c, ibm-bob/samples/base/source/src/nc_parser_modal.c */
#ifndef NC_CYCLE_H
#define NC_CYCLE_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Structure for NC active segment.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    int32_t  start[AXIS_MAX];
    int32_t  end[AXIS_MAX];
    uint32_t end_tick;
    uint8_t  kind;
} NC_ACTIVE_SEGMENT;

/**
 * @brief Definition value for NC seg kind linear.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SEG_KIND_LINEAR       (0U)
/**
 * @brief Definition value for NC seg kind rapid.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SEG_KIND_RAPID        (1U)
/**
 * @brief Definition value for NC seg kind dwell.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SEG_KIND_DWELL        (2U)
/**
 * @brief Definition value for NC seg kind retract.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_SEG_KIND_RETRACT      (3U)

/**
 * @brief Reset NC cycle parser modal state to its defaults.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcCycle_ResetParserModal(void);
/**
 * @brief Handle NC cycle init parser block for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcCycle_InitParserBlock(NC_EXEC_BLOCK* pBlock);
/**
 * @brief Apply cycle G code to the current block or state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param code Code value being applied or recorded.
 * @param pBlock NC execution block to read or update.
 */
void NcCycle_ApplyCycleGCode(int32_t code, NC_EXEC_BLOCK* pBlock);
/**
 * @brief Cancel active NC cycle state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcCycle_CancelParserModal(void);
/**
 * @brief Set repeat word in NC cycle.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param value Numeric value used by this operation.
 * @param pBlock NC execution block to read or update.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCycle_SetRepeatWord(double value, NC_EXEC_BLOCK* pBlock);
/**
 * @brief Finalize parser block before it leaves this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCycle_FinalizeParserBlock(NC_EXEC_BLOCK* pBlock,
                                    NC_ERROR_CODE* pOutError);
/**
 * @brief Build segment plan RT from current shared state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param pSegments Pointer to segments used by this operation.
 * @param pSegmentCount Pointer to segment count used by this operation.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCycle_BuildSegmentPlanRt(const NC_EXEC_BLOCK* pBlock,
                                   NC_ACTIVE_SEGMENT* pSegments,
                                   uint32_t* pSegmentCount);

#endif /* NC_CYCLE_H */
