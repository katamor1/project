#include "test_common.h"
#include "system_shared.h"
#include "nc_spindle.h"
#include "nc_program.h"
static int test_css_parse_then_stop_rt(void){
    NC_EXEC_BLOCK b; SystemShared_Initialize(); NcSpindle_Reset(); memset(&b,0,sizeof(b));
    b.g_code10=NC_G_CODE_WHOLE(96); b.aux_flags=NC_AUX_FLAG_SPINDLE; b.spindle_speed=200; b.line_no=5; NcSpindle_OnParsedBlockTs(&b);
    TEST_ASSERT_EQ_UINT(1U,g_ncSpindleStatus.css_mode_active);
    memset(&b,0,sizeof(b)); b.aux_flags=NC_AUX_FLAG_M_CODE; b.aux_m_code=5U; b.line_no=6; NcSpindle_OnBlockRt(&b);
    TEST_ASSERT_EQ_UINT(0U,g_ncSpindleStatus.running);
    TEST_ASSERT_EQ_UINT(NC_SPINDLE_DIR_STOP,g_ncSpindleStatus.direction);
    return 0;
}
int main(void){RUN_TEST(test_css_parse_then_stop_rt);return 0;}
