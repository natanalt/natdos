#ifndef CORE_H
#define CORE_H

#include "compiler/compiler.h"
#include "memory.h"

// TODO: Panic impl
#define Panic(...) while(1);
#define TODO(msg) Panic("Todo: " msg)
#define UNIMPLEMENTED(msg) Panic("Unimplemented: " msg)
#define UNREACHABLE(msg) Panic("Unreachable: " msg)

#define MAKE_WORD(Lo, Hi) (AS_WORD(Hi) << 8 | (Lo))

#endif
