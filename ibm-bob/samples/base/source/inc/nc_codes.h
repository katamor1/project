/* ibm-bob/samples/base/source/inc/nc_codes.h */
/* Declares small G-code, address, and M-code lookup helpers for NC parsing. */
/* This exists so parser behavior is table-driven before individual code details grow. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_codes.c, ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/inc/nc_program.h */
#ifndef NC_CODES_H
#define NC_CODES_H

#include <stdint.h>
#include "system_shared.h"

typedef enum {
    NC_ADDRESS_KIND_UNKNOWN = 0,
    NC_ADDRESS_KIND_AXIS    = 1,
    NC_ADDRESS_KIND_FEED    = 2,
    NC_ADDRESS_KIND_SPINDLE = 3,
    NC_ADDRESS_KIND_TOOL    = 4,
    NC_ADDRESS_KIND_M_CODE  = 5,
    NC_ADDRESS_KIND_G_CODE  = 6,
    NC_ADDRESS_KIND_ARC_OFFSET = 7,
    NC_ADDRESS_KIND_ARC_RADIUS = 8,
    NC_ADDRESS_KIND_DWELL      = 9,
    NC_ADDRESS_KIND_L_WORD     = 10,
    NC_ADDRESS_KIND_P_WORD     = 11,
    NC_ADDRESS_KIND_Q_WORD     = 12,
    NC_ADDRESS_KIND_NUMBER     = 13,
    NC_ADDRESS_KIND_OFFSET     = 14,
    NC_ADDRESS_KIND_INCREMENTAL_AXIS = 15
} NC_ADDRESS_KIND;

typedef struct {
    int32_t        code;
    uint8_t        supported_v1;
    NC_MOTION_TYPE motion_type;
    uint32_t       modal_flag;
} NC_G_CODE_INFO;

typedef struct {
    char            address;
    uint8_t         supported_v1;
    NC_ADDRESS_KIND kind;
    int32_t         axis_index;
} NC_ADDRESS_INFO;

typedef struct {
    uint32_t code;
    uint8_t  supported_v1;
    uint8_t  waits_for_mfin;
} NC_M_CODE_INFO;

int32_t NcCodes_GetGCodeInfo(int32_t code, NC_G_CODE_INFO* pOutInfo);
int32_t NcCodes_GetAddressInfo(char address, NC_ADDRESS_INFO* pOutInfo);
int32_t NcCodes_GetMCodeInfo(uint32_t code, NC_M_CODE_INFO* pOutInfo);

#endif /* NC_CODES_H */
