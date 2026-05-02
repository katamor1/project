#include <string.h>
#include "system_shared.h"
#include "nc_program.h"
#include "nc_motion_filter.h"

static uint8_t s_interpActive;
static uint32_t s_interpSteps;

void NcInterpolation_Reset(void) { s_interpActive = 0U; s_interpSteps = 0U; }
void NcInterpolation_Cancel(void) { s_interpActive = 0U; }
uint8_t NcInterpolation_HasActive(void) { return s_interpActive; }
int32_t NcInterpolation_BeginBlockRt(const NC_EXEC_BLOCK* pBlock)
{
    uint32_t i;
    s_interpActive = 1U;
    s_interpSteps = 0U;
    for (i = 0U; i < AXIS_MAX; ++i) {
        if ((pBlock->axis_mask & (1UL << i)) != 0U) { g_ioOut.axis_target[i] = pBlock->axis_target[i]; }
    }
    return 0;
}
int32_t NcInterpolation_StepRt(void) { s_interpSteps++; s_interpActive = 0U; return 1; }

void NcDiagnostics_OnBlockRt(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcCompensation_Reset(void) { (void)memset((void*)&g_ncCompensationStatus, 0, sizeof(g_ncCompensationStatus)); }
void NcKinematics_Reset(void) { (void)memset((void*)&g_ncKinematicsStatus, 0, sizeof(g_ncKinematicsStatus)); }
void NcLookahead_Reset(void) { (void)memset((void*)&g_ncPathControlStatus, 0, sizeof(g_ncPathControlStatus)); }
void NcCapability_Reset(void) { (void)memset((void*)&g_ncCapabilityStatus, 0, sizeof(g_ncCapabilityStatus)); }
void NcCapability_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcInterference_Reset(void) { (void)memset((void*)&g_ncInterferenceStatus, 0, sizeof(g_ncInterferenceStatus)); }
void NcInterference_CheckBlockTs(NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcLatheCycle_Reset(void) { (void)memset((void*)&g_ncTurningCycleStatus, 0, sizeof(g_ncTurningCycleStatus)); (void)memset((void*)&g_ncThreadCycleStatus, 0, sizeof(g_ncThreadCycleStatus)); }
void NcLatheCycle_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcLatheCycle_OnBlockRt(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcReference_Reset(void) { (void)memset((void*)&g_ncReferenceStatus, 0, sizeof(g_ncReferenceStatus)); }
void NcReference_OnParsedBlockTs(NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcReference_OnBlockRt(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcPrecision_Reset(void) { (void)memset((void*)&g_ncPrecisionStatus, 0, sizeof(g_ncPrecisionStatus)); }
void NcPrecision_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcPrecision_OnBlockRt(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcSpindle_Reset(void) { (void)memset((void*)&g_ncSpindleStatus, 0, sizeof(g_ncSpindleStatus)); }
void NcSpindle_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcSpindle_OnBlockRt(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcToolManagement_Reset(void) { (void)memset((void*)&g_ncToolManagementStatus, 0, sizeof(g_ncToolManagementStatus)); }
void NcToolManagement_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcToolManagement_OnBlockRt(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcSynchronization_Reset(void) { (void)memset((void*)&g_ncSynchronizationStatus, 0, sizeof(g_ncSynchronizationStatus)); }
void NcSynchronization_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcSynchronization_ApplyBlockRt(NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcRotaryMcc_Reset(void) { (void)memset((void*)&g_ncRotaryMccStatus, 0, sizeof(g_ncRotaryMccStatus)); }
int32_t NcRotaryMcc_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError) { (void)pBlock; *pOutError = NC_ERR_NONE; return 0; }
void NcRotaryMcc_OnBlockRt(const NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcSafetyMotion_ApplyBlockRt(NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcParse_ResetModal(void) {}
int32_t NcParse_LineToBlock(const char* line, uint32_t lineNo, NC_EXEC_BLOCK* pOutBlock, NC_ERROR_CODE* pOutError)
{
    (void)line; (void)lineNo; (void)pOutBlock; *pOutError = NC_ERR_UNSUPPORTED; return -1;
}
void NcLookahead_ApplyBlockTs(NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
