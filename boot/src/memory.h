#ifndef BOOT_MEMORY_H
#define BOOT_MEMORY_H

#include <natdos/core/core.h>

VOID
InitAllocator(VOID);

PVOID
AllocateMemory(SIZE Bytes);

VOID
FreeMemory(PVOID Base);

#endif
