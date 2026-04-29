/* ibm-bob/samples/base/source/src/nc_program.c */
/* Implements NC shared state, ring buffer, TS prefetch, and RT consumption. */
/* This exists so NC program operation stays fixed-size and RT-safe. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_program.h, ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/src/rt_control.c */
#include <stdio.h>
#include <string.h>
#include "nc_interpolation.h"
#include "nc_program.h"

#define NC_LOG_BASE        (5000U)
#define CMD_ENABLE_SERVO   (0x00000001UL)
#define CMD_START_MOTION   (0x00000002UL)

static FILE* s_ncFile = NULL;

static void NcStatus_SetState(NC_PROGRAM_STATE state)
{
    if (g_ncProgramStatus.state != state) {
        g_ncProgramStatus.state = state;
        g_ncProgramStatus.generation++;
        (void)LogQueue_Push(LOG_AUDIT, NC_LOG_BASE + (uint32_t)state, 0);
    }
}

static void NcStatus_SetError(NC_ERROR_CODE errorCode, uint32_t lineNo)
{
    g_ncProgramStatus.error_code = errorCode;
    g_ncProgramStatus.error_line_no = lineNo;
    g_ncProgramStatus.response_code = RESPONSE_REJECTED;
    NcStatus_SetState(NC_ERROR);
    (void)LogQueue_Push(LOG_ALARM, NC_LOG_BASE + (uint32_t)errorCode, (int32_t)lineNo);
}

static void TsNcProgram_CloseFile(void)
{
    if (s_ncFile != NULL) {
        (void)fclose(s_ncFile);
        s_ncFile = NULL;
    }
}

static uint8_t RtNcProgram_IsMotionBlock(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)((pBlock->motion_type == NC_MOTION_RAPID) ||
                     (pBlock->motion_type == NC_MOTION_LINEAR) ||
                     (pBlock->motion_type == NC_MOTION_ARC_CW) ||
                     (pBlock->motion_type == NC_MOTION_ARC_CCW));
}

static uint8_t RtNcProgram_IsActiveBlock(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)((RtNcProgram_IsMotionBlock(pBlock) != 0U) ||
                     (pBlock->motion_type == NC_MOTION_DWELL));
}

static void RtNcProgram_ApplyBlockAux(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t i;

    g_ncProgramStatus.exec_line_no = pBlock->line_no;
    if ((pBlock->motion_type == NC_MOTION_NONE) &&
        (pBlock->axis_mask == 0U) &&
        (pBlock->aux_flags == 0U)) {
        return;
    }

    g_ioOut.command_bits |= (CMD_ENABLE_SERVO | CMD_START_MOTION);
    if (RtNcProgram_IsMotionBlock(pBlock) == 0U) {
        for (i = 0U; i < AXIS_MAX; ++i) {
            if ((pBlock->axis_mask & (1UL << i)) != 0U) {
                g_ioOut.axis_target[i] = pBlock->axis_target[i];
            }
        }
    }
    if ((pBlock->aux_flags & NC_AUX_FLAG_M_CODE) != 0U) {
        g_ioOut.aux_m_code = pBlock->aux_m_code;
    }
    if ((pBlock->aux_flags & NC_AUX_FLAG_SPINDLE) != 0U) {
        g_ioOut.spindle_speed = pBlock->spindle_speed;
    }
    if ((pBlock->aux_flags & NC_AUX_FLAG_TOOL) != 0U) {
        g_ioOut.tool_no = pBlock->tool_no;
    }
    g_ioOut.aux_flags |= pBlock->aux_flags;
}

static void RtNcProgram_FinishIfComplete(void)
{
    if ((NcBuffer_Count() == 0U) &&
        (g_ncProgramBuffer.eof_reached != 0U) &&
        (NcInterpolation_HasActive() == 0U)) {
        NcStatus_SetState(NC_COMPLETED);
        g_machineCtx.machine_state = MACHINE_READY;
    }
}

static void RtNcProgram_StartMotionBlock(const NC_EXEC_BLOCK* pBlock)
{
    int32_t beginResult;

    if (pBlock->motion_type != NC_MOTION_DWELL) {
        RtNcProgram_ApplyBlockAux(pBlock);
    } else {
        g_ncProgramStatus.exec_line_no = pBlock->line_no;
    }

    beginResult = NcInterpolation_BeginBlockRt(pBlock);
    if (beginResult == 0) {
        (void)NcInterpolation_StepRt();
        RtNcProgram_FinishIfComplete();
    } else if (beginResult == -2) {
        NcStatus_SetState(NC_HOLD);
    } else {
        NcStatus_SetError(NC_ERR_GENERATION, pBlock->line_no);
    }
}

void RtNcProgram_ProcessRequests(uint8_t safetyOk)
{
    if (g_machineCtx.machine_state == MACHINE_ALARM) {
        if ((g_ncProgramStatus.state == NC_RUNNING) ||
            (g_ncProgramStatus.state == NC_HOLD)) {
            NcInterpolation_Cancel();
            NcStatus_SetError(NC_ERR_INTERLOCK, g_ncProgramStatus.exec_line_no);
        }
        return;
    }

    if (g_ncProgramRequest.stop_request != 0U) {
        g_ncProgramRequest.stop_request = 0U;
        NcInterpolation_Cancel();
        NcStatus_SetState(NC_COMPLETED);
        g_ncProgramStatus.response_code = RESPONSE_COMPLETED;
        return;
    }
    if ((g_ncProgramRequest.hold_request != 0U) &&
        (g_ncProgramStatus.state == NC_RUNNING)) {
        g_ncProgramRequest.hold_request = 0U;
        NcStatus_SetState(NC_HOLD);
        g_ncProgramStatus.response_code = RESPONSE_COMPLETED;
        return;
    }
    if ((g_ncProgramRequest.resume_request != 0U) &&
        (g_ncProgramStatus.state == NC_HOLD) && (safetyOk != 0U)) {
        g_ncProgramRequest.resume_request = 0U;
        NcStatus_SetState(NC_RUNNING);
        g_ncProgramStatus.response_code = RESPONSE_COMPLETED;
        return;
    }
    if (g_ncProgramRequest.start_request == 0U) {
        return;
    }

    g_ncProgramRequest.start_request = 0U;
    if ((g_ncProgramStatus.state == NC_READY) &&
        (g_machineCtx.run_mode == RUN_MODE_AUTO) && (safetyOk != 0U)) {
        g_machineCtx.machine_state = MACHINE_RUNNING;
        NcStatus_SetState(NC_RUNNING);
        g_ncProgramStatus.response_code = RESPONSE_COMPLETED;
    } else {
        g_ncProgramStatus.response_code = RESPONSE_REJECTED;
    }
}

void RtNcProgram_ConsumeBlocks(void)
{
    uint32_t consumed = 0U;

    if (g_ncProgramStatus.state != NC_RUNNING) {
        return;
    }
    if (NcInterpolation_HasActive() != 0U) {
        (void)NcInterpolation_StepRt();
        RtNcProgram_FinishIfComplete();
        return;
    }

    while (consumed < NC_RT_MAX_BLOCKS_PER_CYCLE) {
        NC_EXEC_BLOCK block;

        if (NcBuffer_Count() == 0U) {
            if (g_ncProgramBuffer.eof_reached != 0U) {
                RtNcProgram_FinishIfComplete();
            } else {
                g_ncProgramStatus.error_code = NC_ERR_BUFFER_UNDERRUN;
                NcStatus_SetState(NC_HOLD);
            }
            return;
        }
        if (NcBuffer_PeekHead(&block) != 0) {
            NcStatus_SetError(NC_ERR_GENERATION, g_ncProgramStatus.exec_line_no);
            return;
        }

        NcBuffer_ConsumeHead();
        if (RtNcProgram_IsActiveBlock(&block) != 0U) {
            RtNcProgram_StartMotionBlock(&block);
            return;
        }
        RtNcProgram_ApplyBlockAux(&block);
        consumed++;
    }
    RtNcProgram_FinishIfComplete();
}

static void TsNcProgram_StartLoad(void)
{
    char path[NC_FILE_PATH_MAX];

    (void)memcpy(path, (const void*)g_ncProgramRequest.file_path, sizeof(path));
    path[NC_FILE_PATH_MAX - 1U] = '\0';
    g_ncProgramRequest.load_request = 0U;
    NcProgram_ClearBuffer();
    NcParse_ResetModal();
    NcInterpolation_Reset();
    g_ncProgramStatus.error_code = NC_ERR_NONE;
    g_ncProgramStatus.error_line_no = 0U;
    g_ncProgramStatus.read_line_no = 0U;
    g_ncProgramStatus.exec_line_no = 0U;
    NcStatus_SetState(NC_LOAD_REQUESTED);

    s_ncFile = fopen(path, "r");
    if (s_ncFile == NULL) {
        NcStatus_SetError(NC_ERR_FILE_OPEN, 0U);
        return;
    }

    NcStatus_SetState(NC_PREFETCHING);
}

void TsNcProgram_ExecuteSlice(void)
{
    uint32_t parsedLines = 0U;

    if (g_ncProgramRequest.load_request != 0U) {
        TsNcProgram_CloseFile();
        TsNcProgram_StartLoad();
    }
    if (s_ncFile == NULL) {
        return;
    }

    while ((parsedLines < NC_TS_PARSE_LINES_PER_SLICE) &&
           (NcBuffer_IsFull() == 0U)) {
        char line[NC_LINE_MAX_LEN];
        NC_EXEC_BLOCK block;
        NC_ERROR_CODE parseError;
        int32_t parseResult;

        if (fgets(line, (int)sizeof(line), s_ncFile) == NULL) {
            if (feof(s_ncFile) != 0) {
                g_ncProgramBuffer.eof_reached = 1U;
                TsNcProgram_CloseFile();
                if (g_ncProgramStatus.state == NC_PREFETCHING) {
                    NcStatus_SetState((NcBuffer_Count() > 0U) ? NC_READY : NC_COMPLETED);
                }
                return;
            }
            NcStatus_SetError(NC_ERR_FILE_READ, g_ncProgramStatus.read_line_no);
            TsNcProgram_CloseFile();
            return;
        }

        g_ncProgramStatus.read_line_no++;
        parsedLines++;
        if ((strchr(line, '\n') == NULL) && (feof(s_ncFile) == 0)) {
            NcStatus_SetError(NC_ERR_LINE_TOO_LONG, g_ncProgramStatus.read_line_no);
            TsNcProgram_CloseFile();
            return;
        }

        parseResult = NcParse_LineToBlock(line,
                                          g_ncProgramStatus.read_line_no,
                                          &block,
                                          &parseError);
        if (parseResult < 0) {
            NcStatus_SetError(parseError, g_ncProgramStatus.read_line_no);
            TsNcProgram_CloseFile();
            return;
        }
        if ((parseResult == 0) && (NcBuffer_CommitBlock(&block) != 0)) {
            return;
        }
    }
}
