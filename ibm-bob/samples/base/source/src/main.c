/* ibm-bob/samples/base/source/src/main.c */
/* Runs a small simulation that exercises baseline machine and NC behavior. */
/* This exists so the pinned smoke tests can verify the sample end to end. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/rt_control.c, ibm-bob/samples/base/source/src/ts_service.c, ibm-bob/samples/base/source/src/nc_program.c */
#include <stdio.h>
#include "system_shared.h"
#include "control_api.h"
#include "nc_program.h"
#include "rt_control.h"
#include "ts_service.h"

#define NC_SMOKE_FILE "sample_nc_program.nc"
#define NC_TOOL_EXPIRED_FILE "sample_nc_tool_expired.nc"

static int32_t WriteNcSmokeFile(void)
{
    FILE* pFile = fopen(NC_SMOKE_FILE, "w");

    if (pFile == NULL) {
        return -1;
    }

    (void)fprintf(pFile, "%%\n");
    (void)fprintf(pFile, "N10 G21 G90 (unit and distance mode)\n");
    (void)fprintf(pFile, "G54\n");
    (void)fprintf(pFile, "G10 L2 P1 X0.1 Y0.2 Z0.3\n");
    (void)fprintf(pFile, "G52 X0.01 Y0.02 Z0.03\n");
    (void)fprintf(pFile, "G00 X0.001 Y0.002 Z0.003\n");
    (void)fprintf(pFile, "G02 X0.002 Y0.001 Z0.004 I0.000 J-0.001 F60000 ; arc-helical segment plan\n");
    (void)fprintf(pFile, "G03 X0.003 Y0.002 I0.000 J0.001 F60000 ; CCW arc segment plan\n");
    (void)fprintf(pFile, "G01 X0.003 Y0.004 F60000 ; sharp corner lookahead slowdown\n");
    (void)fprintf(pFile, "G43 H1 ; apply tool length compensation\n");
    (void)fprintf(pFile, "G41 D1 ; apply cutter radius compensation\n");
    (void)fprintf(pFile, "G68 X0 Y0 R15 ; coordinate rotation enable\n");
    (void)fprintf(pFile, "G68.2 X0 Y0 Z0 A5 B10 C0 ; tilted work plane enable\n");
    (void)fprintf(pFile, "G53.1 B10 ; tool axis direction sample\n");
    (void)fprintf(pFile, "G51.1 X1 ; mirror X axis around tilted origin\n");
    (void)fprintf(pFile, "G04 P1\n");
    (void)fprintf(pFile, "G91\n");
    (void)fprintf(pFile, "G01 X0.001 Y0.001 F60000\n");
    (void)fprintf(pFile, "G01 X0.010 Y0.020 Z0.005 A0.010 F60000 ; transformed 5-axis target\n");
    (void)fprintf(pFile, "G90 G20\n");
    (void)fprintf(pFile, "G01 X0.001 Y0.001 Z0.001 F60000\n");
    (void)fprintf(pFile, "G21\n");
    (void)fprintf(pFile, "G92 X0.1354 Y0.2454 Z0.3554\n");
    (void)fprintf(pFile, "G53 G00 X0 Y0 Z0\n");
    (void)fprintf(pFile, "G28 X0 Y0 ; reference return sample\n");
    (void)fprintf(pFile, "G30 Z0 ; second reference return sample\n");
    (void)fprintf(pFile, "G60 X0.015 Y0.016 F60000 ; one-direction positioning sample\n");
    (void)fprintf(pFile, "G10.6 Z0.100 ; controlled retract sample\n");
    (void)fprintf(pFile, "G54\n");
    (void)fprintf(pFile, "G98 G83 X0.025 Y0.035 Z0.010 R0.050 Q0.020 F60000\n");
    (void)fprintf(pFile, "G80\n");
    (void)fprintf(pFile, "M06 T2 ; tool change waits for MFIN\n");
    (void)fprintf(pFile, "G70.7 X0.028 Z0.012 R0.050 Q0.020 F60000\n");
    (void)fprintf(pFile, "G06.2 X0.029 Y0.036 Z0.013 F60000 ; NURBS-like fixed block\n");
    (void)fprintf(pFile, "G12.1 X0.030 Y0.037 F60000 ; polar/cylindrical mode sample\n");
    (void)fprintf(pFile, "G07.1 A0.100 F60000 ; rotary virtual circumferential speed sample\n");
    (void)fprintf(pFile, "G13.1 ; rotary MCC mode cancel sample\n");
    (void)fprintf(pFile, "G05.2 ; high precision smoothing mode\n");
    (void)fprintf(pFile, "G08 P1 ; standard high-speed smoothing on\n");
    (void)fprintf(pFile, "G01 X0.031 Y0.038 F60000 ; G08 standard short segment\n");
    (void)fprintf(pFile, "G08 P2 ; high-precision smoothing on\n");
    (void)fprintf(pFile, "G01 X0.032 Y0.039 F60000 ; G08 high-precision short segment\n");
    (void)fprintf(pFile, "G08 P0 ; high-speed smoothing off\n");
    (void)fprintf(pFile, "G51.4\n");
    (void)fprintf(pFile, "G51.5\n");
    (void)fprintf(pFile, "G51.6\n");
    (void)fprintf(pFile, "G01 X0.033 Y0.040 F60000 ; sync/compound/overlay active segment\n");
    (void)fprintf(pFile, "G50.6\n");
    (void)fprintf(pFile, "G50.5\n");
    (void)fprintf(pFile, "G16 ; polar coordinate mode on\n");
    (void)fprintf(pFile, "G15 ; polar coordinate mode off\n");
    (void)fprintf(pFile, "G31 X0.030 F60000\n");
    (void)fprintf(pFile, "G02.3 X0.035 Y0.040 F60000\n");
    (void)fprintf(pFile, "G50.4\n");
    (void)fprintf(pFile, "G50.1 ; mirror cancel\n");
    (void)fprintf(pFile, "G69.1 ; tilted work plane cancel\n");
    (void)fprintf(pFile, "G69 ; coordinate rotation cancel\n");
    (void)fprintf(pFile, "G40 ; cutter radius cancel\n");
    (void)fprintf(pFile, "G49 ; tool length cancel\n");
    (void)fprintf(pFile, "G99 G81 X0.040 Y0.041 Z0.010 R0.050 F60000 K2 ; drill cycle repeat\n");
    (void)fprintf(pFile, "G82 X0.041 Y0.042 Z0.011 R0.050 P1 F60000 ; dwell drill\n");
    (void)fprintf(pFile, "G83 X0.042 Y0.043 Z0.012 R0.050 Q0.010 F60000 ; deep peck\n");
    (void)fprintf(pFile, "G73 X0.043 Y0.044 Z0.013 R0.050 Q0.005 F60000 ; chip break peck\n");
    (void)fprintf(pFile, "S600 G84 X0.044 Y0.045 Z0.010 R0.050 F1 ; right tap\n");
    (void)fprintf(pFile, "G74 X0.045 Y0.046 Z0.010 R0.050 F1 ; left tap\n");
    (void)fprintf(pFile, "G85 X0.046 Y0.047 Z0.010 R0.050 F60000 ; boring feed return\n");
    (void)fprintf(pFile, "G86 X0.047 Y0.048 Z0.010 R0.050 F60000 ; boring spindle stop diagnostic\n");
    (void)fprintf(pFile, "G87 X0.048 Y0.049 Z0.010 R0.050 F60000 ; back boring diagnostic\n");
    (void)fprintf(pFile, "G88 X0.049 Y0.050 Z0.010 R0.050 F60000 ; manual return diagnostic\n");
    (void)fprintf(pFile, "G89 X0.050 Y0.051 Z0.010 R0.050 P1 F60000 ; dwell boring\n");
    (void)fprintf(pFile, "G80\n");
    (void)fprintf(pFile, "G05 P10000 ; HPCC on\n");
    (void)fprintf(pFile, "G01 X0.021 Y0.031 Z0.041 F60000 ; HPCC segment 1\n");
    (void)fprintf(pFile, "G01 X0.022 Y0.032 Z0.042 F60000 ; HPCC segment 2\n");
    (void)fprintf(pFile, "G05 P3 ; high-speed relation diagnostic\n");
    (void)fprintf(pFile, "G05 P0 ; HPCC off\n");
    (void)fprintf(pFile, "G00 X0.02 Y0.03 Z0.04 M130\n");
    (void)fclose(pFile);
    return 0;
}

static int32_t WriteNcToolExpiredFile(void)
{
    FILE* pFile = fopen(NC_TOOL_EXPIRED_FILE, "w");
    if (pFile == NULL) {
        return -1;
    }
    (void)fprintf(pFile, "%%\n");
    (void)fprintf(pFile, "G21 G90\n");
    (void)fprintf(pFile, "M06 T3 ; tool-life expiry hold sample\n");
    (void)fprintf(pFile, "G01 X0.050 F60000\n");
    (void)fclose(pFile);
    return 0;
}

static uint32_t ParseLineExpectError(const char* firstLine,
                                     const char* secondLine,
                                     NC_ERROR_CODE expected)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE error = NC_ERR_NONE;
    int32_t result;

    NcParse_ResetModal();
    if (firstLine != NULL) {
        result = NcParse_LineToBlock(firstLine, 1U, &block, &error);
        if (result < 0) {
            return 0U;
        }
    }
    result = NcParse_LineToBlock(secondLine, 2U, &block, &error);
    return (uint32_t)((result < 0) && (error == expected));
}

static uint32_t RunCoordinateErrorChecks(void)
{
    uint32_t passed = 0U;

    passed += ParseLineExpectError("G91", "G53 G00 X0", NC_ERR_COORDINATE);
    passed += ParseLineExpectError(NULL, "G10 L2 P7", NC_ERR_RANGE);
    passed += ParseLineExpectError(NULL, "G10 L3 P1", NC_ERR_UNSUPPORTED);
    passed += ParseLineExpectError(NULL, "G20 G21", NC_ERR_MODAL_CONFLICT);
    passed += ParseLineExpectError(NULL, "G90 G91", NC_ERR_MODAL_CONFLICT);
    return passed;
}

static uint32_t RunG08ErrorChecks(void)
{
    uint32_t passed = 0U;

    passed += ParseLineExpectError(NULL, "G08", NC_ERR_RANGE);
    passed += ParseLineExpectError(NULL, "G08 P3", NC_ERR_RANGE);
    passed += ParseLineExpectError(NULL, "G08 P1 G05.2", NC_ERR_MODAL_CONFLICT);
    return passed;
}

static uint32_t RunHpccErrorChecks(void)
{
    uint32_t passed = 0U;

    passed += ParseLineExpectError(NULL, "G05", NC_ERR_RANGE);
    passed += ParseLineExpectError(NULL, "G05 P2", NC_ERR_RANGE);
    passed += ParseLineExpectError(NULL, "G05 P10000 G08 P1", NC_ERR_MODAL_CONFLICT);
    passed += ParseLineExpectError(NULL, "G05 P10000 G05.2", NC_ERR_MODAL_CONFLICT);
    return passed;
}

static uint32_t RunCycleErrorChecks(void)
{
    uint32_t passed = 0U;

    passed += ParseLineExpectError(NULL, "G81 X1 R1 F100", NC_ERR_RANGE);
    passed += ParseLineExpectError(NULL, "G83 X1 Z-1 R1 Q0 F100", NC_ERR_RANGE);
    passed += ParseLineExpectError(NULL, "G84 X1 Z-1 R1 F1", NC_ERR_FEED);
    passed += ParseLineExpectError(NULL, "G81 X1 Z-1 R1 K10000 F100", NC_ERR_RANGE);
    return passed;
}

static void SimulateExternalInputs(uint32_t cycle)
{
    if (cycle == 1U) {
        g_ioIn.sensor_bits |= 0x00000008UL; /* READY */
        g_ioIn.sensor_bits |= SENSOR_ABSOLUTE_REF_VALID_BIT;
        g_ioIn.analog[0] = 300;
    }
    if (cycle == 3U) {
        (void)Api_RequestRunModeChange((uint8_t)RUN_MODE_AUTO);
    }
    if (cycle == 4U) {
        (void)Api_RequestNcProgramLoad(NC_SMOKE_FILE);
    }
    if (cycle == 5U) {
        (void)Api_RequestNcProgramStart();
    }
    if (cycle == 6U) {
        g_ioIn.sensor_bits |= 0x00000010UL; /* RUN */
    }
    if (cycle == 60U) {
        g_ioIn.sensor_bits |= SENSOR_EXTERNAL_DECEL_BIT;
    }
    if (cycle == 90U) {
        g_ioIn.sensor_bits &= ~SENSOR_EXTERNAL_DECEL_BIT;
    }
    if (cycle == 120U) {
        g_ioIn.sensor_bits |= SENSOR_POSITION_SWITCH_BIT;
        g_ioIn.sensor_bits |= SENSOR_REFERENCE_MARK_BASE_BIT;
    }
    if (cycle == 122U) {
        g_ioIn.sensor_bits &= ~SENSOR_POSITION_SWITCH_BIT;
        g_ioIn.sensor_bits &= ~SENSOR_REFERENCE_MARK_BASE_BIT;
    }
    if (cycle == 30U) {
        g_ioIn.analog[1] = 850; /* X-axis load enters staged-decel range */
        g_ioIn.analog[2] = 640; /* Y-axis load enters warning range */
    }
    if (cycle == 150U) {
        g_ioIn.seq_bits |= SENSOR_SEQ_MFIN_BIT; /* MFIN response */
    }
    if (cycle == 250U) {
        g_ioIn.seq_bits &= ~SENSOR_SEQ_MFIN_BIT;
    }
    if (cycle == 2U) {
        g_ioIn.seq_bits |= SENSOR_SEQ_SKIP_BIT; /* G31 skip probe is armed before G31 */
    }
}

int main(void)
{
    STATUS_SNAPSHOT snapshot;
    NC_PROGRAM_STATUS ncStatus;
    NC_INTERP_STATUS interpStatus;
    NC_FEED_STATUS feedStatus;
    NC_COORDINATE_STATE coordStatus;
    NC_COORDINATE_TRANSFORM_STATUS coordTransformStatus;
    NC_FEATURE_STATUS featureStatus;
    PRESTART_INTERLOCK_STATUS prestartStatus;
    NC_AUX_STATUS auxStatus;
    NC_CYCLE_STATUS cycleStatus;
    IO_TRACE_BUFFER traceBuffer;
    NC_COMPENSATION_STATUS compStatus;
    NC_PATH_CONTROL_STATUS pathStatus;
    NC_KINEMATICS_STATUS kinStatus;
    NC_AXIS_LOAD_STATUS loadStatus;
    NC_TOOL_LIFE_STATUS toolLifeStatus;
    NC_DIAGNOSTIC_SNAPSHOT diagSnapshot;
    NC_MOTION_FILTER_STATUS filterStatus;
    NC_EVENT_RING eventRing;
    NC_CAPABILITY_STATUS capabilityStatus;
    NC_INTERFERENCE_STATUS interferenceStatus;
    NC_SAFETY_MOTION_STATUS safetyStatus;
    NC_BINARY_PROGRAM_STATUS binaryStatus;
    NC_REFERENCE_STATUS referenceStatus;
    NC_ROTARY_MCC_STATUS rotaryMccStatus;
    NC_AXIS_CONFIG_STATUS axisConfigStatus;
    NC_TOOL_MANAGEMENT_STATUS toolMgmtStatus;
    int32_t ncAxis[AXIS_MAX] = {0};
    uint32_t ncMCode = 0U;
    uint32_t mainNcError = 0U;
    uint32_t mainNcErrorLine = 0U;
    uint32_t mainNcExec = 0U;
    uint32_t mainNcRead = 0U;
    uint32_t mainNcBuf = 0U;
    uint32_t interpSegments = 0U;
    uint32_t feedProfile = 0U;
    uint32_t feedOverride = 0U;
    uint32_t dwellTicks = 0U;
    uint32_t coordUnit = 0U;
    uint32_t coordDistance = 0U;
    uint32_t coordWork = 0U;
    uint32_t coordLocal = 0U;
    int32_t coordWorkX = 0;
    uint32_t coordTransformBlocks = 0U;
    uint32_t coordTransformMask = 0U;
    uint32_t coordTransformWorkOffsetSet = 0U;
    uint32_t coordTransformLocalSet = 0U;
    uint32_t coordTransformTempSet = 0U;
    int32_t coordTransformDx = 0;
    int32_t coordTransformErrX = 0;
    int32_t coordTransformRotZ = 0;
    uint32_t featureBlocks = 0U;
    uint32_t featureFlags = 0U;
    uint32_t cannedBlocks = 0U;
    uint32_t skipBlocks = 0U;
    uint32_t advancedBlocks = 0U;
    uint32_t g08StandardSeen = 0U;
    uint32_t g08HighPrecisionSeen = 0U;
    uint32_t g08CancelSeen = 0U;
    uint32_t g08FinalSmoothing = 0U;
    uint32_t g08FinalHighPrecision = 0U;
    uint32_t hpccSeen = 0U;
    uint32_t hpccCancelSeen = 0U;
    uint32_t hpccFinal = 0U;
    uint32_t g05P3Seen = 0U;
    uint32_t auxCompleted = 0U;
    uint32_t auxWaitCycles = 0U;
    uint32_t cycleCompleted = 0U;
    uint32_t cyclePhaseCount = 0U;
    uint32_t cycleSkipCompleted = 0U;
    uint32_t cycleLastCode = 0U;
    uint32_t cycleRepeat = 0U;
    uint32_t cycleRepeatDone = 0U;
    uint32_t cyclePeckBlocks = 0U;
    uint32_t cycleTapBlocks = 0U;
    uint32_t cycleBoringBlocks = 0U;
    uint32_t cycleSpindleSync = 0U;
    uint32_t cycleSpindleReverse = 0U;
    uint32_t cycleManualReturn = 0U;
    int32_t cycleSkipX = 0;
    uint32_t prestartMask = 0U;
    uint32_t prestartOk = 0U;
    uint32_t traceFrozen = 0U;
    uint32_t traceCaptured = 0U;
    uint32_t traceTriggerCycle = 0U;
    uint32_t compAppliedBlocks = 0U;
    uint32_t compAdjustedMask = 0U;
    int32_t compToolLength = 0;
    int32_t compCutterRadius = 0;
    uint32_t compRotation = 0U;
    uint32_t pathArcBlocks = 0U;
    uint32_t pathHelicalBlocks = 0U;
    uint32_t pathArcSegments = 0U;
    uint32_t pathLookaheadBlocks = 0U;
    uint32_t pathSlowdowns = 0U;
    uint32_t pathCornerAngle = 0U;
    uint32_t pathOverride = 0U;
    uint32_t kinTransformed = 0U;
    uint32_t kinMirrored = 0U;
    uint32_t kinRetracts = 0U;
    uint32_t kinSingularityWarn = 0U;
    uint32_t kinMirrorMask = 0U;
    uint32_t kinRetractMask = 0U;
    int32_t kinToolVectorZ = 0;
    int32_t kinLastX = 0;
    int32_t kinLastA = 0;
    uint32_t loadWarningMask = 0U;
    uint32_t loadLimitMask = 0U;
    uint32_t loadCriticalMask = 0U;
    uint32_t loadDecelLevel = 0U;
    uint32_t loadDecelOverride = 0U;
    int32_t loadPeakX = 0;
    uint32_t toolActive = 0U;
    uint32_t tool2Uses = 0U;
    uint32_t toolWarningMask = 0U;
    uint32_t toolExpiredMask = 0U;
    uint32_t toolMgmtActive = 0U;
    uint32_t toolMgmtPrepared = 0U;
    uint32_t toolMgmtParsedT = 0U;
    uint32_t toolMgmtExchangeReq = 0U;
    uint32_t toolMgmtExchangeDone = 0U;
    uint32_t toolMgmtMaxWait = 0U;
    uint32_t toolMgmtPocket2 = 0U;
    uint32_t diagDecelLevel = 0U;
    uint32_t diagToolExpired = 0U;
    uint32_t diagAxisWarn = 0U;
    uint32_t filterSamples = 0U;
    uint32_t filterMode = 0U;
    uint32_t filterAxisMask = 0U;
    int32_t filterRawVX = 0;
    int32_t filterSmoothVX = 0;
    int32_t filterOutX = 0;
    uint32_t filterVelocityLimitEvents = 0U;
    uint32_t filterAccelLimitEvents = 0U;
    uint32_t filterEndpointCorrections = 0U;
    uint32_t filterResidualSamples = 0U;
    int32_t filterMaxLagX = 0;
    int32_t filterResidualX = 0;
    uint32_t eventCaptured = 0U;
    uint32_t eventLost = 0U;
    uint32_t lastEventCode = 0U;
    uint32_t capParsed = 0U;
    uint32_t capG = 0U;
    uint32_t capM = 0U;
    uint32_t capAddr = 0U;
    uint32_t interferenceChecked = 0U;
    uint32_t interferenceSlowdown = 0U;
    uint32_t interferenceWarn = 0U;
    uint32_t safetyExternal = 0U;
    uint32_t safetyPosition = 0U;
    uint32_t safetyOverride = 0U;
    uint32_t binaryLoaded = 0U;
    uint32_t refPlanned = 0U;
    uint32_t refExecuted = 0U;
    uint32_t refHomedMask = 0U;
    uint32_t refMarkerCaptures = 0U;
    uint32_t refOneDirection = 0U;
    uint32_t refRollover = 0U;
    int32_t refApproachX = 0;
    uint32_t rotaryParsed = 0U;
    uint32_t rotaryExecuted = 0U;
    uint32_t rotaryMask = 0U;
    uint32_t rotaryMccOn = 0U;
    uint32_t rotaryMccOff = 0U;
    int32_t rotaryVirtualDelta = 0;
    uint32_t rotaryCmdBits = 0U;
    uint32_t axisCfgPathMask = 0U;
    uint32_t axisCfgRotaryMask = 0U;
    uint32_t axisCfgDiameterMask = 0U;
    uint32_t axisCfgApplied = 0U;
    uint32_t axisCfgDiameterBlocks = 0U;
    uint32_t axisCfgDetachedBlocks = 0U;
    uint32_t axisCfgRejected = 0U;
    uint32_t axisCfgNameA = 0U;
    int32_t axisCfgLastX = 0;
    uint32_t precisionPreviewed = 0U;
    uint32_t precisionLimited = 0U;
    uint32_t precisionLearningUpdates = 0U;
    uint32_t precisionVibrationWarnings = 0U;
    uint32_t precisionRtTicks = 0U;
    int32_t precisionCorrectionX = 0;
    uint32_t syncModeBits = 0U;
    uint32_t syncPlanned = 0U;
    uint32_t syncExecuted = 0U;
    uint32_t syncMotion = 0U;
    uint32_t syncCompoundMotion = 0U;
    uint32_t syncOverlayMotion = 0U;
    uint32_t syncDoubleTable = 0U;
    uint32_t syncSlaveMask = 0U;
    int32_t syncMaxFollowY = 0;
    int32_t syncOutY = 0;
    uint32_t toolLifeHoldState = 0U;
    uint32_t toolLifeHoldError = 0U;
    uint32_t toolLifeHoldLine = 0U;
    uint8_t ncCaptured = 0U;
    uint32_t cycle;

    SystemShared_Initialize();
    (void)Api_SetNcAxisDefinition(0U, (uint8_t)'X', NC_AXIS_TYPE_LINEAR, 0U);
    (void)Api_SetNcAxisDefinition(1U, (uint8_t)'Y', NC_AXIS_TYPE_LINEAR, 0U);
    (void)Api_SetNcAxisDefinition(2U, (uint8_t)'Z', NC_AXIS_TYPE_LINEAR, 0U);
    (void)Api_SetNcAxisDefinition(3U, (uint8_t)'A', NC_AXIS_TYPE_ROTARY, 1U);
    (void)Api_SetNcPathAxisMask(0x0000000fUL);
    (void)Api_SetNcAxisDetachedMask(0x00000000UL);
    (void)Api_SetNcAxisDiameterMode(0U, 1U);
    (void)Api_SetNcWorkOffset(0U, 0U, 100);
    (void)Api_SetNcLocalShift(1U, 20);
    (void)Api_SetNcTemporaryAbsolute(2U, 0);
    (void)Api_SetNcDynamicFixtureOffset(0U, 25);
    (void)Api_SetNcWorkMountError(0U, -10);
    (void)Api_SetNcRotaryTableOffset(2U, 5);
    (void)Api_SetNcToolLengthOffset(1U, 500);
    (void)Api_SetNcCutterRadiusOffset(1U, 300);
    (void)Api_SetNcAxisAssignment(3U, 3U, -1, 0U);
    (void)Api_SetNcMotionFilterConfig(1U, NC_MOTION_FILTER_SECOND_STAGE_FIR, 3U, 5U);
    (void)Api_SetNcMotionFilterAxisLimit(0U, 450, 160);
    (void)Api_SetNcMotionFilterAxisLimit(1U, 450, 160);
    (void)Api_SetNcMotionFilterAxisLimit(2U, 350, 140);
    (void)Api_SetNcMotionFilterAxisLimit(3U, 180, 80);
    (void)Api_SetNcToolLifeLimit(2U, 100U);
    (void)Api_SetNcToolLifeLimit(3U, 1U);
    (void)Api_SetNcToolPocket(2U, 12U);
    (void)Api_SetNcToolPocket(3U, 13U);
    (void)Api_SetNcSynchronizationMasterSlave(0U, 0x00000002UL);
    (void)Api_SetNcOverlayAxis(2U, 15);
    (void)Api_SetNcCompoundPathMask(0x0000000fUL);
    (void)Api_SetNcDoubleTableControl(1U, 0x00000002UL);
    (void)Api_RequestNcToolPrepare(2U);
    (void)Api_SetNcLearningControl(1U, 35, 4U);
    (void)Api_SetNcVibrationSuppression(1U, 120U, 35U);
    (void)Api_SetNcPreviewControl(1U, 32U, 40U);
    (void)Api_SetNcReferenceAxisConfig(0U, 0, 1, 360000000, 1U);
    (void)Api_SetNcReferenceAxisConfig(1U, 0, 1, 360000000, 0U);
    (void)Api_SetNcReferenceAxisConfig(2U, 0, -1, 360000000, 1U);
    (void)Api_SetNcOneDirectionApproach(0U, 1200);
    (void)Api_SetNcRotaryAxisRadius(3U, 75000);
    (void)Api_SetNcMccOutput(1U);
    if (WriteNcSmokeFile() != 0) {
        return 1;
    }

    for (cycle = 0U; cycle < 900U; ++cycle) {
        SimulateExternalInputs(cycle);
        RtDispatcher_ExecuteCycle();
        TsService_ExecuteSlice();
        if ((ncCaptured == 0U) &&
            (Api_GetNcProgramStatus(&ncStatus) == 0) &&
            (ncStatus.state == NC_COMPLETED)) {
            mainNcError = ncStatus.error_code;
            mainNcErrorLine = ncStatus.error_line_no;
            mainNcExec = ncStatus.exec_line_no;
            mainNcRead = ncStatus.read_line_no;
            mainNcBuf = ncStatus.buffered_blocks;
            ncAxis[0] = g_ioOut.axis_target[0];
            ncAxis[1] = g_ioOut.axis_target[1];
            ncAxis[2] = g_ioOut.axis_target[2];
            ncMCode = g_ioOut.aux_m_code;
            if (Api_GetNcInterpolationStatus(&interpStatus) == 0) {
                interpSegments = interpStatus.completed_segments;
            }
            if (Api_GetNcFeedStatus(&feedStatus) == 0) {
                feedProfile = (uint32_t)feedStatus.feed_profile;
                feedOverride = feedStatus.feed_override_percent;
                dwellTicks = feedStatus.completed_dwell_ticks;
            }
            if (Api_GetNcCoordinateStatus(&coordStatus) == 0) {
                coordUnit = (uint32_t)coordStatus.unit_mode;
                coordDistance = (uint32_t)coordStatus.distance_mode;
                coordWork = coordStatus.selected_work_index;
                coordLocal = coordStatus.active_local_mask;
                coordWorkX = coordStatus.work_offset[0][0];
            }
            if (Api_GetNcCoordinateTransformStatus(&coordTransformStatus) == 0) {
                coordTransformBlocks = coordTransformStatus.corrected_blocks;
                coordTransformMask = coordTransformStatus.last_axis_mask;
                coordTransformWorkOffsetSet = coordTransformStatus.work_offset_set_calls;
                coordTransformLocalSet = coordTransformStatus.local_shift_set_calls;
                coordTransformTempSet = coordTransformStatus.temporary_absolute_set_calls;
                coordTransformDx = coordTransformStatus.dynamic_fixture_offset[0];
                coordTransformErrX = coordTransformStatus.work_mount_error[0];
                coordTransformRotZ = coordTransformStatus.rotary_table_offset[2];
            }
            if (Api_GetNcFeatureStatus(&featureStatus) == 0) {
                featureBlocks = featureStatus.executed_feature_blocks;
                featureFlags = featureStatus.active_feature_flags;
                cannedBlocks = featureStatus.canned_cycle_blocks;
                skipBlocks = featureStatus.skip_blocks;
                advancedBlocks = featureStatus.advanced_interp_blocks;
                g08StandardSeen = (featureStatus.g08_standard_blocks != 0U);
                g08HighPrecisionSeen = (featureStatus.g08_high_precision_blocks != 0U);
                g08CancelSeen = (featureStatus.g08_cancel_blocks != 0U);
                g08FinalSmoothing = featureStatus.smoothing_enabled;
                g08FinalHighPrecision = featureStatus.high_precision_enabled;
                hpccSeen = (featureStatus.hpcc_blocks != 0U);
                hpccCancelSeen = (featureStatus.hpcc_cancel_blocks != 0U);
                hpccFinal = featureStatus.hpcc_enabled;
                g05P3Seen = (featureStatus.g05_p3_blocks != 0U);
            }
            if (Api_GetNcAuxStatus(&auxStatus) == 0) {
                auxCompleted = auxStatus.completed_m_codes;
                auxWaitCycles = auxStatus.last_wait_cycles;
            }
            if (Api_GetNcCycleStatus(&cycleStatus) == 0) {
                cycleCompleted = cycleStatus.completed_cycles;
                cyclePhaseCount = cycleStatus.phase_count;
                cycleSkipCompleted = cycleStatus.completed_skips;
                cycleLastCode = cycleStatus.last_cycle_code10;
                cycleRepeat = cycleStatus.cycle_repeat;
                cycleRepeatDone = cycleStatus.repeat_completed;
                cyclePeckBlocks = cycleStatus.peck_blocks;
                cycleTapBlocks = cycleStatus.tap_blocks;
                cycleBoringBlocks = cycleStatus.boring_blocks;
                cycleSpindleSync = cycleStatus.spindle_sync_blocks;
                cycleSpindleReverse = cycleStatus.spindle_reverse_events;
                cycleManualReturn = cycleStatus.manual_return_blocks;
                cycleSkipX = cycleStatus.last_skip_position[0];
            }
            if (Api_GetNcCompensationStatus(&compStatus) == 0) {
                compAppliedBlocks = compStatus.applied_blocks;
                compAdjustedMask = compStatus.adjusted_axis_mask;
                compToolLength = compStatus.tool_length_offset;
                compCutterRadius = compStatus.cutter_radius_offset;
                compRotation = compStatus.rotation_active;
            }
            if (Api_GetNcPathControlStatus(&pathStatus) == 0) {
                pathArcBlocks = pathStatus.arc_blocks;
                pathHelicalBlocks = pathStatus.helical_blocks;
                pathArcSegments = pathStatus.arc_segments_generated;
                pathLookaheadBlocks = pathStatus.lookahead_blocks;
                pathSlowdowns = pathStatus.corner_slowdown_events;
                pathCornerAngle = pathStatus.last_corner_angle_deg;
                pathOverride = pathStatus.last_override_percent;
            }
            if (Api_GetNcKinematicsStatus(&kinStatus) == 0) {
                kinTransformed = kinStatus.transformed_blocks;
                kinMirrored = kinStatus.mirrored_blocks;
                kinRetracts = kinStatus.retract_events;
                kinSingularityWarn = kinStatus.singularity_warnings;
                kinMirrorMask = kinStatus.mirror_axis_mask;
                kinRetractMask = kinStatus.retract_axis_mask;
                kinToolVectorZ = kinStatus.tool_axis_vector[2];
                kinLastX = kinStatus.last_transformed_target[0];
                kinLastA = kinStatus.last_transformed_target[3];
            }
            if (Api_GetNcAxisLoadStatus(&loadStatus) == 0) {
                loadWarningMask = loadStatus.warning_mask;
                loadLimitMask = loadStatus.limit_mask;
                loadCriticalMask = loadStatus.critical_mask;
                loadDecelLevel = loadStatus.staged_decel_level;
                loadDecelOverride = loadStatus.decel_override_percent;
                loadPeakX = loadStatus.peak_load[0];
            }
            if (Api_GetNcToolLifeStatus(&toolLifeStatus) == 0) {
                toolActive = toolLifeStatus.active_tool_no;
                tool2Uses = toolLifeStatus.tool_use_count[2];
                toolWarningMask = toolLifeStatus.warning_mask;
                toolExpiredMask = toolLifeStatus.expired_mask;
            }
            if (Api_GetNcToolManagementStatus(&toolMgmtStatus) == 0) {
                toolMgmtActive = toolMgmtStatus.active_tool_no;
                toolMgmtPrepared = toolMgmtStatus.prepared_tool_no;
                toolMgmtParsedT = toolMgmtStatus.parsed_t_words;
                toolMgmtExchangeReq = toolMgmtStatus.exchange_requests;
                toolMgmtExchangeDone = toolMgmtStatus.exchange_completed;
                toolMgmtMaxWait = toolMgmtStatus.max_exchange_wait_cycles;
                toolMgmtPocket2 = toolMgmtStatus.tool_to_pocket[2];
            }
            {
                NC_SYNCHRONIZATION_STATUS syncStatus;
                if (Api_GetNcSynchronizationStatus(&syncStatus) == 0) {
                    syncModeBits = syncStatus.active_mode_bits;
                    syncPlanned = syncStatus.planned_mode_blocks;
                    syncExecuted = syncStatus.executed_mode_blocks;
                    syncMotion = syncStatus.synchronized_motion_blocks;
                    syncCompoundMotion = syncStatus.compound_motion_blocks;
                    syncOverlayMotion = syncStatus.overlay_motion_blocks;
                    syncDoubleTable = syncStatus.double_table_blocks;
                    syncSlaveMask = syncStatus.slave_axis_mask;
                    syncMaxFollowY = syncStatus.max_following_error[1];
                    syncOutY = syncStatus.last_output_target[1];
                }
            }
            if (Api_GetNcDiagnosticSnapshot(&diagSnapshot) == 0) {
                diagDecelLevel = diagSnapshot.staged_decel_level;
                diagToolExpired = diagSnapshot.tool_expired_mask;
                diagAxisWarn = diagSnapshot.axis_warning_mask;
            }
            if (Api_GetPrestartInterlockStatus(&prestartStatus) == 0) {
                prestartMask = prestartStatus.mask;
                prestartOk = prestartStatus.ok;
            }
            ncCaptured = 1U;
        }
    }



    if (Api_GetNcAuxStatus(&auxStatus) == 0) {
        auxCompleted = auxStatus.completed_m_codes;
        auxWaitCycles = auxStatus.last_wait_cycles;
    }
    if (Api_GetPrestartInterlockStatus(&prestartStatus) == 0) {
        prestartMask = prestartStatus.mask;
        prestartOk = prestartStatus.ok;
    }
    if (Api_GetNcCycleStatus(&cycleStatus) == 0) {
        cycleCompleted = cycleStatus.completed_cycles;
        cyclePhaseCount = cycleStatus.phase_count;
        cycleSkipCompleted = cycleStatus.completed_skips;
        cycleLastCode = cycleStatus.last_cycle_code10;
        cycleRepeat = cycleStatus.cycle_repeat;
        cycleRepeatDone = cycleStatus.repeat_completed;
        cyclePeckBlocks = cycleStatus.peck_blocks;
        cycleTapBlocks = cycleStatus.tap_blocks;
        cycleBoringBlocks = cycleStatus.boring_blocks;
        cycleSpindleSync = cycleStatus.spindle_sync_blocks;
        cycleSpindleReverse = cycleStatus.spindle_reverse_events;
        cycleManualReturn = cycleStatus.manual_return_blocks;
        cycleSkipX = cycleStatus.last_skip_position[0];
    }
    if (Api_GetNcCompensationStatus(&compStatus) == 0) {
        compAppliedBlocks = compStatus.applied_blocks;
        compAdjustedMask = compStatus.adjusted_axis_mask;
        compToolLength = compStatus.tool_length_offset;
        compCutterRadius = compStatus.cutter_radius_offset;
        compRotation = compStatus.rotation_active;
    }
    if (Api_GetNcPathControlStatus(&pathStatus) == 0) {
        pathArcBlocks = pathStatus.arc_blocks;
        pathHelicalBlocks = pathStatus.helical_blocks;
        pathArcSegments = pathStatus.arc_segments_generated;
        pathLookaheadBlocks = pathStatus.lookahead_blocks;
        pathSlowdowns = pathStatus.corner_slowdown_events;
        pathCornerAngle = pathStatus.last_corner_angle_deg;
        pathOverride = pathStatus.last_override_percent;
    }
    if (Api_GetNcKinematicsStatus(&kinStatus) == 0) {
        kinTransformed = kinStatus.transformed_blocks;
        kinMirrored = kinStatus.mirrored_blocks;
        kinRetracts = kinStatus.retract_events;
        kinSingularityWarn = kinStatus.singularity_warnings;
        kinMirrorMask = kinStatus.mirror_axis_mask;
        kinRetractMask = kinStatus.retract_axis_mask;
        kinToolVectorZ = kinStatus.tool_axis_vector[2];
        kinLastX = kinStatus.last_transformed_target[0];
        kinLastA = kinStatus.last_transformed_target[3];
    }
    if (Api_GetNcAxisLoadStatus(&loadStatus) == 0) {
        loadWarningMask = loadStatus.warning_mask;
        loadLimitMask = loadStatus.limit_mask;
        loadCriticalMask = loadStatus.critical_mask;
        loadDecelLevel = loadStatus.staged_decel_level;
        loadDecelOverride = loadStatus.decel_override_percent;
        loadPeakX = loadStatus.peak_load[0];
    }
    if (Api_GetNcToolLifeStatus(&toolLifeStatus) == 0) {
        toolActive = toolLifeStatus.active_tool_no;
        tool2Uses = toolLifeStatus.tool_use_count[2];
        toolWarningMask = toolLifeStatus.warning_mask;
        toolExpiredMask = toolLifeStatus.expired_mask;
    }
    if (Api_GetNcDiagnosticSnapshot(&diagSnapshot) == 0) {
        diagDecelLevel = diagSnapshot.staged_decel_level;
        diagToolExpired = diagSnapshot.tool_expired_mask;
        diagAxisWarn = diagSnapshot.axis_warning_mask;
    }
    if (Api_GetNcMotionFilterStatus(&filterStatus) == 0) {
        filterSamples = filterStatus.applied_samples;
        filterMode = filterStatus.second_stage_mode;
        filterAxisMask = filterStatus.filtered_axis_mask;
        filterRawVX = filterStatus.max_raw_velocity[0];
        filterSmoothVX = filterStatus.max_filtered_velocity[0];
        filterOutX = filterStatus.last_filtered_target[0];
        filterVelocityLimitEvents = filterStatus.velocity_limit_events;
        filterAccelLimitEvents = filterStatus.accel_limit_events;
        filterEndpointCorrections = filterStatus.endpoint_corrections;
        filterResidualSamples = filterStatus.residual_correction_samples;
        filterMaxLagX = filterStatus.max_lag_error[0];
        filterResidualX = filterStatus.endpoint_residual[0];
    }
    if ((capParsed == 0U) && (Api_GetNcCapabilityStatus(&capabilityStatus) == 0)) {
        capParsed = capabilityStatus.parsed_blocks;
        capG = capabilityStatus.supported_g_blocks;
        capM = capabilityStatus.supported_m_blocks;
        capAddr = capabilityStatus.address_words_seen;
    }
    if ((interferenceChecked == 0U) && (Api_GetNcInterferenceStatus(&interferenceStatus) == 0)) {
        interferenceChecked = interferenceStatus.checked_blocks;
        interferenceSlowdown = interferenceStatus.slowdown_applied_blocks;
        interferenceWarn = interferenceStatus.path_interference_warnings +
                           interferenceStatus.three_d_interference_warnings +
                           interferenceStatus.axis_limit_warnings;
    }
    if ((safetyExternal == 0U) && (Api_GetNcSafetyMotionStatus(&safetyStatus) == 0)) {
        safetyExternal = safetyStatus.external_decel_events;
        safetyPosition = safetyStatus.position_switch_events;
        safetyOverride = safetyStatus.active_override_percent;
    }

    if (ncCaptured != 0U) {
        if (WriteNcToolExpiredFile() == 0) {
            uint32_t extra;
            (void)Api_RequestNcProgramLoad(NC_TOOL_EXPIRED_FILE);
            for (extra = 0U; extra < 8U; ++extra) {
                RtDispatcher_ExecuteCycle();
                TsService_ExecuteSlice();
            }
            (void)Api_RequestNcProgramStart();
            for (extra = 0U; extra < 60U; ++extra) {
                RtDispatcher_ExecuteCycle();
                TsService_ExecuteSlice();
            }
            if (Api_GetNcProgramStatus(&ncStatus) == 0) {
                toolLifeHoldState = ncStatus.state;
                toolLifeHoldError = ncStatus.error_code;
                toolLifeHoldLine = ncStatus.error_line_no;
            }
        }
        g_ioIn.analog[0] = 950;
        RtDispatcher_ExecuteCycle();
        TsService_ExecuteSlice();
    }

    if ((capParsed == 0U) && (Api_GetNcCapabilityStatus(&capabilityStatus) == 0)) {
        capParsed = capabilityStatus.parsed_blocks;
        capG = capabilityStatus.supported_g_blocks;
        capM = capabilityStatus.supported_m_blocks;
        capAddr = capabilityStatus.address_words_seen;
    }
    if ((interferenceChecked == 0U) && (Api_GetNcInterferenceStatus(&interferenceStatus) == 0)) {
        interferenceChecked = interferenceStatus.checked_blocks;
        interferenceSlowdown = interferenceStatus.slowdown_applied_blocks;
        interferenceWarn = interferenceStatus.path_interference_warnings +
                           interferenceStatus.three_d_interference_warnings +
                           interferenceStatus.axis_limit_warnings;
    }
    if ((safetyExternal == 0U) && (Api_GetNcSafetyMotionStatus(&safetyStatus) == 0)) {
        safetyExternal = safetyStatus.external_decel_events;
        safetyPosition = safetyStatus.position_switch_events;
        safetyOverride = safetyStatus.active_override_percent;
    }
    if (Api_GetNcBinaryProgramStatus(&binaryStatus) == 0) {
        binaryLoaded = binaryStatus.loaded_blocks;
    }
    if (Api_GetNcReferenceStatus(&referenceStatus) == 0) {
        refPlanned = referenceStatus.planned_reference_return_blocks;
        refExecuted = referenceStatus.executed_reference_return_blocks;
        refHomedMask = referenceStatus.homed_axis_mask;
        refMarkerCaptures = referenceStatus.reference_marker_captures;
        refOneDirection = referenceStatus.one_direction_positioning_blocks;
        refRollover = referenceStatus.rollover_events;
        refApproachX = referenceStatus.last_approach_start[0];
    }
    if (Api_GetNcRotaryMccStatus(&rotaryMccStatus) == 0) {
        rotaryParsed = rotaryMccStatus.parsed_virtual_speed_blocks;
        rotaryExecuted = rotaryMccStatus.executed_virtual_speed_blocks;
        rotaryMask = rotaryMccStatus.active_axis_mask;
        rotaryMccOn = rotaryMccStatus.mcc_on_events;
        rotaryMccOff = rotaryMccStatus.mcc_off_events;
        rotaryVirtualDelta = rotaryMccStatus.last_virtual_linear_delta;
        rotaryCmdBits = rotaryMccStatus.last_mcc_command_bits;
    }
    if (Api_GetNcAxisConfigStatus(&axisConfigStatus) == 0) {
        axisCfgPathMask = axisConfigStatus.active_path_axis_mask;
        axisCfgRotaryMask = axisConfigStatus.rotary_axis_mask;
        axisCfgDiameterMask = axisConfigStatus.diameter_axis_mask;
        axisCfgApplied = axisConfigStatus.applied_blocks;
        axisCfgDiameterBlocks = axisConfigStatus.diameter_converted_blocks;
        axisCfgDetachedBlocks = axisConfigStatus.detached_axis_blocks;
        axisCfgRejected = axisConfigStatus.rejected_calls;
        axisCfgNameA = axisConfigStatus.axis_name[3];
        axisCfgLastX = axisConfigStatus.last_output_target[0];
    }
    {
        NC_PRECISION_STATUS precisionStatus;
        if (Api_GetNcPrecisionStatus(&precisionStatus) == 0) {
            precisionPreviewed = precisionStatus.previewed_blocks;
            precisionLimited = precisionStatus.preview_feed_limited_blocks;
            precisionLearningUpdates = precisionStatus.learning_updates;
            precisionVibrationWarnings = precisionStatus.vibration_warnings;
            precisionRtTicks = precisionStatus.rt_service_ticks;
            precisionCorrectionX = precisionStatus.last_learning_correction[0];
        }
    }
    if (Api_GetNcEventRing(&eventRing) == 0) {
        uint32_t lastIndex = (eventRing.write_index == 0U) ?
                             (NC_EVENT_RING_SIZE - 1U) :
                             (eventRing.write_index - 1U);
        eventCaptured = eventRing.captured_count;
        eventLost = eventRing.lost_count;
        lastEventCode = eventRing.items[lastIndex].code;
    }
    if (Api_GetIoTraceBuffer(&traceBuffer) == 0) {
        traceFrozen = traceBuffer.frozen;
        traceCaptured = traceBuffer.captured_count;
        traceTriggerCycle = traceBuffer.trigger_cycle;
    }

    if (Api_GetNcProgramStatus(&ncStatus) == 0) {
        (void)printf("nc_state=%u nc_error=%u nc_error_line=%u nc_exec=%u nc_read=%u nc_buf=%u nc_x=%d nc_y=%d nc_z=%d nc_m=%u\n",
                     (unsigned int)(ncCaptured ? NC_COMPLETED : ncStatus.state),
                     (unsigned int)(ncCaptured ? mainNcError : ncStatus.error_code),
                     (unsigned int)(ncCaptured ? mainNcErrorLine : ncStatus.error_line_no),
                     (unsigned int)(ncCaptured ? mainNcExec : ncStatus.exec_line_no),
                     (unsigned int)(ncCaptured ? mainNcRead : ncStatus.read_line_no),
                     (unsigned int)(ncCaptured ? mainNcBuf : ncStatus.buffered_blocks),
                     (int)ncAxis[0],
                     (int)ncAxis[1],
                     (int)ncAxis[2],
                     (unsigned int)ncMCode);
    }
    (void)printf("interp_segments=%u feed_profile=%u feed_override=%u dwell_ticks=%u\n",
                 (unsigned int)interpSegments,
                 (unsigned int)feedProfile,
                 (unsigned int)feedOverride,
                 (unsigned int)dwellTicks);
    (void)printf("coord_unit=%u coord_distance=%u coord_work=%u coord_local=%u coord_work_x=%d\n",
                 (unsigned int)coordUnit,
                 (unsigned int)coordDistance,
                 (unsigned int)coordWork,
                 (unsigned int)coordLocal,
                 (int)coordWorkX);
    (void)printf("coord_transform blocks=%u mask=0x%08x work_set=%u local_set=%u temp_set=%u dyn_x=%d mount_err_x=%d rotary_z=%d\n",
                 (unsigned int)coordTransformBlocks,
                 (unsigned int)coordTransformMask,
                 (unsigned int)coordTransformWorkOffsetSet,
                 (unsigned int)coordTransformLocalSet,
                 (unsigned int)coordTransformTempSet,
                 (int)coordTransformDx,
                 (int)coordTransformErrX,
                 (int)coordTransformRotZ);
    (void)printf("feature_blocks=%u feature_flags=%u canned=%u skip=%u advanced=%u\n",
                 (unsigned int)featureBlocks,
                 (unsigned int)featureFlags,
                 (unsigned int)cannedBlocks,
                 (unsigned int)skipBlocks,
                 (unsigned int)advancedBlocks);
    (void)printf("g08_standard_seen=%u g08_high_precision_seen=%u g08_cancel_seen=%u g08_final_smoothing=%u g08_final_high_precision=%u\n",
                 (unsigned int)g08StandardSeen,
                 (unsigned int)g08HighPrecisionSeen,
                 (unsigned int)g08CancelSeen,
                 (unsigned int)g08FinalSmoothing,
                 (unsigned int)g08FinalHighPrecision);
    (void)printf("hpcc_seen=%u hpcc_cancel_seen=%u hpcc_final=%u g05_p3_seen=%u\n",
                 (unsigned int)hpccSeen,
                 (unsigned int)hpccCancelSeen,
                 (unsigned int)hpccFinal,
                 (unsigned int)g05P3Seen);
    (void)printf("comp_applied=%u comp_mask=0x%08x comp_tool=%d comp_cutter=%d comp_rotation=%u\n",
                 (unsigned int)compAppliedBlocks,
                 (unsigned int)compAdjustedMask,
                 (int)compToolLength,
                 (int)compCutterRadius,
                 (unsigned int)compRotation);
    (void)printf("path_arc=%u path_helical=%u path_arc_segments=%u path_lookahead=%u path_slowdowns=%u path_corner_deg=%u path_override=%u\n",
                 (unsigned int)pathArcBlocks,
                 (unsigned int)pathHelicalBlocks,
                 (unsigned int)pathArcSegments,
                 (unsigned int)pathLookaheadBlocks,
                 (unsigned int)pathSlowdowns,
                 (unsigned int)pathCornerAngle,
                 (unsigned int)pathOverride);
    (void)printf("kin_transformed=%u kin_mirrored=%u kin_retracts=%u kin_singularity_warn=%u kin_mirror_mask=0x%08x kin_retract_mask=0x%08x kin_tool_z=%d kin_last_x=%d kin_last_a=%d\n",
                 (unsigned int)kinTransformed,
                 (unsigned int)kinMirrored,
                 (unsigned int)kinRetracts,
                 (unsigned int)kinSingularityWarn,
                 (unsigned int)kinMirrorMask,
                 (unsigned int)kinRetractMask,
                 (int)kinToolVectorZ,
                 (int)kinLastX,
                 (int)kinLastA);
    (void)printf("diag_axis_warn=0x%08x diag_axis_limit=0x%08x diag_axis_critical=0x%08x decel_level=%u decel_override=%u load_peak_x=%d\n",
                 (unsigned int)loadWarningMask,
                 (unsigned int)loadLimitMask,
                 (unsigned int)loadCriticalMask,
                 (unsigned int)loadDecelLevel,
                 (unsigned int)loadDecelOverride,
                 (int)loadPeakX);
    (void)printf("tool_active=%u tool2_uses=%u tool_warn=0x%08x tool_expired=0x%08x diag_decel=%u diag_tool_expired=0x%08x diag_axis_warn=0x%08x\n",
                 (unsigned int)toolActive,
                 (unsigned int)tool2Uses,
                 (unsigned int)toolWarningMask,
                 (unsigned int)toolExpiredMask,
                 (unsigned int)diagDecelLevel,
                 (unsigned int)diagToolExpired,
                 (unsigned int)diagAxisWarn);
    (void)printf("tool_mgmt active=%u prepared=%u parsed_t=%u exchange_req=%u exchange_done=%u max_wait=%u pocket2=%u\n",
                 (unsigned int)toolMgmtActive,
                 (unsigned int)toolMgmtPrepared,
                 (unsigned int)toolMgmtParsedT,
                 (unsigned int)toolMgmtExchangeReq,
                 (unsigned int)toolMgmtExchangeDone,
                 (unsigned int)toolMgmtMaxWait,
                 (unsigned int)toolMgmtPocket2);
    (void)printf("motion_filter_samples=%u mode=%u axis_mask=0x%08x raw_vx=%d smooth_vx=%d out_x=%d\n",
                 (unsigned int)filterSamples,
                 (unsigned int)filterMode,
                 (unsigned int)filterAxisMask,
                 (int)filterRawVX,
                 (int)filterSmoothVX,
                 (int)filterOutX);
    (void)printf("motion_filter_limits vel_events=%u accel_events=%u endpoint_corrections=%u residual_samples=%u max_lag_x=%d residual_x=%d\n",
                 (unsigned int)filterVelocityLimitEvents,
                 (unsigned int)filterAccelLimitEvents,
                 (unsigned int)filterEndpointCorrections,
                 (unsigned int)filterResidualSamples,
                 (int)filterMaxLagX,
                 (int)filterResidualX);
    (void)printf("tool_life_hold_state=%u tool_life_hold_error=%u tool_life_hold_line=%u events=%u lost=%u last_event=%u\n",
                 (unsigned int)toolLifeHoldState,
                 (unsigned int)toolLifeHoldError,
                 (unsigned int)toolLifeHoldLine,
                 (unsigned int)eventCaptured,
                 (unsigned int)eventLost,
                 (unsigned int)lastEventCode);
    (void)printf("cycle_completed=%u cycle_phase_count=%u skip_completed=%u skip_x=%d\n",
                 (unsigned int)cycleCompleted,
                 (unsigned int)cyclePhaseCount,
                 (unsigned int)cycleSkipCompleted,
                 (int)cycleSkipX);
    (void)printf("cycle_detail last=%u repeat=%u repeat_done=%u peck_blocks=%u tap_blocks=%u boring_blocks=%u spindle_sync=%u spindle_reverse=%u manual_return=%u\n",
                 (unsigned int)cycleLastCode,
                 (unsigned int)cycleRepeat,
                 (unsigned int)cycleRepeatDone,
                 (unsigned int)cyclePeckBlocks,
                 (unsigned int)cycleTapBlocks,
                 (unsigned int)cycleBoringBlocks,
                 (unsigned int)cycleSpindleSync,
                 (unsigned int)cycleSpindleReverse,
                 (unsigned int)cycleManualReturn);
    (void)printf("aux_completed=%u aux_wait_cycles=%u prestart_ok=%u prestart_mask=0x%08x\n",
                 (unsigned int)auxCompleted,
                 (unsigned int)auxWaitCycles,
                 (unsigned int)prestartOk,
                 (unsigned int)prestartMask);
    (void)printf("trace_frozen=%u trace_captured=%u trace_trigger_cycle=%u\n",
                 (unsigned int)traceFrozen,
                 (unsigned int)traceCaptured,
                 (unsigned int)traceTriggerCycle);
    (void)printf("capability parsed=%u g=%u m=%u address_words=%u binary_loaded=%u\n",
                 (unsigned int)capParsed,
                 (unsigned int)capG,
                 (unsigned int)capM,
                 (unsigned int)capAddr,
                 (unsigned int)binaryLoaded);
    (void)printf("interference checked=%u warnings=%u slowdown=%u safety_external=%u safety_position=%u safety_override=%u\n",
                 (unsigned int)interferenceChecked,
                 (unsigned int)interferenceWarn,
                 (unsigned int)interferenceSlowdown,
                 (unsigned int)safetyExternal,
                 (unsigned int)safetyPosition,
                 (unsigned int)safetyOverride);

    (void)printf("reference planned=%u executed=%u homed=0x%08x markers=%u one_direction=%u rollover=%u approach_x=%d\n",
                 (unsigned int)refPlanned,
                 (unsigned int)refExecuted,
                 (unsigned int)refHomedMask,
                 (unsigned int)refMarkerCaptures,
                 (unsigned int)refOneDirection,
                 (unsigned int)refRollover,
                 (int)refApproachX);
    (void)printf("rotary_mcc parsed=%u executed=%u active_mask=0x%08x mcc_on=%u mcc_off=%u virt=%d cmd=0x%08x\n",
                 (unsigned int)rotaryParsed,
                 (unsigned int)rotaryExecuted,
                 (unsigned int)rotaryMask,
                 (unsigned int)rotaryMccOn,
                 (unsigned int)rotaryMccOff,
                 (int)rotaryVirtualDelta,
                 (unsigned int)rotaryCmdBits);
    (void)printf("axis_config path=0x%08x rotary=0x%08x diameter=0x%08x applied=%u diameter_blocks=%u detached_blocks=%u rejected=%u name_a=%c last_x=%d\n",
                 (unsigned int)axisCfgPathMask,
                 (unsigned int)axisCfgRotaryMask,
                 (unsigned int)axisCfgDiameterMask,
                 (unsigned int)axisCfgApplied,
                 (unsigned int)axisCfgDiameterBlocks,
                 (unsigned int)axisCfgDetachedBlocks,
                 (unsigned int)axisCfgRejected,
                 (char)axisCfgNameA,
                 (int)axisCfgLastX);
    (void)printf("sync mode_bits=0x%04x planned=%u executed=%u sync_motion=%u compound_motion=%u overlay_motion=%u double_table=%u slave_mask=0x%08x max_follow_y=%d out_y=%d\n",
                 (unsigned int)syncModeBits,
                 (unsigned int)syncPlanned,
                 (unsigned int)syncExecuted,
                 (unsigned int)syncMotion,
                 (unsigned int)syncCompoundMotion,
                 (unsigned int)syncOverlayMotion,
                 (unsigned int)syncDoubleTable,
                 (unsigned int)syncSlaveMask,
                 (int)syncMaxFollowY,
                 (int)syncOutY);
    (void)printf("precision previewed=%u limited=%u learning_updates=%u vibration_warn=%u rt_ticks=%u corr_x=%d\n",
                 (unsigned int)precisionPreviewed,
                 (unsigned int)precisionLimited,
                 (unsigned int)precisionLearningUpdates,
                 (unsigned int)precisionVibrationWarnings,
                 (unsigned int)precisionRtTicks,
                 (int)precisionCorrectionX);

    if (Api_GetStatusSnapshot(&snapshot) == 0) {
        (void)printf("mode=%u state=%u alarm=%u cycle=%u\n",
                     (unsigned int)snapshot.run_mode,
                     (unsigned int)snapshot.machine_state,
                     (unsigned int)snapshot.alarm_code,
                     (unsigned int)snapshot.cycle_count);
    }

    (void)Api_RequestLogFlush();
    TsService_ExecuteSlice();
    (void)printf("coord_error_checks=%u\n",
                 (unsigned int)RunCoordinateErrorChecks());
    (void)printf("g08_error_checks=%u\n",
                 (unsigned int)RunG08ErrorChecks());
    (void)printf("hpcc_error_checks=%u\n",
                 (unsigned int)RunHpccErrorChecks());
    (void)printf("cycle_error_checks=%u\n",
                 (unsigned int)RunCycleErrorChecks());
    (void)remove(NC_SMOKE_FILE);
    (void)remove(NC_TOOL_EXPIRED_FILE);
    return 0;
}
