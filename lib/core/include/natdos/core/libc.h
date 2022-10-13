#ifndef CORE_LIBC_H
#define CORE_LIBC_H

#include "core.h"

typedef SIZE size_t;

void*
memset(void* Destination, int Value, size_t Size);

void*
memcpy(void* Destination, const void* Source, size_t Size);

#endif
