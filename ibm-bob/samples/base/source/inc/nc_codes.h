/* ibm-bob/samples/base/source/inc/nc_codes.h */
/* Declares small G-code, address, and M-code lookup helpers for NC parsing. */
/* This exists so parser behavior is table-driven before individual code details grow. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_codes.c, ibm-bob/samples/base/source/src/nc_parser.c, ibm-bob/samples/base/source/inc/nc_program.h */
#ifndef NC_CODES_H
#define NC_CODES_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Enumeration for NC address kind.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef enum {
    NC_ADDRESS_KIND_UNKNOWN = 0, /**< Definition value for NC address kind unknown. */
    NC_ADDRESS_KIND_AXIS = 1, /**< Definition value for NC address kind axis. */
    NC_ADDRESS_KIND_FEED = 2, /**< Definition value for NC address kind feed. */
    NC_ADDRESS_KIND_SPINDLE = 3, /**< Definition value for NC address kind spindle. */
    NC_ADDRESS_KIND_TOOL = 4, /**< Definition value for NC address kind tool. */
    NC_ADDRESS_KIND_M_CODE = 5, /**< Definition value for NC address kind M code. */
    NC_ADDRESS_KIND_G_CODE = 6, /**< Definition value for NC address kind G code. */
    NC_ADDRESS_KIND_ARC_OFFSET = 7, /**< Definition value for NC address kind arc offset. */
    NC_ADDRESS_KIND_ARC_RADIUS = 8, /**< Definition value for NC address kind arc radius. */
    NC_ADDRESS_KIND_DWELL = 9, /**< Definition value for NC address kind dwell. */
    NC_ADDRESS_KIND_L_WORD = 10, /**< Definition value for NC address kind l word. */
    NC_ADDRESS_KIND_P_WORD = 11, /**< Definition value for NC address kind p word. */
    NC_ADDRESS_KIND_Q_WORD = 12, /**< Definition value for NC address kind q word. */
    NC_ADDRESS_KIND_NUMBER = 13, /**< Definition value for NC address kind number. */
    NC_ADDRESS_KIND_OFFSET = 14, /**< Definition value for NC address kind offset. */
    NC_ADDRESS_KIND_INCREMENTAL_AXIS = 15 /**< Definition value for NC address kind incremental axis. */
} NC_ADDRESS_KIND;

/**
 * @brief Structure for NC G code info.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    int32_t        code;
    uint8_t        supported_v1;
    NC_MOTION_TYPE motion_type;
    uint32_t       modal_flag;
} NC_G_CODE_INFO;

/**
 * @brief Structure for NC address info.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    char            address;
    uint8_t         supported_v1;
    NC_ADDRESS_KIND kind;
    int32_t         axis_index;
} NC_ADDRESS_INFO;

/**
 * @brief Structure for NC M code info.
 * @details Public shared type used by the IBM BOB baseline sample. It is intentionally fixed-size so RT, TS, and API modules can exchange state without dynamic allocation.
 */
typedef struct {
    uint32_t code;
    uint8_t  supported_v1;
    uint8_t  waits_for_mfin;
} NC_M_CODE_INFO;

/**
 * @brief Copy G code info from NC codes.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param code Code value being applied or recorded.
 * @param pOutInfo Output pointer receiving info.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCodes_GetGCodeInfo(int32_t code, NC_G_CODE_INFO* pOutInfo);
/**
 * @brief Copy address info from NC codes.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param address Input value for address.
 * @param pOutInfo Output pointer receiving info.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCodes_GetAddressInfo(char address, NC_ADDRESS_INFO* pOutInfo);
/**
 * @brief Copy M code info from NC codes.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param code Code value being applied or recorded.
 * @param pOutInfo Output pointer receiving info.
 * @return 0 on success; a negative value or module-specific code on failure.
 */
int32_t NcCodes_GetMCodeInfo(uint32_t code, NC_M_CODE_INFO* pOutInfo);

#endif /* NC_CODES_H */
