/* ibm-bob/samples/base/source/inc/nc_parser_internal.h */
/* Shares small parser context helpers between NC parser translation units. */
/* This exists so parser files stay under the repository line limit. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/src/nc_parser_modal.c, ibm-bob/samples/base/source/inc/nc_program.h */
#ifndef NC_PARSER_INTERNAL_H
#define NC_PARSER_INTERNAL_H

#include "system_shared.h"

typedef enum {
    NC_PARSE_COORD_NONE = 0,
    NC_PARSE_COORD_LOCAL_SHIFT = 1,
    NC_PARSE_COORD_WORK_SELECT = 2,
    NC_PARSE_COORD_G10_WORK_OFFSET = 3,
    NC_PARSE_COORD_MACHINE_DIRECT = 4,
    NC_PARSE_COORD_TEMPORARY_SHIFT = 5
} NC_PARSE_COORD_MODE;

typedef struct {
    NC_PARSE_COORD_MODE coord_mode;
    uint8_t seen_groups;
    uint8_t g10_l_valid;
    uint8_t g10_p_valid;
    int32_t g10_l_value;
    int32_t g10_p_value;
} NC_PARSE_CONTEXT;

void NcParser_ResetModalState(void);
void NcParser_InitBlock(NC_EXEC_BLOCK* pBlock, uint32_t lineNo);
int32_t NcParser_ApplyGCode(int32_t code,
                            NC_EXEC_BLOCK* pBlock,
                            NC_PARSE_CONTEXT* pCtx);
void NcParser_SetFeed(int32_t feed, NC_EXEC_BLOCK* pBlock);
void NcParser_SetSpindle(uint32_t spindleSpeed, NC_EXEC_BLOCK* pBlock);
int32_t NcParser_ApplyToken(char address,
                            double value,
                            NC_EXEC_BLOCK* pBlock,
                            NC_PARSE_CONTEXT* pCtx);
int32_t NcParser_FinalizeBlock(NC_EXEC_BLOCK* pBlock,
                               NC_ERROR_CODE* pOutError);

#endif /* NC_PARSER_INTERNAL_H */
