#ifndef CORE_COMPILER_H
#define CORE_COMPILER_H

#ifndef COMPILER_TYPE
    #error "COMPILER_TYPE not set"
#endif

#define COMPILER_GCC 1

#if COMPILER_TYPE == COMPILER_GCC
    #include "gcc.h"
#else
    #error "Unknown compiler"
#endif

#endif
