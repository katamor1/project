#include "test_common.h"
#include "system_shared.h"
#include "nc_tool_management.h"
static int test_invalid_tool_prepare_is_rejected(void){SystemShared_Initialize();NcToolManagement_Reset();TEST_ASSERT_EQ_INT(-1,NcToolManagement_RequestPrepare(NC_TOOL_TABLE_SIZE));TEST_ASSERT_EQ_UINT(1U,g_ncToolManagementStatus.invalid_tool_events);return 0;}
static int test_set_pocket_then_prepare_success(void){SystemShared_Initialize();NcToolManagement_Reset();TEST_ASSERT_EQ_INT(0,NcToolManagement_SetPocket(7U,3U));TEST_ASSERT_EQ_INT(0,NcToolManagement_RequestPrepare(7U));TEST_ASSERT_EQ_UINT(7U,g_ncToolManagementStatus.requested_tool_no);TEST_ASSERT_EQ_UINT(1U,g_ncToolManagementStatus.prepare_requested);return 0;}
int main(void){RUN_TEST(test_invalid_tool_prepare_is_rejected);RUN_TEST(test_set_pocket_then_prepare_success);return 0;}
