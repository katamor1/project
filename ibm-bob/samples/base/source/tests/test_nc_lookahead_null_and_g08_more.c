#include "test_common.h"
#include "system_shared.h"
#include "nc_lookahead.h"
#include "nc_program.h"
static int test_null_is_noop(void){SystemShared_Initialize();NcLookahead_Reset();NcLookahead_ApplyBlockTs(0);TEST_ASSERT_EQ_UINT(0U,g_ncPathControlStatus.lookahead_blocks);return 0;}
static int test_g08_sets_lookahead_counter(void){NC_EXEC_BLOCK b;SystemShared_Initialize();NcLookahead_Reset();memset(&b,0,sizeof(b));b.g_code10=NC_G_CODE_WHOLE(8);b.p_word=1;b.line_no=11;NcLookahead_ApplyBlockTs(&b);TEST_ASSERT_EQ_UINT(11U,g_ncPathControlStatus.last_line_no);TEST_ASSERT_TRUE(g_ncPathControlStatus.generation>0U);return 0;}
int main(void){RUN_TEST(test_null_is_noop);RUN_TEST(test_g08_sets_lookahead_counter);return 0;}
