#include <natdos/core/core.h>

#ifndef BACKEND_IMPLEMENTS_MEMORY_FUNCTIONS
    #error Memory functions are unimplemented without compiler support!
#endif

SIZE
GetStringSize(PCSTR String)
{
    SIZE Result = 0;
    while (*(String++)) Result++;
    return Result;
}
