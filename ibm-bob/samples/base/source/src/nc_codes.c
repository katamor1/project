/* ibm-bob/samples/base/source/src/nc_codes.c */
/* Provides compact G-code, address, and M-code lookup tables for NC parsing. */
/* This exists so code interpretation has a clear foundation before full functions. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_codes.h, ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/inc/nc_program.h */
#include <ctype.h>
#include <stddef.h>
#include "nc_codes.h"
#include "nc_program.h"

#define G(whole)       NC_G_CODE_WHOLE(whole)
#define GD(whole, dec) NC_G_CODE10((whole), (dec))

static const NC_G_CODE_INFO s_gCodes[] = {
    {G(0), 1U, NC_MOTION_RAPID,  NC_MODAL_FLAG_G00},
    {G(1), 1U, NC_MOTION_LINEAR, NC_MODAL_FLAG_G01},
    {G(2), 1U, NC_MOTION_ARC_CW, NC_MODAL_FLAG_G02},
    {G(3), 1U, NC_MOTION_ARC_CCW, NC_MODAL_FLAG_G03},
    {GD(2, 2), 1U, NC_MOTION_ADVANCED_INTERP, 0U},
    {GD(3, 2), 1U, NC_MOTION_ADVANCED_INTERP, 0U},
    {GD(2, 3), 1U, NC_MOTION_ADVANCED_INTERP, 0U},
    {GD(3, 3), 1U, NC_MOTION_ADVANCED_INTERP, 0U},
    {G(4), 1U, NC_MOTION_DWELL,  NC_MODAL_FLAG_G04},
    {G(5), 1U, NC_MOTION_NONE,   NC_MODAL_FLAG_G05},
    {GD(5, 1), 1U, NC_MOTION_NONE, 0U},
    {GD(6, 2), 1U, NC_MOTION_ADVANCED_INTERP, 0U},
    {GD(7, 1), 1U, NC_MOTION_ADVANCED_INTERP, 0U},
    {GD(5, 2), 1U, NC_MOTION_NONE, 0U},
    {G(8), 1U, NC_MOTION_NONE,   NC_MODAL_FLAG_G08},
    {G(9), 1U, NC_MOTION_NONE,   NC_MODAL_FLAG_G09},
    {G(10), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G10},
    {GD(10, 6), 1U, NC_MOTION_REFERENCE_RETURN, 0U},
    {GD(11, 6), 1U, NC_MOTION_REFERENCE_RETURN, 0U},
    {GD(12, 1), 1U, NC_MOTION_ADVANCED_INTERP, 0U},
    {GD(13, 1), 1U, NC_MOTION_ADVANCED_INTERP, 0U},
    {GD(12, 4), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {GD(13, 4), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(15), 1U, NC_MOTION_NONE,  0U},
    {G(16), 1U, NC_MOTION_NONE,  0U},
    {G(17), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G17},
    {G(18), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G18},
    {G(19), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G19},
    {G(20), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G20},
    {G(21), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G21},
    {G(28), 1U, NC_MOTION_REFERENCE_RETURN, 0U},
    {G(30), 1U, NC_MOTION_REFERENCE_RETURN, 0U},
    {G(31), 1U, NC_MOTION_SKIP,  0U},
    {G(32), 1U, NC_MOTION_THREAD, 0U},
    {G(40), 1U, NC_MOTION_NONE,  0U},
    {G(41), 1U, NC_MOTION_NONE,  0U},
    {G(42), 1U, NC_MOTION_NONE,  0U},
    {G(43), 1U, NC_MOTION_NONE,  0U},
    {G(44), 1U, NC_MOTION_NONE,  0U},
    {G(49), 1U, NC_MOTION_NONE,  0U},
    {GD(50, 1), 1U, NC_MOTION_NONE, 0U},
    {GD(50, 4), 1U, NC_MOTION_NONE, 0U},
    {GD(50, 5), 1U, NC_MOTION_NONE, 0U},
    {GD(50, 6), 1U, NC_MOTION_NONE, 0U},
    {G(52), 1U, NC_MOTION_NONE,  0U},
    {G(53), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G53},
    {GD(53, 1), 1U, NC_MOTION_NONE, 0U},
    {G(54), 1U, NC_MOTION_NONE,  0U},
    {G(55), 1U, NC_MOTION_NONE,  0U},
    {G(56), 1U, NC_MOTION_NONE,  0U},
    {G(57), 1U, NC_MOTION_NONE,  0U},
    {G(58), 1U, NC_MOTION_NONE,  0U},
    {G(59), 1U, NC_MOTION_NONE,  0U},
    {G(60), 1U, NC_MOTION_LINEAR, 0U},
    {GD(51, 1), 1U, NC_MOTION_NONE, 0U},
    {GD(51, 4), 1U, NC_MOTION_NONE, 0U},
    {GD(51, 5), 1U, NC_MOTION_NONE, 0U},
    {GD(51, 6), 1U, NC_MOTION_NONE, 0U},
    {G(61), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G61},
    {G(62), 1U, NC_MOTION_NONE,  0U},
    {G(63), 1U, NC_MOTION_NONE,  0U},
    {G(64), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G64},
    {G(68), 1U, NC_MOTION_NONE,  0U},
    {GD(68, 2), 1U, NC_MOTION_NONE, 0U},
    {G(69), 1U, NC_MOTION_NONE,  0U},
    {GD(69, 1), 1U, NC_MOTION_NONE, 0U},
    {G(70), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {GD(70, 7), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(71), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {GD(71, 7), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(72), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {GD(72, 7), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(73), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {GD(73, 7), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(74), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {GD(74, 7), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(75), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {GD(75, 7), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(76), 1U, NC_MOTION_THREAD, 0U},
    {GD(76, 7), 1U, NC_MOTION_THREAD, 0U},
    {G(77), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(78), 1U, NC_MOTION_THREAD, 0U},
    {G(79), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(80), 1U, NC_MOTION_NONE,  0U},
    {G(81), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(82), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(83), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(84), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(85), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(86), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(87), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(88), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(89), 1U, NC_MOTION_CANNED_CYCLE, 0U},
    {G(90), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G90},
    {G(91), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G91},
    {G(92), 1U, NC_MOTION_THREAD, NC_MODAL_FLAG_G92},
    {G(94), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G94},
    {G(95), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G95},
    {G(96), 1U, NC_MOTION_NONE,  0U},
    {G(97), 1U, NC_MOTION_NONE,  0U},
    {G(98), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G98},
    {G(99), 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G99}
};

static const NC_ADDRESS_INFO s_addresses[] = {
    {'A', 1U, NC_ADDRESS_KIND_AXIS,    3},
    {'B', 1U, NC_ADDRESS_KIND_OFFSET, -1},
    {'C', 1U, NC_ADDRESS_KIND_OFFSET, -1},
    {'D', 1U, NC_ADDRESS_KIND_OFFSET, -1},
    {'E', 1U, NC_ADDRESS_KIND_OFFSET, -1},
    {'F', 1U, NC_ADDRESS_KIND_FEED,   -1},
    {'G', 1U, NC_ADDRESS_KIND_G_CODE, -1},
    {'H', 1U, NC_ADDRESS_KIND_OFFSET, -1},
    {'I', 1U, NC_ADDRESS_KIND_ARC_OFFSET, 0},
    {'J', 1U, NC_ADDRESS_KIND_ARC_OFFSET, 1},
    {'K', 1U, NC_ADDRESS_KIND_ARC_OFFSET, 2},
    {'L', 1U, NC_ADDRESS_KIND_L_WORD, -1},
    {'M', 1U, NC_ADDRESS_KIND_M_CODE, -1},
    {'N', 1U, NC_ADDRESS_KIND_NUMBER, -1},
    {'O', 1U, NC_ADDRESS_KIND_NUMBER, -1},
    {'P', 1U, NC_ADDRESS_KIND_P_WORD, -1},
    {'Q', 1U, NC_ADDRESS_KIND_Q_WORD, -1},
    {'R', 1U, NC_ADDRESS_KIND_ARC_RADIUS, -1},
    {'S', 1U, NC_ADDRESS_KIND_SPINDLE, -1},
    {'T', 1U, NC_ADDRESS_KIND_TOOL,   -1},
    {'U', 1U, NC_ADDRESS_KIND_INCREMENTAL_AXIS, 0},
    {'V', 1U, NC_ADDRESS_KIND_INCREMENTAL_AXIS, 1},
    {'W', 1U, NC_ADDRESS_KIND_INCREMENTAL_AXIS, 2},
    {'X', 1U, NC_ADDRESS_KIND_AXIS,    0},
    {'Y', 1U, NC_ADDRESS_KIND_AXIS,    1},
    {'Z', 1U, NC_ADDRESS_KIND_AXIS,    2}
};

static const NC_M_CODE_INFO s_mCodes[] = {
    {0U, 1U, 0U},
    {1U, 1U, 0U},
    {2U, 1U, 0U},
    {3U, 1U, 0U},
    {4U, 1U, 0U},
    {5U, 1U, 0U},
    {6U, 1U, 1U},
    {7U, 1U, 0U},
    {8U, 1U, 0U},
    {9U, 1U, 0U},
    {30U, 1U, 0U},
    {98U, 1U, 0U},
    {99U, 1U, 0U},
    {130U, 1U, 0U}
};

/**
 * @brief Copy G code info from NC codes.
 * @param code Code value being applied or recorded.
 * @param pOutInfo Output pointer receiving info.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCodes_GetGCodeInfo(int32_t code, NC_G_CODE_INFO* pOutInfo)
{
    size_t i;

    if (pOutInfo == NULL) {
        return -1;
    }
    for (i = 0U; i < (sizeof(s_gCodes) / sizeof(s_gCodes[0])); ++i) {
        if (s_gCodes[i].code == code) {
            *pOutInfo = s_gCodes[i];
            return 0;
        }
    }
    return -1;
}

/**
 * @brief Copy address info from NC codes.
 * @param address Input value for address.
 * @param pOutInfo Output pointer receiving info.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCodes_GetAddressInfo(char address, NC_ADDRESS_INFO* pOutInfo)
{
    size_t i;
    char normalized = (char)toupper((unsigned char)address);

    if (pOutInfo == NULL) {
        return -1;
    }
    for (i = 0U; i < (sizeof(s_addresses) / sizeof(s_addresses[0])); ++i) {
        if (s_addresses[i].address == normalized) {
            *pOutInfo = s_addresses[i];
            return 0;
        }
    }
    return -1;
}

/**
 * @brief Copy M code info from NC codes.
 * @param code Code value being applied or recorded.
 * @param pOutInfo Output pointer receiving info.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCodes_GetMCodeInfo(uint32_t code, NC_M_CODE_INFO* pOutInfo)
{
    size_t i;

    if (pOutInfo == NULL) {
        return -1;
    }
    for (i = 0U; i < (sizeof(s_mCodes) / sizeof(s_mCodes[0])); ++i) {
        if (s_mCodes[i].code == code) {
            *pOutInfo = s_mCodes[i];
            return 0;
        }
    }

    pOutInfo->code = code;
    pOutInfo->supported_v1 = (uint8_t)(code <= 999U);
    pOutInfo->waits_for_mfin = 0U;
    return 0;
}
