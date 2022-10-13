#ifndef CORE_H
#define CORE_H

#include "compiler/compiler.h"
#include "memory.h"
#include "fmt.h"

#define UNREFERENCED_PARAMETER(x) ((void) x)

/// Emits given buffer and its size as 2 function parameters 
///
/// Used for functions like FormatString:
///     CHAR Buffer[128];
///     FormatString("Hello", PBUFFER(Buffer));
///     // Emitted as:
///     FormatString("Hello", (Buffer), sizeof(Buffer))
#define PBUFFER(buf) (buf), sizeof(buf)

// TODO: Panic impl
#define CRASH(...) while(1);
#define TODO(msg) CRASH("Todo: " msg)
#define UNIMPLEMENTED(msg) CRASH("Unimplemented: " msg)
#define UNREACHABLE(msg) CRASH("Unreachable: " msg)

#define MAKE_WORD(Lo, Hi) (AS_WORD(Hi) << 8 | (Lo))

#endif
