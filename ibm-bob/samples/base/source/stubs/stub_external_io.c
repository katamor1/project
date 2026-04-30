#include <string.h>
#include "stub_external_io.h"
#include "nc_motion_filter.h"

void Test_ResetSystem(void)
{
    SystemShared_Initialize();
    NcProgram_ClearBuffer();
    NcMotionFilter_ResetRt();
}

void Test_SetSensorBits(uint32_t sensorBits)
{
    g_ioIn.sensor_bits = sensorBits;
}

void Test_SetCommandBits(uint32_t commandBits)
{
    g_ioOut.command_bits = commandBits;
}

void Test_SetAnalog(uint8_t channel, int32_t value)
{
    if (channel < ANALOG_CH_MAX) {
        g_ioIn.analog[channel] = value;
    }
}

void Test_ClearLogQueue(void)
{
    (void)memset((void*)&g_logQueue, 0, sizeof(g_logQueue));
}

NC_EXEC_BLOCK Test_MakeMotionBlock(uint32_t lineNo,
                                   NC_MOTION_TYPE motionType,
                                   uint32_t axisMask,
                                   int32_t xTarget,
                                   int32_t yTarget,
                                   int32_t zTarget,
                                   int32_t aTarget)
{
    NC_EXEC_BLOCK block;
    (void)memset(&block, 0, sizeof(block));
    block.valid = 1U;
    block.line_no = lineNo;
    block.motion_type = motionType;
    block.axis_mask = axisMask;
    block.axis_target[0] = xTarget;
    block.axis_target[1] = yTarget;
    block.axis_target[2] = zTarget;
    block.axis_target[3] = aTarget;
    block.feed = 1000;
    block.interp_ticks = 10U;
    block.feed_override_percent = NC_FEED_OVERRIDE_DEFAULT_PERCENT;
    block.feed_profile = NC_FEED_PROFILE_TRAPEZOID;
    return block;
}

NC_EXEC_BLOCK Test_MakeToolBlock(uint32_t lineNo,
                                 uint32_t toolNo,
                                 uint32_t auxMCode,
                                 uint32_t auxFlags)
{
    NC_EXEC_BLOCK block = Test_MakeMotionBlock(lineNo, NC_MOTION_AUX_ONLY, 0U, 0, 0, 0, 0);
    block.tool_no = toolNo;
    block.aux_m_code = auxMCode;
    block.aux_flags = auxFlags;
    return block;
}
