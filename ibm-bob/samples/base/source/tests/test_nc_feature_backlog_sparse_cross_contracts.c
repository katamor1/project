/*
 * IBM-Bob additional unit tests v47_40.
 *
 * Adds 40 more unit tests for backlog modules v35-v41.
 * This set focuses on test angles not covered by v43-v46:
 *   1) sparse feature enable masks and TS hit isolation
 *   2) last-feature-only TS application and last_feature_id contract
 *   3) feed override lower/upper safety after TS adjustments
 *   4) range-error no-mutation contracts
 *   5) full valid-name coverage for every implemented feature ID
 *   6) cross-module self-check, zero-enabled TS, and first/last enable contracts
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

static uint32_t expected_sparse_count(uint32_t feature_count)
{
    uint32_t i;
    uint32_t count = 0u;
    for (i = 0u; i < feature_count; ++i) {
        if ((i % 2u) == 0u) {
            count++;
        }
    }
    return count;
}

#define DEFINE_V47_COMMON_TESTS(VNUM) \
static uint32_t sum_hits_v##VNUM(const NC_FEATURE_BACKLOG_V##VNUM##_STATUS *status) \
{ \
    uint32_t i; \
    uint32_t sum = 0u; \
    for (i = 0u; i < NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT; ++i) { \
        sum += status->per_feature_hits[i]; \
    } \
    return sum; \
} \
static void fill_block_v##VNUM(NC_FEATURE_BACKLOG_V##VNUM##_BLOCK *block, int32_t base_value, int32_t feed_override) \
{ \
    uint32_t i; \
    memset(block, 0, sizeof(*block)); \
    block->feed_override_percent = feed_override; \
    for (i = 0u; i < NC_FEATURE_BACKLOG_V##VNUM##_AXIS_COUNT; ++i) { \
        block->axis_target[i] = base_value + (int32_t)(i * 11u); \
    } \
} \
static void test_v##VNUM##_v47_01_sparse_enable_ts_hit_isolation(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_BLOCK block; \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    uint32_t i; \
    uint32_t expected_count = expected_sparse_count(NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT); \
    uint32_t base = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID; \
    test_begin("v" #VNUM " sparse enable TS hit isolation"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    for (i = 0u; i < NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT; ++i) { \
        if ((i % 2u) == 0u) { \
            EXPECT_EQ_I32(Api_SetNcFeatureBacklogV##VNUM##Feature(base + i, 1u), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
        } \
    } \
    fill_block_v##VNUM(&block, (int32_t)(VNUM * 1000 + 470), 100); \
    NcFeatureBacklogV##VNUM##_ApplyBlockTs(&block); \
    EXPECT_EQ_I32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.configured_count, expected_count); \
    EXPECT_EQ_U32(status.ts_apply_count, 1u); \
    EXPECT_EQ_U32(sum_hits_v##VNUM(&status), expected_count); \
    for (i = 0u; i < NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT; ++i) { \
        EXPECT_EQ_U32(status.per_feature_hits[i], ((i % 2u) == 0u) ? 1u : 0u); \
    } \
} \
static void test_v##VNUM##_v47_02_last_feature_only_updates_last_feature_id(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_BLOCK block; \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    uint32_t base = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID; \
    uint32_t last = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT - 1u; \
    test_begin("v" #VNUM " last feature only updates last_feature_id"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    EXPECT_EQ_I32(Api_SetNcFeatureBacklogV##VNUM##Feature(base + last, 1u), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    fill_block_v##VNUM(&block, (int32_t)(VNUM * 1000 + 500), 100); \
    NcFeatureBacklogV##VNUM##_ApplyBlockTs(&block); \
    EXPECT_EQ_I32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(status.configured_count, 1u); \
    EXPECT_EQ_U32(status.last_feature_id, base + last); \
    EXPECT_EQ_U32(status.per_feature_hits[last], 1u); \
    EXPECT_EQ_U32(sum_hits_v##VNUM(&status), 1u); \
    EXPECT_TRUE(status.last_policy <= NC_FEATURE_BACKLOG_V##VNUM##_POLICY_TRACE); \
} \
static void test_v##VNUM##_v47_03_feed_override_bounds_after_ts_adjustment(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_BLOCK block; \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS status; \
    test_begin("v" #VNUM " feed override remains inside safe bounds"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    Api_EnableAllNcFeatureBacklogV##VNUM##Features(); \
    fill_block_v##VNUM(&block, (int32_t)(VNUM * 1000 + 530), 1); \
    NcFeatureBacklogV##VNUM##_ApplyBlockTs(&block); \
    EXPECT_EQ_I32(Api_GetNcFeatureBacklogV##VNUM##Status(&status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_TRUE(block.feed_override_percent >= 5); \
    EXPECT_TRUE(block.feed_override_percent <= 100); \
    EXPECT_TRUE(status.last_feed_override_percent >= 5); \
    EXPECT_TRUE(status.last_feed_override_percent <= 100); \
} \
static void test_v##VNUM##_v47_04_range_errors_do_not_mutate_status(void) \
{ \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS before_status; \
    NC_FEATURE_BACKLOG_V##VNUM##_STATUS after_status; \
    uint32_t base = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID; \
    test_begin("v" #VNUM " range errors do not mutate status"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    EXPECT_EQ_I32(Api_SetNcFeatureBacklogV##VNUM##Feature(base, 1u), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_I32(Api_GetNcFeatureBacklogV##VNUM##Status(&before_status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_I32(Api_SetNcFeatureBacklogV##VNUM##Feature(base - 1u, 1u), NC_FEATURE_BACKLOG_V##VNUM##_ERR_RANGE); \
    EXPECT_EQ_I32(Api_SetNcFeatureBacklogV##VNUM##Feature(base + NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT, 1u), NC_FEATURE_BACKLOG_V##VNUM##_ERR_RANGE); \
    EXPECT_EQ_I32(Api_GetNcFeatureBacklogV##VNUM##Status(&after_status), NC_FEATURE_BACKLOG_V##VNUM##_OK); \
    EXPECT_EQ_U32(after_status.configured_count, before_status.configured_count); \
    EXPECT_EQ_U32(after_status.enabled_mask, before_status.enabled_mask); \
    EXPECT_EQ_U32(after_status.warning_count, before_status.warning_count); \
    EXPECT_EQ_U32(after_status.ts_apply_count, before_status.ts_apply_count); \
} \
static void test_v##VNUM##_v47_05_all_valid_feature_names_are_resolved(void) \
{ \
    uint32_t i; \
    uint32_t base = NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_BASE_ID; \
    test_begin("v" #VNUM " all valid feature names are resolved"); \
    NcFeatureBacklogV##VNUM##_Init(); \
    for (i = 0u; i < NC_FEATURE_BACKLOG_V##VNUM##_FEATURE_COUNT; ++i) { \
        const char *name = NcFeatureBacklogV##VNUM##_GetFeatureName(base + i); \
        EXPECT_TRUE(name != (const char *)0); \
        EXPECT_TRUE(strcmp(name, "unknown") != 0); \
        EXPECT_TRUE(strlen(name) > 4u); \
    } \
}

DEFINE_V47_COMMON_TESTS(35)
DEFINE_V47_COMMON_TESTS(36)
DEFINE_V47_COMMON_TESTS(37)
DEFINE_V47_COMMON_TESTS(38)
DEFINE_V47_COMMON_TESTS(39)
DEFINE_V47_COMMON_TESTS(40)
DEFINE_V47_COMMON_TESTS(41)

static void cross_v47_36_self_check_contracts(void)
{
    NC_FEATURE_BACKLOG_V35_STATUS s35;
    NC_FEATURE_BACKLOG_V36_STATUS s36;
    NC_FEATURE_BACKLOG_V37_STATUS s37;
    NC_FEATURE_BACKLOG_V38_STATUS s38;
    NC_FEATURE_BACKLOG_V39_STATUS s39;
    NC_FEATURE_BACKLOG_V40_STATUS s40;
    NC_FEATURE_BACKLOG_V41_STATUS s41;
    test_begin("cross self-check contracts leave active evidence");
    NcFeatureBacklogV38_Init();
    NcFeatureBacklogV39_Init();
    NcFeatureBacklogV40_Init();
    NcFeatureBacklogV41_Init();
    EXPECT_EQ_I32(Api_RunNcFeatureBacklogV35SelfCheck(), NC_FEATURE_BACKLOG_V35_OK);
    EXPECT_EQ_I32(Api_RunNcFeatureBacklogV36SelfCheck(), NC_FEATURE_BACKLOG_V36_OK);
    EXPECT_EQ_I32(Api_RunNcFeatureBacklogV37SelfCheck(), NC_FEATURE_BACKLOG_V37_OK);
    EXPECT_EQ_I32(Api_RunNcFeatureBacklogV38SelfCheck(), NC_FEATURE_BACKLOG_V38_OK);
    EXPECT_EQ_I32(Api_RunNcFeatureBacklogV39SelfCheck(), NC_FEATURE_BACKLOG_V39_OK);
    EXPECT_EQ_I32(Api_RunNcFeatureBacklogV40SelfCheck(), NC_FEATURE_BACKLOG_V40_OK);
    EXPECT_EQ_I32(Api_RunNcFeatureBacklogV41SelfCheck(), NC_FEATURE_BACKLOG_V41_OK);
    EXPECT_EQ_I32(Api_GetNcFeatureBacklogV35Status(&s35), NC_FEATURE_BACKLOG_V35_OK);
    EXPECT_EQ_I32(Api_GetNcFeatureBacklogV36Status(&s36), NC_FEATURE_BACKLOG_V36_OK);
    EXPECT_EQ_I32(Api_GetNcFeatureBacklogV37Status(&s37), NC_FEATURE_BACKLOG_V37_OK);
    EXPECT_EQ_I32(Api_GetNcFeatureBacklogV38Status(&s38), NC_FEATURE_BACKLOG_V38_OK);
    EXPECT_EQ_I32(Api_GetNcFeatureBacklogV39Status(&s39), NC_FEATURE_BACKLOG_V39_OK);
    EXPECT_EQ_I32(Api_GetNcFeatureBacklogV40Status(&s40), NC_FEATURE_BACKLOG_V40_OK);
    EXPECT_EQ_I32(Api_GetNcFeatureBacklogV41Status(&s41), NC_FEATURE_BACKLOG_V41_OK);
    EXPECT_TRUE(s35.ts_apply_count > 0u && s35.rt_tick_count > 0u);
    EXPECT_TRUE(s36.ts_apply_count > 0u && s36.rt_tick_count > 0u);
    EXPECT_TRUE(s37.ts_apply_count > 0u && s37.rt_tick_count > 0u);
    EXPECT_EQ_U32(s38.implemented_count, NC_FEATURE_BACKLOG_V38_FEATURE_COUNT);
    EXPECT_EQ_U32(s39.implemented_count, NC_FEATURE_BACKLOG_V39_FEATURE_COUNT);
    EXPECT_EQ_U32(s40.implemented_count, NC_FEATURE_BACKLOG_V40_FEATURE_COUNT);
    EXPECT_EQ_U32(s41.implemented_count, NC_FEATURE_BACKLOG_V41_FEATURE_COUNT);
}

static void cross_v47_37_zero_enabled_ts_has_no_feature_hits(void)
{
    NC_FEATURE_BACKLOG_V35_BLOCK b35;
    NC_FEATURE_BACKLOG_V36_BLOCK b36;
    NC_FEATURE_BACKLOG_V37_BLOCK b37;
    NC_FEATURE_BACKLOG_V38_BLOCK b38;
    NC_FEATURE_BACKLOG_V39_BLOCK b39;
    NC_FEATURE_BACKLOG_V40_BLOCK b40;
    NC_FEATURE_BACKLOG_V41_BLOCK b41;
    NC_FEATURE_BACKLOG_V35_STATUS s35;
    NC_FEATURE_BACKLOG_V36_STATUS s36;
    NC_FEATURE_BACKLOG_V37_STATUS s37;
    NC_FEATURE_BACKLOG_V38_STATUS s38;
    NC_FEATURE_BACKLOG_V39_STATUS s39;
    NC_FEATURE_BACKLOG_V40_STATUS s40;
    NC_FEATURE_BACKLOG_V41_STATUS s41;
    test_begin("cross zero-enabled TS has no feature hits");
    NcFeatureBacklogV35_Init(); fill_block_v35(&b35, 3500, 100); NcFeatureBacklogV35_ApplyBlockTs(&b35); Api_GetNcFeatureBacklogV35Status(&s35);
    NcFeatureBacklogV36_Init(); fill_block_v36(&b36, 3600, 100); NcFeatureBacklogV36_ApplyBlockTs(&b36); Api_GetNcFeatureBacklogV36Status(&s36);
    NcFeatureBacklogV37_Init(); fill_block_v37(&b37, 3700, 100); NcFeatureBacklogV37_ApplyBlockTs(&b37); Api_GetNcFeatureBacklogV37Status(&s37);
    NcFeatureBacklogV38_Init(); fill_block_v38(&b38, 3800, 100); NcFeatureBacklogV38_ApplyBlockTs(&b38); Api_GetNcFeatureBacklogV38Status(&s38);
    NcFeatureBacklogV39_Init(); fill_block_v39(&b39, 3900, 100); NcFeatureBacklogV39_ApplyBlockTs(&b39); Api_GetNcFeatureBacklogV39Status(&s39);
    NcFeatureBacklogV40_Init(); fill_block_v40(&b40, 4000, 100); NcFeatureBacklogV40_ApplyBlockTs(&b40); Api_GetNcFeatureBacklogV40Status(&s40);
    NcFeatureBacklogV41_Init(); fill_block_v41(&b41, 4100, 100); NcFeatureBacklogV41_ApplyBlockTs(&b41); Api_GetNcFeatureBacklogV41Status(&s41);
    EXPECT_EQ_U32(s35.ts_apply_count, 1u); EXPECT_EQ_U32(sum_hits_v35(&s35), 0u);
    EXPECT_EQ_U32(s36.ts_apply_count, 1u); EXPECT_EQ_U32(sum_hits_v36(&s36), 0u);
    EXPECT_EQ_U32(s37.ts_apply_count, 1u); EXPECT_EQ_U32(sum_hits_v37(&s37), 0u);
    EXPECT_EQ_U32(s38.ts_apply_count, 1u); EXPECT_EQ_U32(sum_hits_v38(&s38), 0u);
    EXPECT_EQ_U32(s39.ts_apply_count, 1u); EXPECT_EQ_U32(sum_hits_v39(&s39), 0u);
    EXPECT_EQ_U32(s40.ts_apply_count, 1u); EXPECT_EQ_U32(sum_hits_v40(&s40), 0u);
    EXPECT_EQ_U32(s41.ts_apply_count, 1u); EXPECT_EQ_U32(sum_hits_v41(&s41), 0u);
}

static void cross_v47_38_first_last_enable_contracts(void)
{
    test_begin("cross first/last enable contracts");
    NcFeatureBacklogV35_Init(); Api_EnableAllNcFeatureBacklogV35Features(); Api_SetNcFeatureBacklogV35Feature(NC_FEATURE_BACKLOG_V35_FEATURE_BASE_ID, 0u);
    NcFeatureBacklogV36_Init(); Api_EnableAllNcFeatureBacklogV36Features(); Api_SetNcFeatureBacklogV36Feature(NC_FEATURE_BACKLOG_V36_FEATURE_BASE_ID, 0u);
    NcFeatureBacklogV37_Init(); Api_EnableAllNcFeatureBacklogV37Features(); Api_SetNcFeatureBacklogV37Feature(NC_FEATURE_BACKLOG_V37_FEATURE_BASE_ID, 0u);
    NcFeatureBacklogV38_Init(); Api_EnableAllNcFeatureBacklogV38Features(); Api_SetNcFeatureBacklogV38Feature(NC_FEATURE_BACKLOG_V38_FEATURE_BASE_ID, 0u);
    NcFeatureBacklogV39_Init(); Api_EnableAllNcFeatureBacklogV39Features(); Api_SetNcFeatureBacklogV39Feature(NC_FEATURE_BACKLOG_V39_FEATURE_BASE_ID, 0u);
    NcFeatureBacklogV40_Init(); Api_EnableAllNcFeatureBacklogV40Features(); Api_SetNcFeatureBacklogV40Feature(NC_FEATURE_BACKLOG_V40_FEATURE_BASE_ID, 0u);
    NcFeatureBacklogV41_Init(); Api_EnableAllNcFeatureBacklogV41Features(); Api_SetNcFeatureBacklogV41Feature(NC_FEATURE_BACKLOG_V41_FEATURE_BASE_ID, 0u);
    EXPECT_EQ_U32(NcFeatureBacklogV35_IsFeatureEnabled(NC_FEATURE_BACKLOG_V35_FEATURE_BASE_ID), 0u);
    EXPECT_EQ_U32(NcFeatureBacklogV36_IsFeatureEnabled(NC_FEATURE_BACKLOG_V36_FEATURE_BASE_ID), 0u);
    EXPECT_EQ_U32(NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_BACKLOG_V37_FEATURE_BASE_ID), 0u);
    EXPECT_EQ_U32(NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_BACKLOG_V38_FEATURE_BASE_ID), 0u);
    EXPECT_EQ_U32(NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_BACKLOG_V39_FEATURE_BASE_ID), 0u);
    EXPECT_EQ_U32(NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_BACKLOG_V40_FEATURE_BASE_ID), 0u);
    EXPECT_EQ_U32(NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_BACKLOG_V41_FEATURE_BASE_ID), 0u);
    EXPECT_EQ_U32(NcFeatureBacklogV35_IsFeatureEnabled(NC_FEATURE_BACKLOG_V35_FEATURE_BASE_ID + NC_FEATURE_BACKLOG_V35_FEATURE_COUNT - 1u), 1u);
    EXPECT_EQ_U32(NcFeatureBacklogV36_IsFeatureEnabled(NC_FEATURE_BACKLOG_V36_FEATURE_BASE_ID + NC_FEATURE_BACKLOG_V36_FEATURE_COUNT - 1u), 1u);
    EXPECT_EQ_U32(NcFeatureBacklogV37_IsFeatureEnabled(NC_FEATURE_BACKLOG_V37_FEATURE_BASE_ID + NC_FEATURE_BACKLOG_V37_FEATURE_COUNT - 1u), 1u);
    EXPECT_EQ_U32(NcFeatureBacklogV38_IsFeatureEnabled(NC_FEATURE_BACKLOG_V38_FEATURE_BASE_ID + NC_FEATURE_BACKLOG_V38_FEATURE_COUNT - 1u), 1u);
    EXPECT_EQ_U32(NcFeatureBacklogV39_IsFeatureEnabled(NC_FEATURE_BACKLOG_V39_FEATURE_BASE_ID + NC_FEATURE_BACKLOG_V39_FEATURE_COUNT - 1u), 1u);
    EXPECT_EQ_U32(NcFeatureBacklogV40_IsFeatureEnabled(NC_FEATURE_BACKLOG_V40_FEATURE_BASE_ID + NC_FEATURE_BACKLOG_V40_FEATURE_COUNT - 1u), 1u);
    EXPECT_EQ_U32(NcFeatureBacklogV41_IsFeatureEnabled(NC_FEATURE_BACKLOG_V41_FEATURE_BASE_ID + NC_FEATURE_BACKLOG_V41_FEATURE_COUNT - 1u), 1u);
}

static void cross_v47_39_trace_base_names_and_invalid_state(void)
{
    test_begin("cross invalid state and trace base sanity");
    NcFeatureBacklogV35_Init();
    NcFeatureBacklogV36_Init();
    NcFeatureBacklogV37_Init();
    NcFeatureBacklogV38_Init();
    NcFeatureBacklogV39_Init();
    NcFeatureBacklogV40_Init();
    NcFeatureBacklogV41_Init();
    EXPECT_TRUE(NC_FEATURE_BACKLOG_V35_TRACE_BASE < NC_FEATURE_BACKLOG_V36_TRACE_BASE);
    EXPECT_TRUE(NC_FEATURE_BACKLOG_V36_TRACE_BASE < NC_FEATURE_BACKLOG_V37_TRACE_BASE);
    EXPECT_TRUE(NC_FEATURE_BACKLOG_V37_TRACE_BASE < NC_FEATURE_BACKLOG_V38_TRACE_BASE);
    EXPECT_TRUE(NC_FEATURE_BACKLOG_V38_TRACE_BASE < NC_FEATURE_BACKLOG_V39_TRACE_BASE);
    EXPECT_TRUE(NC_FEATURE_BACKLOG_V39_TRACE_BASE < NC_FEATURE_BACKLOG_V40_TRACE_BASE);
    EXPECT_TRUE(NC_FEATURE_BACKLOG_V40_TRACE_BASE < NC_FEATURE_BACKLOG_V41_TRACE_BASE);
    EXPECT_TRUE(strcmp(NcFeatureBacklogV35_GetFeatureName(0u), "unknown") == 0);
    EXPECT_TRUE(strcmp(NcFeatureBacklogV36_GetFeatureName(0u), "unknown") == 0);
    EXPECT_TRUE(strcmp(NcFeatureBacklogV37_GetFeatureName(0u), "unknown") == 0);
    EXPECT_TRUE(strcmp(NcFeatureBacklogV38_GetFeatureName(0u), "unknown") == 0);
    EXPECT_TRUE(strcmp(NcFeatureBacklogV39_GetFeatureName(0u), "unknown") == 0);
    EXPECT_TRUE(strcmp(NcFeatureBacklogV40_GetFeatureName(0u), "unknown") == 0);
    EXPECT_TRUE(strcmp(NcFeatureBacklogV41_GetFeatureName(0u), "unknown") == 0);
}

static void cross_v47_40_implemented_counts_match_constants_after_init(void)
{
    NC_FEATURE_BACKLOG_V35_STATUS s35;
    NC_FEATURE_BACKLOG_V36_STATUS s36;
    NC_FEATURE_BACKLOG_V37_STATUS s37;
    NC_FEATURE_BACKLOG_V38_STATUS s38;
    NC_FEATURE_BACKLOG_V39_STATUS s39;
    NC_FEATURE_BACKLOG_V40_STATUS s40;
    NC_FEATURE_BACKLOG_V41_STATUS s41;
    test_begin("cross implemented counts match constants after init");
    NcFeatureBacklogV35_Init(); Api_GetNcFeatureBacklogV35Status(&s35);
    NcFeatureBacklogV36_Init(); Api_GetNcFeatureBacklogV36Status(&s36);
    NcFeatureBacklogV37_Init(); Api_GetNcFeatureBacklogV37Status(&s37);
    NcFeatureBacklogV38_Init(); Api_GetNcFeatureBacklogV38Status(&s38);
    NcFeatureBacklogV39_Init(); Api_GetNcFeatureBacklogV39Status(&s39);
    NcFeatureBacklogV40_Init(); Api_GetNcFeatureBacklogV40Status(&s40);
    NcFeatureBacklogV41_Init(); Api_GetNcFeatureBacklogV41Status(&s41);
    EXPECT_EQ_U32(s35.implemented_count, NC_FEATURE_BACKLOG_V35_FEATURE_COUNT);
    EXPECT_EQ_U32(s36.implemented_count, NC_FEATURE_BACKLOG_V36_FEATURE_COUNT);
    EXPECT_EQ_U32(s37.implemented_count, NC_FEATURE_BACKLOG_V37_FEATURE_COUNT);
    EXPECT_EQ_U32(s38.implemented_count, NC_FEATURE_BACKLOG_V38_FEATURE_COUNT);
    EXPECT_EQ_U32(s39.implemented_count, NC_FEATURE_BACKLOG_V39_FEATURE_COUNT);
    EXPECT_EQ_U32(s40.implemented_count, NC_FEATURE_BACKLOG_V40_FEATURE_COUNT);
    EXPECT_EQ_U32(s41.implemented_count, NC_FEATURE_BACKLOG_V41_FEATURE_COUNT);
    EXPECT_EQ_U32(s36.implemented_count, 5u);
    EXPECT_EQ_U32(s35.implemented_count + s36.implemented_count + s37.implemented_count + s38.implemented_count + s39.implemented_count + s40.implemented_count + s41.implemented_count, 65u);
}

int main(void)
{
    test_v35_v47_01_sparse_enable_ts_hit_isolation();
    test_v35_v47_02_last_feature_only_updates_last_feature_id();
    test_v35_v47_03_feed_override_bounds_after_ts_adjustment();
    test_v35_v47_04_range_errors_do_not_mutate_status();
    test_v35_v47_05_all_valid_feature_names_are_resolved();

    test_v36_v47_01_sparse_enable_ts_hit_isolation();
    test_v36_v47_02_last_feature_only_updates_last_feature_id();
    test_v36_v47_03_feed_override_bounds_after_ts_adjustment();
    test_v36_v47_04_range_errors_do_not_mutate_status();
    test_v36_v47_05_all_valid_feature_names_are_resolved();

    test_v37_v47_01_sparse_enable_ts_hit_isolation();
    test_v37_v47_02_last_feature_only_updates_last_feature_id();
    test_v37_v47_03_feed_override_bounds_after_ts_adjustment();
    test_v37_v47_04_range_errors_do_not_mutate_status();
    test_v37_v47_05_all_valid_feature_names_are_resolved();

    test_v38_v47_01_sparse_enable_ts_hit_isolation();
    test_v38_v47_02_last_feature_only_updates_last_feature_id();
    test_v38_v47_03_feed_override_bounds_after_ts_adjustment();
    test_v38_v47_04_range_errors_do_not_mutate_status();
    test_v38_v47_05_all_valid_feature_names_are_resolved();

    test_v39_v47_01_sparse_enable_ts_hit_isolation();
    test_v39_v47_02_last_feature_only_updates_last_feature_id();
    test_v39_v47_03_feed_override_bounds_after_ts_adjustment();
    test_v39_v47_04_range_errors_do_not_mutate_status();
    test_v39_v47_05_all_valid_feature_names_are_resolved();

    test_v40_v47_01_sparse_enable_ts_hit_isolation();
    test_v40_v47_02_last_feature_only_updates_last_feature_id();
    test_v40_v47_03_feed_override_bounds_after_ts_adjustment();
    test_v40_v47_04_range_errors_do_not_mutate_status();
    test_v40_v47_05_all_valid_feature_names_are_resolved();

    test_v41_v47_01_sparse_enable_ts_hit_isolation();
    test_v41_v47_02_last_feature_only_updates_last_feature_id();
    test_v41_v47_03_feed_override_bounds_after_ts_adjustment();
    test_v41_v47_04_range_errors_do_not_mutate_status();
    test_v41_v47_05_all_valid_feature_names_are_resolved();

    cross_v47_36_self_check_contracts();
    cross_v47_37_zero_enabled_ts_has_no_feature_hits();
    cross_v47_38_first_last_enable_contracts();
    cross_v47_39_trace_base_names_and_invalid_state();
    cross_v47_40_implemented_counts_match_constants_after_init();

    printf("UNIT_TEST_SUMMARY tests=%u failures=%u\n", g_tests_run, g_tests_failed);
    if (g_tests_run != 40u) {
        printf("FAILED expected exactly 40 test cases\n");
        return 1;
    }
    if (g_tests_failed != 0u) {
        printf("FAILED failures=%u\n", g_tests_failed);
        return 1;
    }
    printf("PASS additional_unit_tests_v47_40\n");
    return 0;
}
