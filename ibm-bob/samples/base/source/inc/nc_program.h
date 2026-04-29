/* ibm-bob/samples/base/source/inc/nc_program.h */
/* Declares NC parser, buffer, TS, and RT helpers for NC program operation. */
/* This exists so NC logic stays separate from the baseline RT and TS shells. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_program.c, ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/inc/system_shared.h */
#ifndef NC_PROGRAM_H
#define NC_PROGRAM_H

#include <stdint.h>
#include "system_shared.h"

#define NC_AUX_FLAG_M_CODE      (0x00000001UL)
#define NC_AUX_FLAG_SPINDLE     (0x00000002UL)
#define NC_AUX_FLAG_TOOL        (0x00000004UL)
#define NC_MODAL_FLAG_G00       (0x00000001UL)
#define NC_MODAL_FLAG_G01       (0x00000002UL)
#define NC_MODAL_FLAG_G02       (0x00000004UL)
#define NC_MODAL_FLAG_G03       (0x00000008UL)
#define NC_MODAL_FLAG_G17       (0x00000010UL)
#define NC_MODAL_FLAG_G18       (0x00000020UL)
#define NC_MODAL_FLAG_G19       (0x00000040UL)
#define NC_MODAL_FLAG_G94       (0x00000080UL)
#define NC_MODAL_FLAG_G95       (0x00000100UL)
#define NC_MODAL_FLAG_G04       (0x00000200UL)
#define NC_MODAL_FLAG_G09       (0x00000400UL)
#define NC_MODAL_FLAG_G61       (0x00000800UL)
#define NC_MODAL_FLAG_G64       (0x00001000UL)
#define NC_MODAL_FLAG_G10       (0x00002000UL)
#define NC_MODAL_FLAG_G20       (0x00004000UL)
#define NC_MODAL_FLAG_G21       (0x00008000UL)
#define NC_MODAL_FLAG_G53       (0x00040000UL)
#define NC_MODAL_FLAG_G90       (0x00080000UL)
#define NC_MODAL_FLAG_G91       (0x00100000UL)
#define NC_MODAL_FLAG_G92       (0x00200000UL)
#define NC_ARC_FLAG_IJK         (0x00010000UL)
#define NC_ARC_FLAG_R           (0x00020000UL)

void NcProgram_ClearBuffer(void);
uint32_t NcBuffer_Count(void);
uint8_t NcBuffer_IsFull(void);
int32_t NcBuffer_CommitBlock(const NC_EXEC_BLOCK* pBlock);
int32_t NcBuffer_PeekHead(NC_EXEC_BLOCK* pOutBlock);
void NcBuffer_ConsumeHead(void);

void RtNcProgram_ProcessRequests(uint8_t safetyOk);
void RtNcProgram_ConsumeBlocks(void);
void TsNcProgram_ExecuteSlice(void);

void NcParse_ResetModal(void);
int32_t NcParse_LineToBlock(const char* line,
                            uint32_t lineNo,
                            NC_EXEC_BLOCK* pOutBlock,
                            NC_ERROR_CODE* pOutError);

#endif /* NC_PROGRAM_H */
