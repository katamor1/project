/* ibm-bob/samples/base/source/inc/nc_interference.h */
/* Declares lightweight path/3D interference screening for TS-side block generation. */
#ifndef NC_INTERFERENCE_H
#define NC_INTERFERENCE_H

#include <stdint.h>
#include "system_shared.h"

void NcInterference_Reset(void);
int32_t NcInterference_SetEnabled(uint8_t enabled);
void NcInterference_CheckBlockTs(NC_EXEC_BLOCK* pBlock);

#endif /* NC_INTERFERENCE_H */
