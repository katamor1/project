/* ibm-bob/samples/base/source/tests/test_nc_feature_backlog_reachability.c */
/* Tests that exact deterministic backlog transforms are not starved by legacy matches. */
/* This exists to protect the TS backlog apply order after the v23+ descriptor split. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_feature_backlog.c, ibm-bob/samples/base/source/inc/nc_feature_backlog.h, ibm-bob/samples/base/source/scripts/additive_tests.tsv */
#include "test_common.h"
#include "nc_feature_backlog.h"

static int test_v23_exact_feature_is_not_skipped_after_legacy_matches(void)
{
    NC_EXEC_BLOCK block;
    NC_ERROR_CODE err = NC_ERR_NONE;
    uint16_t id;

    NcFeatureBacklog_Reset();
    for (id = 0U; id < NC_IMPL_BACKLOG_FEATURE_COUNT; ++id) {
        TEST_ASSERT_EQ_INT(0, NcFeatureBacklog_SetFeature(id, 0U, 0U));
    }
    for (id = 0U; id < 4U; ++id) {
        TEST_ASSERT_EQ_INT(0, NcFeatureBacklog_SetFeature(id, 1U, 92U));
    }
    TEST_ASSERT_EQ_INT(0, NcFeatureBacklog_SetFeature(103U, 1U, 5U));

    memset(&block, 0, sizeof(block));
    block.line_no = 730U;
    block.motion_type = NC_MOTION_ADVANCED_INTERP;
    block.g_code10 = 730U;
    block.feed = 1000;

    TEST_ASSERT_EQ_INT(0, NcFeatureBacklog_ApplyBlockTs(&block, &err));
    TEST_ASSERT_EQ_UINT(1U, g_ncImplementationBacklogStatus.feature_hits[103]);
    TEST_ASSERT_EQ_UINT(1103U, block.p_word);
    return 0;
}

int main(void)
{
    RUN_TEST(test_v23_exact_feature_is_not_skipped_after_legacy_matches);
    printf("PASS test_nc_feature_backlog_reachability\n");
    return 0;
}
