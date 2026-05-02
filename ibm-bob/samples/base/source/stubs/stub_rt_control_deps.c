#include "system_shared.h"

void NcDiagnostics_UpdateAxisLoadRt(void) { g_ncAxisLoadStatus.generation++; }
void NcDiagnostics_BuildSnapshot(void) { g_ncDiagnosticSnapshot.nc_state = g_ncProgramStatus.state; g_ncDiagnosticSnapshot.generation++; }
void NcSafetyMotion_UpdateRt(void) { g_ncSafetyMotionStatus.generation++; }
void NcReference_ServiceRt(void) { g_ncReferenceStatus.generation++; }
void NcPrecision_ServiceRt(void) { g_ncPrecisionStatus.generation++; }
void NcSpindle_ServiceRt(void) { g_ncSpindleStatus.generation++; }
void NcToolManagement_ServiceRt(void) { g_ncToolManagementStatus.generation++; }
void NcRotaryMcc_ServiceRt(void) { g_ncRotaryMccStatus.generation++; }
void RtNcProgram_ProcessRequests(uint8_t safetyOk) { g_ncProgramStatus.response_code = safetyOk ? RESPONSE_ACCEPTED : RESPONSE_REJECTED; }
void RtNcProgram_ConsumeBlocks(void) { g_ncProgramStatus.generation++; }
