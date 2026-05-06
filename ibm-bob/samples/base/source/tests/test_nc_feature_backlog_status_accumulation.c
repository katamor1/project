/* ibm-bob/samples/base/source/tests/test_nc_feature_backlog_status_accumulation.c */
/* Tests the v45 feature-backlog aggregate contracts for v35-v41 modules. */
/* This exists to keep v45 regression coverage in one standalone executable. */
/* RELEVANT FILES: ibm-bob/samples/base/source/scripts/additive_tests.tsv, ibm-bob/samples/base/source/src/nc_feature_backlog_process_safety.c, ibm-bob/samples/base/source/tests/test_nc_feature_backlog_reset_idempotence.c */
/*
 * IBM-Bob additional unit tests v45_40.
 *
 * Adds 40 unit tests for backlog modules v35-v41.
 * Coverage split:
 *   - v35-v39: 6 tests each = 30
 *   - v40-v41: 5 tests each = 10
 * Total: 40 tests.
 *
 * Focus points:
 *   1) status snapshot copy independence
 *   2) non-contiguous enable mask/count consistency
 *   3) repeated TS application hit accumulation
 *   4) repeated zero-input RT tick contract
 *   5) valid feature-name resolution for all IDs
 *   6) disabled TS path does not record hits or side-effect masks
 */
#include "nc_feature_backlog_aux_safety.h"
#include "nc_feature_backlog_lube_brake_thermal.h"
#include "nc_feature_backlog_readiness_sensors.h"
#include "nc_feature_backlog_pallet_fixture_safety.h"
#include "nc_feature_backlog_tool_utility_monitors.h"
#include "nc_feature_backlog_process_safety.h"
#include "nc_feature_backlog_service_interlocks.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(__GNUC__) || defined(__clang__)
#define IBM_BOB_UNUSED __attribute__((unused))
#else
#define IBM_BOB_UNUSED
#endif

static uint32_t g_tests_run = 0u;
static uint32_t g_tests_failed = 0u;

static void test_begin(const char *name)
{
    g_tests_run++;
    printf("[TEST %02u] %s\n", g_tests_run, name);
}

static void fail_u32(const char *expr, uint32_t actual, uint32_t expected, const char *file, int line)
{
    g_tests_failed++;
    printf("  FAIL %s:%d %s actual=%u expected=%u\n", file, line, expr, actual, expected);
}

static void fail_i32(const char *expr, int32_t actual, int32_t expected, const char *file, int line)
{
    g_tests_failed++;
    printf("  FAIL %s:%d %s actual=%d expected=%d\n", file, line, expr, actual, expected);
}

static void fail_true(const char *expr, const char *file, int line)
{
    g_tests_failed++;
    printf("  FAIL %s:%d %s\n", file, line, expr);
}

#define EXPECT_EQ_U32(actual_, expected_) do { \
    uint32_t a__ = (uint32_t)(actual_); \
    uint32_t e__ = (uint32_t)(expected_); \
    if (a__ != e__) { fail_u32(#actual_, a__, e__, __FILE__, __LINE__); } \
} while (0)

#define EXPECT_EQ_I32(actual_, expected_) do { \
    int32_t a__ = (int32_t)(actual_); \
    int32_t e__ = (int32_t)(expected_); \
    if (a__ != e__) { fail_i32(#actual_, a__, e__, __FILE__, __LINE__); } \
} while (0)

#define EXPECT_TRUE(expr_) do { \
    if (!(expr_)) { fail_true(#expr_, __FILE__, __LINE__); } \
} while (0)

#define EXPECT_NONZERO_U32(value_) do { \
    if ((uint32_t)(value_) == 0u) { fail_true(#value_ " != 0", __FILE__, __LINE__); } \
} while (0)

#define DEFINE_V45_COMMON_TESTS(VNUM) \
static uint32_t sum_hits_v##VNUM(const NC_FEATURE_BACKLOG_V##VNUM##_STATUS *status) \
{ \
    uint32_t i; \
    uint32_t sum = 0u; \
    for (i = 0u; i < NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT; ++i) { \
        sum += status->per_feature_hits[i]; \
    } \
    return sum; \
} \
static void fill_block_v##VNUM(NC_FEATURE_BACKLOG_V##VNUM##_BLOCK *block, int32_t base_value) \
{ \
    uint32_t i; \
    memset(block, 0, sizeof(*block)); \
    block->feed_override_percent = 100; \
    for (i = 0u; i < NC_FEATURE_BACKLOG_V##VNUM##_AXIS_COUNT; ++i) { \
        block->axis_target[i] = base_value + (int32_t)i; \
    } \
} \
static void test_v##VNUM##_v45_01_status_snapshot_independent_after_mutation(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS before_status; \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS after_status; \
    test_begin("v" #VNUM " status snapshot independent after mutation"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&before_status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    Api_EnableAllNcFeatureBacklogV##VNUM##Features(); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&after_status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(before_status.configured_count, 0u); \
    EXPECT_EQ_U32(before_status.enabled_mask, 0u); \
    EXPECT_EQ_U32(after_status.configured_count, NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT); \
    EXPECT_NONZERO_U32(after_status.enabled_mask); \
    EXPECT_EQ_U32(before_status.implemented_count, after_status.implemented_count); \
} \
static void test_v##VNUM##_v45_02_non_contiguous_enable_mask(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    uint32_t base = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID; \
    uint32_t mid_index = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT / 2u; \
    uint32_t last_index = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT - 1u; \
    uint32_t expected_mask = (1u << 0u) | (1u << last_index); \
    test_begin("v" #VNUM " non-contiguous enable mask"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    EXPECT_EQ_U32(Api_SetNcFeatureBacklogV##VNUM##Feature(base + 0u, 1u), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(Api_SetNcFeatureBacklogV##VNUM##Feature(base + mid_index, 1u), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(Api_SetNcFeatureBacklogV##VNUM##Feature(base + last_index, 1u), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(Api_SetNcFeatureBacklogV##VNUM##Feature(base + mid_index, 0u), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.configured_count, 2u); \
    EXPECT_EQ_U32(status.enabled_mask, expected_mask); \
    EXPECT_EQ_U32(NcFeatureBacklogV##VNUM##_IsFeatureEnabled(base + 0u), 1u); \
    EXPECT_EQ_U32(NcFeatureBacklogV##VNUM##_IsFeatureEnabled(base + mid_index), 0u); \
    EXPECT_EQ_U32(NcFeatureBacklogV##VNUM##_IsFeatureEnabled(base + last_index), 1u); \
} \
static void test_v##VNUM##_v45_03_repeated_ts_apply_accumulates_hits(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_BLOCK first_block; \
    NC_FEATURE_BACKLOG_V##VNUM##_BLOCK second_block; \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    test_begin("v" #VNUM " repeated TS apply accumulates hits"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    Api_EnableAllNcFeatureBacklogV##VNUM##Features(); \
    fill_block_v##VNUM(&first_block, (int32_t)(VNUM * 1000)); \
    fill_block_v##VNUM(&second_block, (int32_t)(VNUM * 1000 + 25)); \
    NcFeatureBacklogV##VNUM##_ApplyBlockTs(&first_block); \
    NcFeatureBacklogV##VNUM##_ApplyBlockTs(&second_block); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.ts_apply_count, 2u); \
    EXPECT_EQ_U32(status.warning_count, 0u); \
    EXPECT_EQ_U32(sum_hits_v##VNUM(&status), NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT * 2u); \
    EXPECT_EQ_U32(status.per_feature_hits[0], 2u); \
    EXPECT_EQ_U32(status.per_feature_hits[NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT - 1u], 2u); \
    EXPECT_EQ_I32(status.last_x, second_block.axis_target[0]); \
} \
static void test_v##VNUM##_v45_04_repeated_zero_rt_ticks_count_contract(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_RT_IO io; \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    uint32_t i; \
    test_begin("v" #VNUM " repeated zero RT ticks count contract"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    Api_EnableAllNcFeatureBacklogV##VNUM##Features(); \
    memset(&io, 0, sizeof(io)); \
    for (i = 0u; i < 3u; ++i) { \
        io.cycle_counter = 100u + i; \
        NcFeatureBacklogV##VNUM##_RunRtTick(&io); \
    } \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.rt_tick_count, 3u); \
    EXPECT_EQ_U32(status.warning_count, 0u); \
    EXPECT_EQ_U32(status.configured_count, NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT); \
} \
static void test_v##VNUM##_v45_05_all_valid_feature_names_resolve(void) \
{ \
    uint32_t i; \
    uint32_t base = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID; \
    test_begin("v" #VNUM " all valid feature names resolve"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    for (i = 0u; i < NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT; ++i) { \
        const char *name = NcFeatureBacklogV##VNUM##_GetFeatureName(base + i); \
        EXPECT_TRUE(name != (const char *)0); \
        EXPECT_TRUE(strcmp(name, "unknown") != 0); \
        EXPECT_TRUE(strlen(name) > 0u); \
    } \
    EXPECT_TRUE(strcmp(NcFeatureBacklogV##VNUM##_GetFeatureName(base + NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT), "unknown") == 0); \
} \
static IBM_BOB_UNUSED void test_v##VNUM##_v45_06_disabled_ts_has_no_hits_or_masks(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_BLOCK block; \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    test_begin("v" #VNUM " disabled TS path has no hits or masks"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    fill_block_v##VNUM(&block, (int32_t)(VNUM * 1000 + 50)); \
    NcFeatureBacklogV##VNUM##_ApplyBlockTs(&block); \
    EXPECT_EQ_U32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.ts_apply_count, 1u); \
    EXPECT_EQ_U32(status.configured_count, 0u); \
    EXPECT_EQ_U32(sum_hits_v##VNUM(&status), 0u); \
    EXPECT_EQ_U32(block.aux_request_bits, 0u); \
    EXPECT_EQ_U32(block.hold_request, 0u); \
    EXPECT_EQ_U32(block.interlock_mask, 0u); \
    EXPECT_EQ_U32(block.sequence_gate_mask, 0u); \
    EXPECT_EQ_U32(block.diagnostics_mask, 0u); \
    EXPECT_EQ_U32(block.trace_event_code, 0u); \
}

DEFINE_V45_COMMON_TESTS(35)
DEFINE_V45_COMMON_TESTS(36)
DEFINE_V45_COMMON_TESTS(37)
DEFINE_V45_COMMON_TESTS(38)
DEFINE_V45_COMMON_TESTS(39)
DEFINE_V45_COMMON_TESTS(40)
DEFINE_V45_COMMON_TESTS(41)

int main(void)
{
    test_v35_v45_01_status_snapshot_independent_after_mutation();
    test_v35_v45_02_non_contiguous_enable_mask();
    test_v35_v45_03_repeated_ts_apply_accumulates_hits();
    test_v35_v45_04_repeated_zero_rt_ticks_count_contract();
    test_v35_v45_05_all_valid_feature_names_resolve();
    test_v35_v45_06_disabled_ts_has_no_hits_or_masks();

    test_v36_v45_01_status_snapshot_independent_after_mutation();
    test_v36_v45_02_non_contiguous_enable_mask();
    test_v36_v45_03_repeated_ts_apply_accumulates_hits();
    test_v36_v45_04_repeated_zero_rt_ticks_count_contract();
    test_v36_v45_05_all_valid_feature_names_resolve();
    test_v36_v45_06_disabled_ts_has_no_hits_or_masks();

    test_v37_v45_01_status_snapshot_independent_after_mutation();
    test_v37_v45_02_non_contiguous_enable_mask();
    test_v37_v45_03_repeated_ts_apply_accumulates_hits();
    test_v37_v45_04_repeated_zero_rt_ticks_count_contract();
    test_v37_v45_05_all_valid_feature_names_resolve();
    test_v37_v45_06_disabled_ts_has_no_hits_or_masks();

    test_v38_v45_01_status_snapshot_independent_after_mutation();
    test_v38_v45_02_non_contiguous_enable_mask();
    test_v38_v45_03_repeated_ts_apply_accumulates_hits();
    test_v38_v45_04_repeated_zero_rt_ticks_count_contract();
    test_v38_v45_05_all_valid_feature_names_resolve();
    test_v38_v45_06_disabled_ts_has_no_hits_or_masks();

    test_v39_v45_01_status_snapshot_independent_after_mutation();
    test_v39_v45_02_non_contiguous_enable_mask();
    test_v39_v45_03_repeated_ts_apply_accumulates_hits();
    test_v39_v45_04_repeated_zero_rt_ticks_count_contract();
    test_v39_v45_05_all_valid_feature_names_resolve();
    test_v39_v45_06_disabled_ts_has_no_hits_or_masks();

    test_v40_v45_01_status_snapshot_independent_after_mutation();
    test_v40_v45_02_non_contiguous_enable_mask();
    test_v40_v45_03_repeated_ts_apply_accumulates_hits();
    test_v40_v45_04_repeated_zero_rt_ticks_count_contract();
    test_v40_v45_05_all_valid_feature_names_resolve();

    test_v41_v45_01_status_snapshot_independent_after_mutation();
    test_v41_v45_02_non_contiguous_enable_mask();
    test_v41_v45_03_repeated_ts_apply_accumulates_hits();
    test_v41_v45_04_repeated_zero_rt_ticks_count_contract();
    test_v41_v45_05_all_valid_feature_names_resolve();

    printf("UNIT_TEST_SUMMARY tests=%u failures=%u\n", g_tests_run, g_tests_failed);
    if (g_tests_run != 40u) {
        printf("FAILED expected exactly 40 test cases\n");
        return 1;
    }
    if (g_tests_failed != 0u) {
        printf("FAILED failures=%u\n", g_tests_failed);
        return 1;
    }
    printf("PASS additional_unit_tests_v45_40\n");
    return 0;
}
