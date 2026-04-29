/* ibm-bob/samples/base/source/src/nc_parser.c */
/* Scans NC text lines and delegates token handling to parser modal helpers. */
/* This exists so the scanner stays small and RT never parses text. */
/* RELEVANT FILES: ibm-bob/samples/base/source/inc/nc_parser_internal.h, ibm-bob/samples/base/source/src/nc_parser_modal.c, ibm-bob/samples/base/source/inc/nc_program.h */
#include <ctype.h>
#include <stdlib.h>
#include "nc_parser_internal.h"
#include "nc_program.h"

void NcParse_ResetModal(void)
{
    NcParser_ResetModalState();
}

static int32_t ReadNumber(const char** ppCursor, double* pOutValue)
{
    char* pEnd;
    const char* pStart = *ppCursor;

    *pOutValue = strtod(pStart, &pEnd);
    if (pEnd == pStart) {
        return -1;
    }
    *ppCursor = pEnd;
    return 0;
}

static int32_t MapApplyResult(int32_t result, NC_ERROR_CODE* pOutError)
{
    if (result == -3) {
        *pOutError = NC_ERR_MODAL_CONFLICT;
        return -1;
    }
    if (result == -2) {
        *pOutError = NC_ERR_RANGE;
        return -1;
    }
    if (result == -4) {
        *pOutError = NC_ERR_COORDINATE;
        return -1;
    }
    if (result == -5) {
        *pOutError = NC_ERR_UNIT;
        return -1;
    }
    if (result != 0) {
        *pOutError = NC_ERR_UNSUPPORTED;
        return -1;
    }
    return 0;
}

int32_t NcParse_LineToBlock(const char* line,
                            uint32_t lineNo,
                            NC_EXEC_BLOCK* pOutBlock,
                            NC_ERROR_CODE* pOutError)
{
    const char* pCursor = line;
    NC_PARSE_CONTEXT ctx = {0};
    uint8_t hasToken = 0U;
    double value;

    NcParser_InitBlock(pOutBlock, lineNo);
    ctx.coord_mode = NC_PARSE_COORD_NONE;
    *pOutError = NC_ERR_NONE;

    while (*pCursor != '\0') {
        int32_t result;
        char address;

        while (isspace((unsigned char)*pCursor) != 0) {
            pCursor++;
        }
        if (*pCursor == '\0') {
            break;
        }
        if (isalpha((unsigned char)*pCursor) == 0) {
            *pOutError = NC_ERR_TOKEN;
            return -1;
        }
        address = (char)toupper((unsigned char)*pCursor);
        pCursor++;
        if (ReadNumber(&pCursor, &value) != 0) {
            *pOutError = NC_ERR_TOKEN;
            return -1;
        }
        result = NcParser_ApplyToken(address, value, pOutBlock, &ctx);
        if (MapApplyResult(result, pOutError) != 0) {
            return -1;
        }
        hasToken = 1U;
    }

    if (hasToken == 0U) {
        return 1;
    }
    return NcParser_FinalizeBlock(pOutBlock, pOutError);
}
