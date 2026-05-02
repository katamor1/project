#include <string.h>
#include "nc_capability.h"
#include "nc_capability.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_parser_internal.h"

void NcCoordinate_Reset(void)
{
    (void)memset((void*)&g_ncCoordinateState, 0, sizeof(g_ncCoordinateState));
    g_ncCoordinateState.unit_mode = NC_UNIT_MM;
    g_ncCoordinateState.distance_mode = NC_DISTANCE_ABSOLUTE;
}
void NcCoordinate_SetUnitMode(NC_UNIT_MODE mode) { g_ncCoordinateState.unit_mode = mode; }
void NcCoordinate_SetDistanceMode(NC_DISTANCE_MODE mode) { g_ncCoordinateState.distance_mode = mode; }
void NcCoordinate_SelectWork(uint8_t workIndex) { g_ncCoordinateState.selected_work_index = workIndex; }
void NcCoordinate_FillTargets(NC_EXEC_BLOCK* pBlock)
{
    uint32_t i;
    for (i = 0U; i < AXIS_MAX; ++i) {
        pBlock->axis_target[i] = g_ncCoordinateState.program_position[i];
    }
}
int32_t NcCoordinate_SetWorkOffsetAxis(uint8_t workIndex, int32_t axisIndex, int32_t offset)
{
    if ((workIndex >= NC_WORK_COORD_SYSTEMS) || (axisIndex < 0) || (axisIndex >= (int32_t)AXIS_MAX)) { return -1; }
    g_ncCoordinateState.work_offset[workIndex][axisIndex] = offset;
    return 0;
}
int32_t NcCoordinate_SetLocalShiftAxis(int32_t axisIndex, int32_t shift)
{
    if ((axisIndex < 0) || (axisIndex >= (int32_t)AXIS_MAX)) { return -1; }
    g_ncCoordinateState.local_shift[axisIndex] = shift;
    return 0;
}
int32_t NcCoordinate_SetMachineAxis(int32_t axisIndex, int32_t machinePosition, NC_EXEC_BLOCK* pBlock)
{
    if ((axisIndex < 0) || (axisIndex >= (int32_t)AXIS_MAX)) { return -1; }
    pBlock->axis_target[axisIndex] = machinePosition;
    pBlock->axis_mask |= (1UL << axisIndex);
    return 0;
}
int32_t NcCoordinate_SetTemporaryAxis(int32_t axisIndex, int32_t programPosition)
{
    if ((axisIndex < 0) || (axisIndex >= (int32_t)AXIS_MAX)) { return -1; }
    g_ncCoordinateState.program_position[axisIndex] = programPosition;
    return 0;
}
int32_t NcCoordinate_SetProgramAxis(int32_t axisIndex, int32_t programPosition, NC_EXEC_BLOCK* pBlock)
{
    if ((axisIndex < 0) || (axisIndex >= (int32_t)AXIS_MAX)) { return -1; }
    pBlock->axis_target[axisIndex] = programPosition;
    pBlock->axis_mask |= (1UL << axisIndex);
    return 0;
}

void NcCycle_ResetParserModal(void) {}
void NcCycle_InitParserBlock(NC_EXEC_BLOCK* pBlock) { (void)pBlock; }
void NcCycle_ApplyCycleGCode(int32_t code, NC_EXEC_BLOCK* pBlock) { pBlock->cycle_code10 = (uint16_t)code; }
void NcCycle_CancelParserModal(void) {}
int32_t NcCycle_SetRepeatWord(double value, NC_EXEC_BLOCK* pBlock)
{
    if (value < 0.0) { return -2; }
    pBlock->cycle_repeat = (uint32_t)(value + 0.5);
    return 0;
}
int32_t NcCycle_FinalizeParserBlock(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError)
{
    (void)pBlock;
    *pOutError = NC_ERR_NONE;
    return 0;
}

int32_t NcParser_FinalizeFeatureBlock(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError)
{
    (void)pBlock;
    *pOutError = NC_ERR_NONE;
    return 0;
}
int32_t NcCoordinateTransform_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError) { (void)pBlock; *pOutError = NC_ERR_NONE; return 0; }
int32_t NcCompensation_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError) { (void)pBlock; *pOutError = NC_ERR_NONE; return 0; }
int32_t NcAxisConfig_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError) { (void)pBlock; *pOutError = NC_ERR_NONE; return 0; }
int32_t NcKinematics_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError) { (void)pBlock; *pOutError = NC_ERR_NONE; return 0; }
int32_t NcRotaryMcc_ApplyBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError) { (void)pBlock; *pOutError = NC_ERR_NONE; return 0; }
int32_t NcInterpolation_PrepareBlockTs(NC_EXEC_BLOCK* pBlock, NC_ERROR_CODE* pOutError) { (void)pBlock; *pOutError = NC_ERR_NONE; return 0; }

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
        g_ncCapabilityStatus.last_address_mask = (1UL << code);
    }
    g_ncCapabilityStatus.generation++;
}
void NcParse_ResetModal(void) { NcParser_ResetModalState(); }
int32_t NcParser_ApplyFeatureG(int32_t code, NC_EXEC_BLOCK* pBlock, NC_PARSE_CONTEXT* pCtx)
{
    (void)code; (void)pBlock; (void)pCtx;
    return 1;
}
void NcCapability_Reset(void)
{
    (void)memset((void*)&g_ncCapabilityStatus, 0, sizeof(g_ncCapabilityStatus));
}
