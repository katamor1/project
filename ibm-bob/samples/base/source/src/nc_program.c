/* ibm-bob/samples/base/source/src/nc_program.c */
/* Implements NC shared state, ring buffer, TS prefetch, and RT consumption. */
/* This exists so NC program operation stays fixed-size and RT-safe. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_program.h, ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/src/rt_control.c */
#include <stdio.h>
#include <string.h>
#include "nc_interpolation.h"
#include "nc_diagnostics.h"
#include "nc_compensation.h"
#include "nc_kinematics.h"
#include "nc_lookahead.h"
#include "nc_capability.h"
#include "nc_interference.h"
#include "nc_safety_motion.h"
#include "nc_program.h"

#define NC_LOG_BASE        (5000U)
#define CMD_ENABLE_SERVO   (0x00000001UL)
#define CMD_START_MOTION   (0x00000002UL)

static FILE* s_ncFile = NULL;

static void RtNcProgram_ResetAuxWait(void);

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
    RtNcProgram_ResetAuxWait();
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
                     (pBlock->motion_type == NC_MOTION_ARC_CCW) ||
                     (pBlock->motion_type == NC_MOTION_CANNED_CYCLE) ||
                     (pBlock->motion_type == NC_MOTION_THREAD) ||
                     (pBlock->motion_type == NC_MOTION_SKIP) ||
                     (pBlock->motion_type == NC_MOTION_REFERENCE_RETURN) ||
                     (pBlock->motion_type == NC_MOTION_ADVANCED_INTERP));
}

static void RtNcProgram_ResetAuxWait(void)
{
    g_ncAuxStatus.state = NC_AUX_STATE_IDLE;
    g_ncAuxStatus.active_m_code = 0U;
    g_ncAuxStatus.wait_line_no = 0U;
    g_ncAuxStatus.wait_cycles = 0U;
    g_ncAuxStatus.generation++;
}

static uint8_t RtNcProgram_ServiceAuxWait(void)
{
    if (g_ncAuxStatus.state != NC_AUX_STATE_WAIT_MFIN) {
        return 0U;
    }
    if ((g_ioIn.seq_bits & SENSOR_SEQ_MFIN_BIT) != 0U) {
        g_ncAuxStatus.last_wait_cycles = g_ncAuxStatus.wait_cycles;
        g_ncAuxStatus.total_wait_cycles += g_ncAuxStatus.wait_cycles;
        g_ncAuxStatus.completed_m_codes++;
        RtNcProgram_ResetAuxWait();
        return 0U;
    }
    g_ncAuxStatus.wait_cycles++;
    g_ncAuxStatus.generation++;
    return 1U;
}

static void RtNcProgram_UpdateFeatureStatus(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t flags = pBlock->feature_flags;

    if ((flags == 0U) && (pBlock->g_code10 == 0U) &&
        (pBlock->cycle_code10 == 0U)) {
        return;
    }

    g_ncFeatureStatus.last_block_feature_flags = flags;
    g_ncFeatureStatus.last_g_code10 = pBlock->g_code10;
    g_ncFeatureStatus.last_cycle_code10 = pBlock->cycle_code10;
    g_ncFeatureStatus.last_line_no = pBlock->line_no;
    g_ncFeatureStatus.executed_feature_blocks++;
    g_ncFeatureStatus.return_mode = pBlock->return_mode;

    if (pBlock->g_code10 == (uint16_t)NC_G_CODE_WHOLE(8)) {
        if (pBlock->p_word == 0U) {
            g_ncFeatureStatus.g08_cancel_blocks++;
            g_ncFeatureStatus.smoothing_enabled = 0U;
            g_ncFeatureStatus.high_precision_enabled = 0U;
            g_ncFeatureStatus.active_feature_flags &=
                ~(NC_FEATURE_FLAG_SMOOTHING | NC_FEATURE_FLAG_HIGH_PRECISION);
        } else if (pBlock->p_word == 1U) {
            g_ncFeatureStatus.g08_standard_blocks++;
            g_ncFeatureStatus.smoothing_enabled = 1U;
            g_ncFeatureStatus.high_precision_enabled = 0U;
            g_ncFeatureStatus.active_feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
            g_ncFeatureStatus.active_feature_flags &= ~NC_FEATURE_FLAG_HIGH_PRECISION;
        } else if (pBlock->p_word == 2U) {
            g_ncFeatureStatus.g08_high_precision_blocks++;
            g_ncFeatureStatus.smoothing_enabled = 1U;
            g_ncFeatureStatus.high_precision_enabled = 1U;
            g_ncFeatureStatus.active_feature_flags |=
                (NC_FEATURE_FLAG_SMOOTHING | NC_FEATURE_FLAG_HIGH_PRECISION);
        }
    }
    if (pBlock->g_code10 == (uint16_t)NC_G_CODE_WHOLE(5)) {
        if (pBlock->p_word == 10000U) {
            g_ncFeatureStatus.hpcc_blocks++;
            g_ncFeatureStatus.hpcc_enabled = 1U;
            g_ncFeatureStatus.smoothing_enabled = 1U;
            g_ncFeatureStatus.high_precision_enabled = 1U;
            g_ncFeatureStatus.active_feature_flags |=
                (NC_FEATURE_FLAG_HPCC | NC_FEATURE_FLAG_SMOOTHING |
                 NC_FEATURE_FLAG_HIGH_PRECISION);
        } else if (pBlock->p_word == 0U) {
            g_ncFeatureStatus.hpcc_cancel_blocks++;
            g_ncFeatureStatus.hpcc_enabled = 0U;
            g_ncFeatureStatus.high_speed_mode_enabled = 0U;
            g_ncFeatureStatus.smoothing_enabled = 0U;
            g_ncFeatureStatus.high_precision_enabled = 0U;
            g_ncFeatureStatus.active_feature_flags &=
                ~(NC_FEATURE_FLAG_HPCC | NC_FEATURE_FLAG_HIGH_SPEED_MODE |
                  NC_FEATURE_FLAG_SMOOTHING | NC_FEATURE_FLAG_HIGH_PRECISION);
        } else if (pBlock->p_word == 3U) {
            g_ncFeatureStatus.g05_p3_blocks++;
            g_ncFeatureStatus.high_speed_mode_enabled = 1U;
            g_ncFeatureStatus.active_feature_flags |=
                NC_FEATURE_FLAG_HIGH_SPEED_MODE;
        }
    }

    if ((flags & NC_FEATURE_FLAG_CANNED_CYCLE) != 0U) {
        g_ncFeatureStatus.canned_cycle_blocks++;
        g_ncFeatureStatus.active_feature_flags |= NC_FEATURE_FLAG_CANNED_CYCLE;
    }
    if ((flags & NC_FEATURE_FLAG_THREAD) != 0U) {
        g_ncFeatureStatus.thread_blocks++;
        g_ncFeatureStatus.active_feature_flags |= NC_FEATURE_FLAG_THREAD;
    }
    if ((flags & NC_FEATURE_FLAG_SKIP) != 0U) {
        g_ncFeatureStatus.skip_blocks++;
    }
    if ((flags & NC_FEATURE_FLAG_REFERENCE_RETURN) != 0U) {
        g_ncFeatureStatus.reference_return_blocks++;
    }
    if ((flags & NC_FEATURE_FLAG_ADVANCED_INTERP) != 0U) {
        g_ncFeatureStatus.advanced_interp_blocks++;
    }
    if ((flags & NC_FEATURE_FLAG_TOOL_LENGTH) != 0U) {
        g_ncFeatureStatus.active_feature_flags |= NC_FEATURE_FLAG_TOOL_LENGTH;
    }
    if ((flags & NC_FEATURE_FLAG_CUTTER_RADIUS) != 0U) {
        g_ncFeatureStatus.active_feature_flags |= NC_FEATURE_FLAG_CUTTER_RADIUS;
    }
    if ((flags & NC_FEATURE_FLAG_COORD_ROTATION) != 0U) {
        g_ncFeatureStatus.active_feature_flags |= NC_FEATURE_FLAG_COORD_ROTATION;
    }
    if ((flags & NC_FEATURE_FLAG_POLAR_COORD) != 0U) {
        g_ncFeatureStatus.active_feature_flags |= NC_FEATURE_FLAG_POLAR_COORD;
    }
    if ((flags & NC_FEATURE_FLAG_HIGH_PRECISION) != 0U) {
        g_ncFeatureStatus.high_precision_enabled = 1U;
        g_ncFeatureStatus.active_feature_flags |= NC_FEATURE_FLAG_HIGH_PRECISION;
    }
    if ((flags & NC_FEATURE_FLAG_SMOOTHING) != 0U) {
        g_ncFeatureStatus.smoothing_enabled = 1U;
        g_ncFeatureStatus.active_feature_flags |= NC_FEATURE_FLAG_SMOOTHING;
    }
    if ((flags & NC_FEATURE_FLAG_HPCC) != 0U) {
        g_ncFeatureStatus.hpcc_enabled = 1U;
        g_ncFeatureStatus.active_feature_flags |= NC_FEATURE_FLAG_HPCC;
    }
    if ((flags & NC_FEATURE_FLAG_HIGH_SPEED_MODE) != 0U) {
        g_ncFeatureStatus.high_speed_mode_enabled = 1U;
        g_ncFeatureStatus.active_feature_flags |= NC_FEATURE_FLAG_HIGH_SPEED_MODE;
    }
    if ((flags & NC_FEATURE_FLAG_EXACT_STOP) != 0U) {
        g_ncFeatureStatus.exact_stop_enabled = 1U;
        g_ncFeatureStatus.active_feature_flags |= NC_FEATURE_FLAG_EXACT_STOP;
    }
    if ((flags & NC_FEATURE_FLAG_SYNC) != 0U) {
        g_ncFeatureStatus.sync_enabled =
            (uint8_t)(pBlock->g_code10 == NC_G_CODE10(50, 4) ? 0U : 1U);
    }
    if ((flags & NC_FEATURE_FLAG_COMPOUND) != 0U) {
        g_ncFeatureStatus.compound_enabled =
            (uint8_t)(pBlock->g_code10 == NC_G_CODE10(50, 5) ? 0U : 1U);
    }
    if ((flags & NC_FEATURE_FLAG_OVERLAY) != 0U) {
        g_ncFeatureStatus.overlay_enabled =
            (uint8_t)(pBlock->g_code10 == NC_G_CODE10(50, 6) ? 0U : 1U);
    }
    if (pBlock->g_code10 == NC_G_CODE_WHOLE(64)) {
        g_ncFeatureStatus.exact_stop_enabled = 0U;
        g_ncFeatureStatus.active_feature_flags &= ~NC_FEATURE_FLAG_EXACT_STOP;
    }
    if (pBlock->g_code10 == NC_G_CODE_WHOLE(80)) {
        g_ncFeatureStatus.active_feature_flags &= ~NC_FEATURE_FLAG_CANNED_CYCLE;
    }
    if (pBlock->tool_length_command == NC_TOOL_LENGTH_CMD_CANCEL) {
        g_ncFeatureStatus.active_feature_flags &= ~NC_FEATURE_FLAG_TOOL_LENGTH;
    }
    if (pBlock->cutter_comp_command == NC_CUTTER_COMP_CMD_CANCEL) {
        g_ncFeatureStatus.active_feature_flags &= ~NC_FEATURE_FLAG_CUTTER_RADIUS;
    }
    if (pBlock->rotation_command == NC_ROTATION_CMD_CANCEL) {
        g_ncFeatureStatus.active_feature_flags &= ~NC_FEATURE_FLAG_COORD_ROTATION;
    }
    if (pBlock->polar_command == NC_POLAR_CMD_CANCEL) {
        g_ncFeatureStatus.active_feature_flags &= ~NC_FEATURE_FLAG_POLAR_COORD;
    }
    g_ncFeatureStatus.generation++;
}

static uint8_t RtNcProgram_IsActiveBlock(const NC_EXEC_BLOCK* pBlock)
{
    return (uint8_t)((RtNcProgram_IsMotionBlock(pBlock) != 0U) ||
                     (pBlock->motion_type == NC_MOTION_DWELL));
}

static uint8_t RtNcProgram_IsActiveToolExpired(void)
{
    uint32_t toolNo = g_ncToolLifeStatus.active_tool_no;
    if ((toolNo == 0U) || (toolNo >= NC_TOOL_LIFE_TABLE_SIZE)) {
        return 0U;
    }
    return (uint8_t)((g_ncToolLifeStatus.expired_mask & (1UL << toolNo)) != 0U);
}

static void RtNcProgram_HoldForToolLife(uint32_t lineNo)
{
    g_ncProgramStatus.error_code = NC_ERR_TOOL_LIFE_EXPIRED;
    g_ncProgramStatus.error_line_no = lineNo;
    g_ncProgramStatus.response_code = RESPONSE_REJECTED;
    NcInterpolation_Cancel();
    RtNcProgram_ResetAuxWait();
    NcStatus_SetState(NC_HOLD);
    NcEvent_Push(NC_EVENT_CODE_TOOL_LIFE_EXPIRED_HOLD,
                 (int32_t)g_ncToolLifeStatus.active_tool_no,
                 (int32_t)g_ncToolLifeStatus.tool_use_count[g_ncToolLifeStatus.active_tool_no],
                 lineNo);
    (void)LogQueue_Push(LOG_WARN, NC_EVENT_CODE_TOOL_LIFE_EXPIRED_HOLD,
                        (int32_t)g_ncToolLifeStatus.active_tool_no);
}

static void RtNcProgram_ApplyBlockAux(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t i;

    g_ncProgramStatus.exec_line_no = pBlock->line_no;
    RtNcProgram_UpdateFeatureStatus(pBlock);
    NcDiagnostics_OnBlockRt(pBlock);
    if (RtNcProgram_IsActiveToolExpired() != 0U) {
        RtNcProgram_HoldForToolLife(pBlock->line_no);
        return;
    }
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
    if ((pBlock->aux_flags & NC_AUX_FLAG_MFIN_WAIT) != 0U) {
        g_ncAuxStatus.state = NC_AUX_STATE_WAIT_MFIN;
        g_ncAuxStatus.active_m_code = pBlock->aux_m_code;
        g_ncAuxStatus.wait_line_no = pBlock->line_no;
        g_ncAuxStatus.wait_cycles = 0U;
        g_ncAuxStatus.generation++;
    }
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
    NC_EXEC_BLOCK workBlock;

    (void)memcpy(&workBlock, pBlock, sizeof(workBlock));
    NcSafetyMotion_ApplyBlockRt(&workBlock);

    if (workBlock.motion_type != NC_MOTION_DWELL) {
        RtNcProgram_ApplyBlockAux(&workBlock);
        if (g_ncProgramStatus.state != NC_RUNNING) {
            return;
        }
    } else {
        g_ncProgramStatus.exec_line_no = workBlock.line_no;
    }

    beginResult = NcInterpolation_BeginBlockRt(&workBlock);
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
        RtNcProgram_ResetAuxWait();
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
    if (RtNcProgram_IsActiveToolExpired() != 0U) {
        RtNcProgram_HoldForToolLife(g_ncProgramStatus.exec_line_no);
        g_ncProgramStatus.response_code = RESPONSE_REJECTED;
    } else if ((g_ncProgramStatus.state == NC_READY) &&
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
    if (RtNcProgram_ServiceAuxWait() != 0U) {
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
        if (g_ncAuxStatus.state == NC_AUX_STATE_WAIT_MFIN) {
            return;
        }
        consumed++;
    }
    RtNcProgram_FinishIfComplete();
}

int32_t NcProgram_RequestBinaryLoad(const NC_EXEC_BLOCK* pBlocks, uint32_t count)
{
    if ((pBlocks == NULL) || (count == 0U) ||
        (count > NC_BINARY_PROGRAM_MAX_BLOCKS)) {
        return -1;
    }
    (void)memcpy((void*)g_ncBinaryProgramRequest.blocks,
                 pBlocks,
                 sizeof(NC_EXEC_BLOCK) * count);
    g_ncBinaryProgramRequest.requested_count = count;
    g_ncBinaryProgramRequest.accepted_count = count;
    g_ncBinaryProgramRequest.load_request = 1U;
    g_ncBinaryProgramRequest.ready = 0U;
    g_ncBinaryProgramRequest.rejected = 0U;
    g_ncBinaryProgramRequest.generation++;
    g_ncProgramRequest.load_request = 0U;
    g_ncProgramRequest.start_request = 0U;
    g_ncProgramRequest.hold_request = 0U;
    g_ncProgramRequest.stop_request = 0U;
    g_ncProgramRequest.resume_request = 0U;
    g_ncProgramRequest.reset_request = 0U;
    return 0;
}

static void TsNcProgram_ResetForNewLoad(void)
{
    NcProgram_ClearBuffer();
    NcParse_ResetModal();
    NcInterpolation_Reset();
    NcCompensation_Reset();
    NcKinematics_Reset();
    NcLookahead_Reset();
    NcCapability_Reset();
    NcInterference_Reset();
    (void)memset((void*)&g_ncAuxStatus, 0, sizeof(g_ncAuxStatus));
    RtNcProgram_ResetAuxWait();
    (void)memset((void*)&g_ncFeatureStatus, 0, sizeof(g_ncFeatureStatus));
    g_ncProgramStatus.error_code = NC_ERR_NONE;
    g_ncProgramStatus.error_line_no = 0U;
    g_ncProgramStatus.read_line_no = 0U;
    g_ncProgramStatus.exec_line_no = 0U;
    g_ncBinaryProgramStatus.committed_blocks = 0U;
    g_ncBinaryProgramStatus.last_error_line_no = 0U;
}

static void TsNcProgram_StartLoad(void)
{
    char path[NC_FILE_PATH_MAX];

    (void)memcpy(path, (const void*)g_ncProgramRequest.file_path, sizeof(path));
    path[NC_FILE_PATH_MAX - 1U] = '\0';
    g_ncProgramRequest.load_request = 0U;
    TsNcProgram_ResetForNewLoad();
    g_ncBinaryProgramStatus.active = 0U;
    g_ncBinaryProgramStatus.source_ready = 0U;
    NcStatus_SetState(NC_LOAD_REQUESTED);

    s_ncFile = fopen(path, "r");
    if (s_ncFile == NULL) {
        NcStatus_SetError(NC_ERR_FILE_OPEN, 0U);
        return;
    }

    NcStatus_SetState(NC_PREFETCHING);
}

static void TsNcProgram_StartBinaryLoad(void)
{
    uint32_t i;
    uint32_t count = g_ncBinaryProgramRequest.requested_count;

    g_ncBinaryProgramRequest.load_request = 0U;
    g_ncBinaryProgramRequest.busy = 1U;
    TsNcProgram_ResetForNewLoad();
    (void)memset((void*)&g_ncBinaryProgramStatus, 0, sizeof(g_ncBinaryProgramStatus));
    NcStatus_SetState(NC_LOAD_REQUESTED);

    if ((count == 0U) || (count > NC_BINARY_PROGRAM_MAX_BLOCKS)) {
        g_ncBinaryProgramRequest.rejected = 1U;
        g_ncBinaryProgramRequest.busy = 0U;
        g_ncBinaryProgramStatus.rejected_blocks = count;
        g_ncBinaryProgramStatus.generation++;
        NcStatus_SetError(NC_ERR_RANGE, 0U);
        return;
    }

    for (i = 0U; i < count; ++i) {
        NC_EXEC_BLOCK block;
        (void)memcpy(&block,
                     (const void*)&g_ncBinaryProgramRequest.blocks[i],
                     sizeof(block));
        block.valid = 1U;
        block.generation = g_ncProgramBuffer.producer_generation + 1U;
        if (block.line_no == 0U) {
            block.line_no = i + 1U;
        }
        NcCapability_OnParsedBlockTs(&block);
        NcInterference_CheckBlockTs(&block);
        if (NcBuffer_CommitBlock(&block) != 0) {
            g_ncBinaryProgramStatus.last_error_line_no = block.line_no;
            NcStatus_SetError(NC_ERR_BUFFER_UNDERRUN, block.line_no);
            break;
        }
        g_ncBinaryProgramStatus.committed_blocks++;
    }

    g_ncProgramStatus.read_line_no = count;
    g_ncProgramBuffer.eof_reached = 1U;
    g_ncBinaryProgramStatus.loaded_blocks = count;
    g_ncBinaryProgramStatus.active = 1U;
    g_ncBinaryProgramStatus.source_ready = 1U;
    g_ncBinaryProgramStatus.generation++;
    g_ncBinaryProgramRequest.ready = 1U;
    g_ncBinaryProgramRequest.busy = 0U;
    NcEvent_Push(NC_EVENT_CODE_BINARY_LOAD_READY,
                 (int32_t)g_ncBinaryProgramStatus.committed_blocks,
                 (int32_t)count,
                 0U);
    if (g_ncProgramStatus.state != NC_ERROR) {
        NcStatus_SetState((NcBuffer_Count() > 0U) ? NC_READY : NC_COMPLETED);
    }
}

void TsNcProgram_ExecuteSlice(void)
{
    uint32_t parsedLines = 0U;

    if (g_ncBinaryProgramRequest.load_request != 0U) {
        TsNcProgram_CloseFile();
        TsNcProgram_StartBinaryLoad();
        return;
    }
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
        if (parseResult == 0) {
            NcCapability_OnParsedBlockTs(&block);
            NcLookahead_ApplyBlockTs(&block);
            NcInterference_CheckBlockTs(&block);
        }
        if ((parseResult == 0) && (NcBuffer_CommitBlock(&block) != 0)) {
            return;
        }
    }
}
