#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include "core.h"

#ifndef BACKEND_IMPLEMENTS_MEMORY_FUNCTIONS

VOID
CopyMemory(
    PVOID Destination,
    PVOID Source,
    SIZE Size);

VOID
FarCopyMemory(
    LPVOID Destination,
    LPVOID Source,
    SIZE Size);

VOID
SetMemoryBytes(
    PBYTE Destination,
    BYTE Value,
    SIZE Size);

VOID
SetMemoryWords(
    PWORD Destination,
    WORD Value,
    SIZE Size);

VOID
SetFarMemoryBytes(
    LPBYTE Destination,
    BYTE Value,
    SIZE Size);

VOID
SetFarMemoryWords(
    LPWORD Destination,
    WORD Value,
    SIZE Size);

BOOL
CompareMemoryBytes(
    PCVOID BlockA,
    PCVOID BlockB,
    SIZE Size);

BOOL
CompareFarMemoryBytes(
    LPCVOID BlockA,
    LPCVOID BlockB,
    SIZE Size);

#endif

SIZE
GetStringSize(PCSTR String);

#endif
