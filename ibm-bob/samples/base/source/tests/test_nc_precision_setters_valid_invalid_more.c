#include "test_common.h"
#include "system_shared.h"
#include "nc_precision.h"
extern int32_t NcPrecision_SetLearningControl(uint8_t enabled, int32_t gainPercent, uint8_t memoryWindow);
extern int32_t NcPrecision_SetVibrationControl(uint8_t enabled, uint16_t notchFreqHz, uint16_t dampingPercent);
extern int32_t NcPrecision_SetPreviewControl(uint8_t enabled, uint16_t lookaheadBlocks, uint16_t cornerTolerance);
static int test_precision_setters_accept_boundaries(void){SystemShared_Initialize();NcPrecision_Reset();TEST_ASSERT_EQ_INT(0,NcPrecision_SetLearningControl(1U,NC_PRECISION_LEARNING_GAIN_MAX,NC_PRECISION_LEARNING_MEMORY_MAX));TEST_ASSERT_EQ_INT(0,NcPrecision_SetVibrationControl(1U,NC_PRECISION_VIBRATION_FREQ_MAX_HZ,NC_PRECISION_VIBRATION_DAMPING_MAX));TEST_ASSERT_EQ_INT(0,NcPrecision_SetPreviewControl(1U,NC_PRECISION_PREVIEW_LOOKAHEAD_MAX,NC_PRECISION_PREVIEW_CORNER_TOLERANCE_MAX));TEST_ASSERT_EQ_UINT(1U,g_ncPrecisionStatus.preview_control_enabled);return 0;}
static int test_precision_invalids_rejected(void){SystemShared_Initialize();NcPrecision_Reset();TEST_ASSERT_EQ_INT(-1,NcPrecision_SetLearningControl(1U,NC_PRECISION_LEARNING_GAIN_MAX+1,1U));TEST_ASSERT_EQ_INT(-1,NcPrecision_SetPreviewControl(1U,0U,0U));return 0;}
int main(void){RUN_TEST(test_precision_setters_accept_boundaries);RUN_TEST(test_precision_invalids_rejected);return 0;}
