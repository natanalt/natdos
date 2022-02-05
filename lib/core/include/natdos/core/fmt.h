#ifndef CORE_FMT_H
#define CORE_FMT_H

#include "core.h"

typedef void (*FMTCALLBACK)(LPVOID UserData, SIZE Index, CHAR Next);

SIZE
FormatWithCallback(
    LPCSTR Format,
    FMTCALLBACK Callback,
    LPVOID UserData,
    va_list Variadic);

SIZE
FormatString(
    LPCSTR Format,
    PSTR Buffer,
    SIZE BufferSize,
    ...);

#endif
