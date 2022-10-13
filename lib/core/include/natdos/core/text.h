#ifndef CORE_TEXT_H
#define CORE_TEXT_H

#include "core.h"

STATIC INLINE CHAR
AsciiToUpper(CHAR c)
{
    if (c >= 'a' && c <= 'z')
        return c - 32;
    else
        return c;
}

STATIC INLINE CHAR
AsciiToLower(CHAR c)
{
    if (c >= 'A' && c <= 'Z')
        return c + 32;
    else
        return c;
}

#endif
