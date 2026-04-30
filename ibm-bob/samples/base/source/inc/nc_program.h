/* ibm-bob/samples/base/source/inc/nc_program.h */
/* Declares NC parser, buffer, TS, and RT helpers for NC program operation. */
/* This exists so NC logic stays separate from the baseline RT and TS shells. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_PROGRAM_H
#define NC_PROGRAM_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Definition value for NC aux flag M code.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AUX_FLAG_M_CODE      (0x00000001UL)

/**
 * @brief Definition value for NC G code10.
 * @details Function-like macro kept in the public header so G-code and fixed-point helpers stay lightweight.
 * @param whole Macro argument for whole.
 * @param tenths Macro argument for tenths.
 * @return Expanded macro value.
 */
#define NC_G_CODE10(whole, tenths) ((int32_t)(((whole) * 10) + (tenths)))
/**
 * @brief Definition value for NC G code whole.
 * @details Function-like macro kept in the public header so G-code and fixed-point helpers stay lightweight.
 * @param whole Macro argument for whole.
 * @return Expanded macro value.
 */
#define NC_G_CODE_WHOLE(whole)     NC_G_CODE10((whole), 0)
/**
 * @brief Definition value for NC feature flag canned cycle.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_CANNED_CYCLE      (0x00000001UL)
/**
 * @brief Definition value for NC feature flag thread.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_THREAD            (0x00000002UL)
/**
 * @brief Definition value for NC feature flag skip.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_SKIP              (0x00000004UL)
/**
 * @brief Definition value for NC feature flag reference return.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_REFERENCE_RETURN  (0x00000008UL)
/**
 * @brief Definition value for NC feature flag advanced interp.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_ADVANCED_INTERP   (0x00000010UL)
/**
 * @brief Definition value for NC feature flag high precision.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_HIGH_PRECISION    (0x00000020UL)
/**
 * @brief Definition value for NC feature flag smoothing.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_SMOOTHING         (0x00000040UL)
/**
 * @brief Definition value for NC feature flag sync.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_SYNC              (0x00000080UL)
/**
 * @brief Definition value for NC feature flag compound.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_COMPOUND          (0x00000100UL)
/**
 * @brief Definition value for NC feature flag overlay.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_OVERLAY           (0x00000200UL)
/**
 * @brief Definition value for NC feature flag exact stop.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_EXACT_STOP        (0x00000400UL)
/**
 * @brief Definition value for NC feature flag return initial.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_RETURN_INITIAL    (0x00000800UL)
/**
 * @brief Definition value for NC feature flag return r plane.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_RETURN_R_PLANE    (0x00001000UL)
/**
 * @brief Definition value for NC feature flag param r.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_PARAM_R           (0x00002000UL)
/**
 * @brief Definition value for NC feature flag param q.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_PARAM_Q           (0x00004000UL)
/**
 * @brief Definition value for NC feature flag param p.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_PARAM_P           (0x00008000UL)
/**
 * @brief Definition value for NC feature flag HPCC.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_HPCC              (0x00400000UL)
/**
 * @brief Definition value for NC feature flag high speed mode.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_FEATURE_FLAG_HIGH_SPEED_MODE   (0x00800000UL)
/**
 * @brief Definition value for NC cycle flag modal replay.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CYCLE_FLAG_MODAL_REPLAY        (0x0001U)
/**
 * @brief Definition value for NC cycle flag has k.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CYCLE_FLAG_HAS_K               (0x0002U)
/**
 * @brief Definition value for NC cycle flag tap.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CYCLE_FLAG_TAP                 (0x0004U)
/**
 * @brief Definition value for NC cycle flag left tap.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CYCLE_FLAG_LEFT_TAP            (0x0008U)
/**
 * @brief Definition value for NC cycle flag boring.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CYCLE_FLAG_BORING              (0x0010U)
/**
 * @brief Definition value for NC cycle flag manual return.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CYCLE_FLAG_MANUAL_RETURN       (0x0020U)
/**
 * @brief Definition value for NC aux flag spindle.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AUX_FLAG_SPINDLE     (0x00000002UL)
/**
 * @brief Definition value for NC aux flag tool.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_AUX_FLAG_TOOL        (0x00000004UL)
/**
 * @brief Definition value for NC modal flag g00.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G00       (0x00000001UL)
/**
 * @brief Definition value for NC modal flag g01.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G01       (0x00000002UL)
/**
 * @brief Definition value for NC modal flag g02.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G02       (0x00000004UL)
/**
 * @brief Definition value for NC modal flag g03.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G03       (0x00000008UL)
/**
 * @brief Definition value for NC modal flag g17.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G17       (0x00000010UL)
/**
 * @brief Definition value for NC modal flag g18.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G18       (0x00000020UL)
/**
 * @brief Definition value for NC modal flag g19.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G19       (0x00000040UL)
/**
 * @brief Definition value for NC modal flag g94.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G94       (0x00000080UL)
/**
 * @brief Definition value for NC modal flag g95.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G95       (0x00000100UL)
/**
 * @brief Definition value for NC modal flag g04.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G04       (0x00000200UL)
/**
 * @brief Definition value for NC modal flag g09.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G09       (0x00000400UL)
/**
 * @brief Definition value for NC modal flag g61.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G61       (0x00000800UL)
/**
 * @brief Definition value for NC modal flag g64.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G64       (0x00001000UL)
/**
 * @brief Definition value for NC modal flag g10.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G10       (0x00002000UL)
/**
 * @brief Definition value for NC modal flag g20.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G20       (0x00004000UL)
/**
 * @brief Definition value for NC modal flag g21.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G21       (0x00008000UL)
/**
 * @brief Definition value for NC modal flag g53.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G53       (0x00040000UL)
/**
 * @brief Definition value for NC modal flag g90.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G90       (0x00080000UL)
/**
 * @brief Definition value for NC modal flag g91.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G91       (0x00100000UL)
/**
 * @brief Definition value for NC modal flag g92.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G92       (0x00200000UL)
/**
 * @brief Definition value for NC modal flag g98.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G98       (0x00400000UL)
/**
 * @brief Definition value for NC modal flag g99.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G99       (0x00800000UL)
/**
 * @brief Definition value for NC modal flag g08.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G08       (0x01000000UL)
/**
 * @brief Definition value for NC modal flag g05.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_MODAL_FLAG_G05       (0x02000000UL)
/**
 * @brief Definition value for NC arc flag IJK.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ARC_FLAG_IJK         (0x00010000UL)
/**
 * @brief Definition value for NC arc flag r.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_ARC_FLAG_R           (0x00020000UL)

/**
 * @brief Clear buffer state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcProgram_ClearBuffer(void);
/**
 * @brief Return the current NC buffer count.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @return Current count, mask, or generated value.
 */
uint32_t NcBuffer_Count(void);
/**
 * @brief Return whether full is true for the current block or state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @return Non-zero when true or active; zero when false or inactive.
 */
uint8_t NcBuffer_IsFull(void);
/**
 * @brief Commit block into the active queue or output state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcBuffer_CommitBlock(const NC_EXEC_BLOCK* pBlock);
/**
 * @brief Copy head without consuming it.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pOutBlock Output pointer receiving block.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcBuffer_PeekHead(NC_EXEC_BLOCK* pOutBlock);
/**
 * @brief Consume head from the active queue.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcBuffer_ConsumeHead(void);

/**
 * @brief Process pending requests work.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param safetyOk Input value for safety ok.
 */
void RtNcProgram_ProcessRequests(uint8_t safetyOk);
/**
 * @brief Consume blocks from the active queue.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void RtNcProgram_ConsumeBlocks(void);
/**
 * @brief Run one TS NC program time-share service slice.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void TsNcProgram_ExecuteSlice(void);
/**
 * @brief Handle NC program request binary load for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlocks Array of NC execution blocks supplied by the caller.
 * @param count Number of entries supplied by the caller.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcProgram_RequestBinaryLoad(const NC_EXEC_BLOCK* pBlocks, uint32_t count);

/**
 * @brief Reset NC parse state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcParse_ResetModal(void);
/**
 * @brief Handle NC parse line to block for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param line Input NC source line to parse.
 * @param lineNo NC source line number associated with the operation.
 * @param pOutBlock Output pointer receiving block.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcParse_LineToBlock(const char* line,
                            uint32_t lineNo,
                            NC_EXEC_BLOCK* pOutBlock,
                            NC_ERROR_CODE* pOutError);

#endif /* NC_PROGRAM_H */
