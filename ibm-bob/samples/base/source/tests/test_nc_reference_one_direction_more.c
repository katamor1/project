#include "test_common.h"
#include "system_shared.h"
#include "nc_reference.h"
extern int32_t NcReference_SetOneDirectionApproach(uint8_t axis, int32_t approachAmount);
static int test_one_direction_approach_rejects_negative(void){SystemShared_Initialize();NcReference_Reset();TEST_ASSERT_EQ_INT(-1,NcReference_SetOneDirectionApproach(0U,-1));return 0;}
static int test_one_direction_approach_records_value(void){SystemShared_Initialize();NcReference_Reset();TEST_ASSERT_EQ_INT(0,NcReference_SetOneDirectionApproach(0U,1234));TEST_ASSERT_EQ_INT(1234,g_ncReferenceStatus.one_direction_approach[0]);return 0;}
int main(void){RUN_TEST(test_one_direction_approach_rejects_negative);RUN_TEST(test_one_direction_approach_records_value);return 0;}
