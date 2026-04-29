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

static int32_t WriteNcSmokeFile(void)
{
    FILE* pFile = fopen(NC_SMOKE_FILE, "w");

    if (pFile == NULL) {
        return -1;
    }

    (void)fprintf(pFile, "G21 G90\n");
    (void)fprintf(pFile, "G54\n");
    (void)fprintf(pFile, "G10 L2 P1 X0.1 Y0.2 Z0.3\n");
    (void)fprintf(pFile, "G52 X0.01 Y0.02 Z0.03\n");
    (void)fprintf(pFile, "G00 X0.001 Y0.002 Z0.003\n");
    (void)fprintf(pFile, "G04 P1\n");
    (void)fprintf(pFile, "G91\n");
    (void)fprintf(pFile, "G01 X0.001 Y0.001 F60000\n");
    (void)fprintf(pFile, "G90 G20\n");
    (void)fprintf(pFile, "G01 X0.001 Y0.001 Z0.001 F60000\n");
    (void)fprintf(pFile, "G21\n");
    (void)fprintf(pFile, "G92 X0.1354 Y0.2454 Z0.3554\n");
    (void)fprintf(pFile, "G53 G00 X0 Y0 Z0\n");
    (void)fprintf(pFile, "G54\n");
    (void)fprintf(pFile, "G00 X0.02 Y0.03 Z0.04 M130\n");
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

static void SimulateExternalInputs(uint32_t cycle)
{
    if (cycle == 1U) {
        g_ioIn.sensor_bits |= 0x00000008UL; /* READY */
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
    if (cycle == 15U) {
        g_ioIn.analog[0] = 950; /* pressure high */
    }
}

int main(void)
{
    STATUS_SNAPSHOT snapshot;
    NC_PROGRAM_STATUS ncStatus;
    NC_INTERP_STATUS interpStatus;
    NC_FEED_STATUS feedStatus;
    NC_COORDINATE_STATE coordStatus;
    int32_t ncAxis[AXIS_MAX] = {0};
    uint32_t ncMCode = 0U;
    uint32_t interpSegments = 0U;
    uint32_t feedProfile = 0U;
    uint32_t feedOverride = 0U;
    uint32_t dwellTicks = 0U;
    uint32_t coordUnit = 0U;
    uint32_t coordDistance = 0U;
    uint32_t coordWork = 0U;
    uint32_t coordLocal = 0U;
    int32_t coordWorkX = 0;
    uint8_t ncCaptured = 0U;
    uint32_t cycle;

    SystemShared_Initialize();
    if (WriteNcSmokeFile() != 0) {
        return 1;
    }

    for (cycle = 0U; cycle < 16U; ++cycle) {
        SimulateExternalInputs(cycle);
        RtDispatcher_ExecuteCycle();
        TsService_ExecuteSlice();
        if ((ncCaptured == 0U) &&
            (Api_GetNcProgramStatus(&ncStatus) == 0) &&
            (ncStatus.state == NC_COMPLETED)) {
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
            ncCaptured = 1U;
        }
    }

    if (Api_GetNcProgramStatus(&ncStatus) == 0) {
        (void)printf("nc_state=%u nc_exec=%u nc_x=%d nc_y=%d nc_z=%d nc_m=%u\n",
                     (unsigned int)(ncCaptured ? NC_COMPLETED : ncStatus.state),
                     (unsigned int)ncStatus.exec_line_no,
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
    (void)remove(NC_SMOKE_FILE);
    return 0;
}
