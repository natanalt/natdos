#ifndef BOOT_MEMORY_H
#define BOOT_MEMORY_H

#include <natdos/core/core.h>

VOID
InitAllocator(VOID);

VOID*
AllocateMemory(SIZE Bytes);

VOID*
FreeMemory(VOID* Base);

#endif
