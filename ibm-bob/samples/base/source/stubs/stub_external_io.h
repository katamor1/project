#ifndef STUB_EXTERNAL_IO_H
#define STUB_EXTERNAL_IO_H

#include <stdint.h>
#include "system_shared.h"
#include "nc_program.h"

/*
 * Unit-test support helpers.
 * These simulate PLC/sensor/motion-controller shared-memory inputs without
 * touching src/ production code. They are intentionally fixed-size and have
 * no dynamic allocation so RT-oriented tests keep the same constraints.
 */
void Test_ResetSystem(void);
void Test_SetSensorBits(uint32_t sensorBits);
void Test_SetCommandBits(uint32_t commandBits);
void Test_SetAnalog(uint8_t channel, int32_t value);
void Test_ClearLogQueue(void);
NC_EXEC_BLOCK Test_MakeMotionBlock(uint32_t lineNo,
                                   NC_MOTION_TYPE motionType,
                                   uint32_t axisMask,
                                   int32_t xTarget,
                                   int32_t yTarget,
                                   int32_t zTarget,
                                   int32_t aTarget);
NC_EXEC_BLOCK Test_MakeToolBlock(uint32_t lineNo,
                                 uint32_t toolNo,
                                 uint32_t auxMCode,
                                 uint32_t auxFlags);

#endif /* STUB_EXTERNAL_IO_H */
