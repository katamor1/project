#include "test_common.h"
#include "system_shared.h"
#include "nc_program.h"
#include "nc_capability.h"

static void setup(void)
{
    SystemShared_Initialize();
    NcCapability_Reset();
}

static int test_g_history_wrap_keeps_latest_entry_and_index(void)
{
    uint32_t i;
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    b.valid = 1U;

    for (i = 0U; i < (NC_CAPABILITY_G_HISTORY_SIZE + 3U); ++i) {
        b.g_code10 = (uint16_t)(NC_G_CODE_WHOLE(1) + i);
        NcCapability_OnParsedBlockTs(&b);
    }

    TEST_ASSERT_EQ_UINT(NC_CAPABILITY_G_HISTORY_SIZE + 3U, g_ncCapabilityStatus.g_history_index);
    TEST_ASSERT_EQ_UINT((uint32_t)(NC_G_CODE_WHOLE(1) + NC_CAPABILITY_G_HISTORY_SIZE + 2U),
                        g_ncCapabilityStatus.last_g_code10);
    TEST_ASSERT_EQ_UINT(g_ncCapabilityStatus.last_g_code10,
                        g_ncCapabilityStatus.g_history[(NC_CAPABILITY_G_HISTORY_SIZE + 2U) % NC_CAPABILITY_G_HISTORY_SIZE]);
    TEST_ASSERT_EQ_UINT(NC_CAPABILITY_G_HISTORY_SIZE + 3U, g_ncCapabilityStatus.supported_g_blocks);
    return 0;
}

static int test_m_history_wrap_and_unknown_kind_event_side_effect(void)
{
    uint32_t i;
    NC_EXEC_BLOCK b;
    setup();
    memset(&b, 0, sizeof(b));
    b.valid = 1U;
    b.aux_flags = NC_AUX_FLAG_M_CODE;

    for (i = 0U; i < (NC_CAPABILITY_M_HISTORY_SIZE + 2U); ++i) {
        b.aux_m_code = 10U + i;
        NcCapability_OnParsedBlockTs(&b);
    }

    TEST_ASSERT_EQ_UINT(NC_CAPABILITY_M_HISTORY_SIZE + 2U, g_ncCapabilityStatus.m_history_index);
    TEST_ASSERT_EQ_UINT(10U + NC_CAPABILITY_M_HISTORY_SIZE + 1U, g_ncCapabilityStatus.last_m_code);
    TEST_ASSERT_EQ_UINT(g_ncCapabilityStatus.last_m_code,
                        g_ncCapabilityStatus.m_history[(NC_CAPABILITY_M_HISTORY_SIZE + 1U) % NC_CAPABILITY_M_HISTORY_SIZE]);

    NcCapability_RecordUnsupported(99U, 12345U);
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.unsupported_g_events);
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.unsupported_m_events);
    TEST_ASSERT_EQ_UINT(0U, g_ncCapabilityStatus.unsupported_address_events);
    TEST_ASSERT_EQ_UINT(1U, g_ncEventRing.captured_count);
    TEST_ASSERT_EQ_UINT(NC_EVENT_CODE_CAPABILITY_UNSUPPORTED, g_ncEventRing.items[0].code);
    return 0;
}

int main(void)
{
    RUN_TEST(test_g_history_wrap_keeps_latest_entry_and_index);
    RUN_TEST(test_m_history_wrap_and_unknown_kind_event_side_effect);
    printf("test_nc_capability_history_wrap: PASS\n");
    return 0;
}
