/* ibm-bob/samples/base/source/inc/nc_capability.h */
/* Tracks table-driven G/M/address coverage and parser usage diagnostics. */
/* This exists so feature coverage can be reported without coupling parser code to UI snapshots. */
/* RELEVANT FILES: ibm-bob/samples/base/source/src/nc_capability.c, ibm-bob/samples/base/source/inc/system_shared.h, ibm-bob/samples/base/source/src/nc_parser_tokens.c */
#ifndef NC_CAPABILITY_H
#define NC_CAPABILITY_H

#include <stdint.h>
#include "system_shared.h"

/**
 * @brief Definition value for NC capability kind G.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CAPABILITY_KIND_G       (1U)
/**
 * @brief Definition value for NC capability kind M.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CAPABILITY_KIND_M       (2U)
/**
 * @brief Definition value for NC capability kind address.
 * @details This build-time constant is shared by RT, TS, parser, and API code through the public header contract.
 */
#define NC_CAPABILITY_KIND_ADDRESS (3U)

/**
 * @brief Reset NC capability state to its default values.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 */
void NcCapability_Reset(void);

/**
 * @brief Record NC capability data from one parsed TS block.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param pBlock NC execution block to read or update.
 */
void NcCapability_OnParsedBlockTs(const NC_EXEC_BLOCK* pBlock);

/**
 * @brief Record unsupported for diagnostics.
 * @details This symbol is part of the sample workspace public surface. Callers use it through the shared header contract; implementation code must keep RT-safe behavior deterministic.
 * @param kind Input value for kind.
 * @param code Code value being applied or recorded.
 */
void NcCapability_RecordUnsupported(uint8_t kind, uint32_t code);

#endif /* NC_CAPABILITY_H */
