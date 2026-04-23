#include <stdio.h>
#include "system_shared.h"
#include "control_api.h"
#include "rt_control.h"
#include "ts_service.h"

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
        g_ioIn.sensor_bits |= 0x00000010UL; /* RUN */
    }
    if (cycle == 12U) {
        g_ioIn.analog[0] = 950; /* pressure high */
    }
}

int main(void)
{
    STATUS_SNAPSHOT snapshot;
    uint32_t cycle;

    SystemShared_Initialize();

    for (cycle = 0U; cycle < 16U; ++cycle) {
        SimulateExternalInputs(cycle);
        RtDispatcher_ExecuteCycle();
        TsService_ExecuteSlice();
    }

    if (Api_GetStatusSnapshot(&snapshot) == 0) {
        (void)printf("mode=%u state=%u alarm=%u cycle=%u\n",
                     (unsigned int)snapshot.run_mode,
                     (unsigned int)snapshot.machine_state,
                     (unsigned int)snapshot.alarm_code,
                     (unsigned int)snapshot.cycle_count);
    }

    (void)Api_RequestLogFlush();
    TsService_ExecuteSlice();
    return 0;
}
