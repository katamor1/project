#include "test_common.h"
#include "system_shared.h"
#include "nc_coordinate.h"
extern int32_t NcCoordinate_SetWorkOffsetAxis(uint8_t workIndex, int32_t axisIndex, int32_t offset);
extern int32_t NcCoordinate_SetProgramAxis(int32_t axisIndex, int32_t programPosition, NC_EXEC_BLOCK* pBlock);
static int test_work_offset_affects_machine_target(void){
    NC_EXEC_BLOCK b; SystemShared_Initialize(); NcCoordinate_Reset(); memset(&b,0,sizeof(b));
    TEST_ASSERT_EQ_INT(0,NcCoordinate_SetWorkOffsetAxis(0U,0,1000));
    TEST_ASSERT_EQ_INT(0,NcCoordinate_SetProgramAxis(0,2000,&b));
    TEST_ASSERT_EQ_INT(3000,b.axis_target[0]);
    TEST_ASSERT_TRUE((b.axis_mask & 1U)!=0U);
    return 0;
}
static int test_invalid_work_offset_rejected(void){SystemShared_Initialize();TEST_ASSERT_EQ_INT(-1,NcCoordinate_SetWorkOffsetAxis((uint8_t)NC_WORK_COORD_SYSTEMS,0,1));return 0;}
int main(void){RUN_TEST(test_work_offset_affects_machine_target);RUN_TEST(test_invalid_work_offset_rejected);return 0;}
