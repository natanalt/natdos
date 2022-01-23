#include "memory.h"

extern BYTE FreeStart;

STATIC BYTE* Heap;
STATIC SIZE UsedHeap;
STATIC SIZE TotalHeap;

VOID
InitAllocator(VOID)
{
    Heap = AS(BYTE*, &FreeStart);
    UsedHeap = 0;
    TotalHeap = 24 * 1024;
}

VOID*
AllocateMemory(SIZE Bytes)
{
    VOID* Result = Heap;
    UsedHeap += Bytes;
    return Result;
}

VOID*
FreeMemory(VOID* Base)
{
}
