#include "memory.h"

extern BYTE FreeStart;

STATIC PBYTE Heap;
STATIC SIZE UsedHeap;
STATIC SIZE TotalHeap;

VOID
InitAllocator(VOID)
{
    Heap = AS(PBYTE, &FreeStart);
    UsedHeap = 0;
    TotalHeap = 24 * 1024;
}

PVOID
AllocateMemory(SIZE Bytes)
{
    PVOID Result = Heap;
    UsedHeap += Bytes;
    return Result;
}

VOID*
FreeMemory(VOID* Base)
{
}
