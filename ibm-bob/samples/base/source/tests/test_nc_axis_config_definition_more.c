#include "test_common.h"
#include "system_shared.h"
#include "nc_axis_config.h"
extern int32_t NcAxisConfig_SetAxisDefinition(uint8_t axis, uint8_t axisName, uint8_t axisType, uint8_t coordinateGroup);
static int test_lowercase_axis_name_is_normalized(void) {
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(0, NcAxisConfig_SetAxisDefinition(1U, (uint8_t)'b', NC_AXIS_TYPE_ROTARY, 2U));
    TEST_ASSERT_EQ_UINT((uint8_t)'B', g_ncAxisConfigStatus.axis_name[1]);
    TEST_ASSERT_TRUE((g_ncAxisConfigStatus.rotary_axis_mask & (1UL << 1U)) != 0U);
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.axis_definition_calls);
    return 0;
}
static int test_invalid_axis_type_is_rejected(void) {
    SystemShared_Initialize();
    TEST_ASSERT_EQ_INT(-1, NcAxisConfig_SetAxisDefinition(0U, (uint8_t)'X', 9U, 0U));
    TEST_ASSERT_EQ_UINT(1U, g_ncAxisConfigStatus.rejected_calls);
    return 0;
}
int main(void){RUN_TEST(test_lowercase_axis_name_is_normalized);RUN_TEST(test_invalid_axis_type_is_rejected);return 0;}
