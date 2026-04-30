/* ibm-bob/samples/base/source/inc/nc_parser_internal.h */
/* Shares small parser context helpers between NC parser translation units. */
/* This exists so parser files stay under the repository line limit. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/src/nc_parser_modal.c, ibm-bob/samples/base/source/inc/nc_program.h */
#ifndef NC_PARSER_INTERNAL_H
#define NC_PARSER_INTERNAL_H

#include "system_shared.h"

/**
 * @brief Enumeration for NC parse coord mode.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_PARSE_COORD_NONE = 0, /**< Definition value for NC parse coord none. */
    NC_PARSE_COORD_LOCAL_SHIFT = 1, /**< Definition value for NC parse coord local shift. */
    NC_PARSE_COORD_WORK_SELECT = 2, /**< Definition value for NC parse coord work select. */
    NC_PARSE_COORD_G10_WORK_OFFSET = 3, /**< Definition value for NC parse coord g10 work offset. */
    NC_PARSE_COORD_MACHINE_DIRECT = 4, /**< Definition value for NC parse coord machine direct. */
    NC_PARSE_COORD_TEMPORARY_SHIFT = 5, /**< Definition value for NC parse coord temporary shift. */
    NC_PARSE_COORD_ROTATION_CENTER = 6, /**< Definition value for NC parse coord rotation center. */
    NC_PARSE_COORD_TILTED_PLANE = 7, /**< Definition value for NC parse coord tilted plane. */
    NC_PARSE_COORD_TOOL_AXIS = 8, /**< Definition value for NC parse coord tool axis. */
    NC_PARSE_COORD_MIRROR_SETTING = 9 /**< Definition value for NC parse coord mirror setting. */
} NC_PARSE_COORD_MODE;

/**
 * @brief Structure for NC parse context.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    NC_PARSE_COORD_MODE coord_mode;
    uint8_t seen_groups;
    uint8_t g10_l_valid;
    uint8_t g10_p_valid;
    uint8_t smoothing_seen;
    int32_t g10_l_value;
    int32_t g10_p_value;
} NC_PARSE_CONTEXT;

/**
 * @brief Reset NC parser state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcParser_ResetModalState(void);
/**
 * @brief Handle NC parser init block for this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param lineNo NC source line number associated with the operation.
 */
void NcParser_InitBlock(NC_EXEC_BLOCK* pBlock, uint32_t lineNo);
/**
 * @brief Apply G code to the current block or state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param code Code value being applied or recorded.
 * @param pBlock NC execution block to read or update.
 * @param pCtx Pointer to ctx used by this operation.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcParser_ApplyGCode(int32_t code,
                            NC_EXEC_BLOCK* pBlock,
                            NC_PARSE_CONTEXT* pCtx);
/**
 * @brief Apply feature G to the current block or state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param code Code value being applied or recorded.
 * @param pBlock NC execution block to read or update.
 * @param pCtx Pointer to ctx used by this operation.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcParser_ApplyFeatureG(int32_t code,
                               NC_EXEC_BLOCK* pBlock,
                               NC_PARSE_CONTEXT* pCtx);
/**
 * @brief Finalize feature block before it leaves this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcParser_FinalizeFeatureBlock(NC_EXEC_BLOCK* pBlock,
                                      NC_ERROR_CODE* pOutError);
/**
 * @brief Set feed in NC parser.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param feed Input value for feed.
 * @param pBlock NC execution block to read or update.
 */
void NcParser_SetFeed(int32_t feed, NC_EXEC_BLOCK* pBlock);
/**
 * @brief Set spindle in NC parser.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param spindleSpeed Input value for spindle speed.
 * @param pBlock NC execution block to read or update.
 */
void NcParser_SetSpindle(uint32_t spindleSpeed, NC_EXEC_BLOCK* pBlock);
/**
 * @brief Apply token to the current block or state.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param address Input value for address.
 * @param value Numeric value used by this operation.
 * @param pBlock NC execution block to read or update.
 * @param pCtx Pointer to ctx used by this operation.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcParser_ApplyToken(char address,
                            double value,
                            NC_EXEC_BLOCK* pBlock,
                            NC_PARSE_CONTEXT* pCtx);
/**
 * @brief Finalize block before it leaves this module.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 * @param pOutError Output pointer receiving the NC error code.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcParser_FinalizeBlock(NC_EXEC_BLOCK* pBlock,
                               NC_ERROR_CODE* pOutError);

#endif /* NC_PARSER_INTERNAL_H */
