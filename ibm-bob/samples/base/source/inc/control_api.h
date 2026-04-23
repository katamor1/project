#ifndef CONTROL_API_H
#define CONTROL_API_H

#include <stdint.h>
#include "system_shared.h"

int32_t Api_RequestRunModeChange(uint8_t requestedMode);
int32_t Api_GetStatusSnapshot(STATUS_SNAPSHOT* pOutSnapshot);
int32_t Api_RequestLogFlush(void);
int32_t Api_GetPrefetchStatus(uint8_t* pReady, uint16_t* pGeneration);

#endif /* CONTROL_API_H */
