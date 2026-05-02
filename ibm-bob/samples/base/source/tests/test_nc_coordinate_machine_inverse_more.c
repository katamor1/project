#include "test_common.h"
#include "system_shared.h"
#include "nc_coordinate.h"
extern int32_t NcCoordinate_SetWorkOffsetAxis(uint8_t workIndex, int32_t axisIndex, int32_t offset);
extern int32_t NcCoordinate_SetLocalShiftAxis(int32_t axisIndex, int32_t shift);
extern int32_t NcCoordinate_SetMachineAxis(int32_t axisIndex, int32_t machinePosition, NC_EXEC_BLOCK* pBlock);
static int test_set_machine_axis_calculates_program_position(void){
    NC_EXEC_BLOCK b; SystemShared_Initialize(); NcCoordinate_Reset(); memset(&b,0,sizeof(b));
    TEST_ASSERT_EQ_INT(0,NcCoordinate_SetWorkOffsetAxis(0U,1,100));
    TEST_ASSERT_EQ_INT(0,NcCoordinate_SetLocalShiftAxis(1,20));
    TEST_ASSERT_EQ_INT(0,NcCoordinate_SetMachineAxis(1,500,&b));
    TEST_ASSERT_EQ_INT(380,g_ncCoordinateState.program_position[1]);
    TEST_ASSERT_EQ_INT(500,b.axis_target[1]);
    return 0;
}
int main(void){RUN_TEST(test_set_machine_axis_calculates_program_position);return 0;}
