#include <natdos/core/core.h>
#include <natdos/core/libc.h>

void*
memset(void* Destination, int Value, size_t Size)
{
    SetMemoryBytes(Destination, Value, Size);
    return Destination;
}

void*
memcpy(void* Destination, const void* Source, size_t Size)
{
    CopyMemory(Destination, Source, Size);
    return Destination;
}
