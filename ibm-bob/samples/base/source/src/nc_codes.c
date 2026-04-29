/* ibm-bob/samples/base/source/src/nc_codes.c */
/* Provides compact G-code, address, and M-code lookup tables for NC parsing. */
/* This exists so code interpretation has a clear foundation before full functions. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_codes.h, ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/inc/nc_program.h */
#include <ctype.h>
#include <stddef.h>
#include "nc_codes.h"
#include "nc_program.h"

static const NC_G_CODE_INFO s_gCodes[] = {
    {0, 1U, NC_MOTION_RAPID,  NC_MODAL_FLAG_G00},
    {1, 1U, NC_MOTION_LINEAR, NC_MODAL_FLAG_G01},
    {2, 1U, NC_MOTION_ARC_CW, NC_MODAL_FLAG_G02},
    {3, 1U, NC_MOTION_ARC_CCW, NC_MODAL_FLAG_G03},
    {4, 1U, NC_MOTION_DWELL,  NC_MODAL_FLAG_G04},
    {9, 1U, NC_MOTION_NONE,   NC_MODAL_FLAG_G09},
    {10, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G10},
    {17, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G17},
    {18, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G18},
    {19, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G19},
    {20, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G20},
    {21, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G21},
    {52, 1U, NC_MOTION_NONE,  0U},
    {53, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G53},
    {54, 1U, NC_MOTION_NONE,  0U},
    {55, 1U, NC_MOTION_NONE,  0U},
    {56, 1U, NC_MOTION_NONE,  0U},
    {57, 1U, NC_MOTION_NONE,  0U},
    {58, 1U, NC_MOTION_NONE,  0U},
    {59, 1U, NC_MOTION_NONE,  0U},
    {61, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G61},
    {63, 0U, NC_MOTION_NONE,  0U},
    {64, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G64},
    {90, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G90},
    {91, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G91},
    {92, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G92},
    {94, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G94},
    {95, 1U, NC_MOTION_NONE,  NC_MODAL_FLAG_G95}
};

static const NC_ADDRESS_INFO s_addresses[] = {
    {'A', 1U, NC_ADDRESS_KIND_AXIS,    3},
    {'B', 0U, NC_ADDRESS_KIND_AXIS,   -1},
    {'C', 0U, NC_ADDRESS_KIND_AXIS,   -1},
    {'F', 1U, NC_ADDRESS_KIND_FEED,   -1},
    {'G', 1U, NC_ADDRESS_KIND_G_CODE, -1},
    {'I', 1U, NC_ADDRESS_KIND_ARC_OFFSET, 0},
    {'J', 1U, NC_ADDRESS_KIND_ARC_OFFSET, 1},
    {'K', 1U, NC_ADDRESS_KIND_ARC_OFFSET, 2},
    {'L', 1U, NC_ADDRESS_KIND_L_WORD, -1},
    {'M', 1U, NC_ADDRESS_KIND_M_CODE, -1},
    {'P', 1U, NC_ADDRESS_KIND_P_WORD, -1},
    {'R', 1U, NC_ADDRESS_KIND_ARC_RADIUS, -1},
    {'S', 1U, NC_ADDRESS_KIND_SPINDLE, -1},
    {'T', 1U, NC_ADDRESS_KIND_TOOL,   -1},
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
    {130U, 1U, 0U}
};

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
