#ifndef CORE_COMPILER_GCC_H
#define CORE_COMPILER_GCC_H

#include <stdint.h>

#ifndef __GNUC__
    #error Attempt to include GCC core header on a non GCC compiler
#endif

#ifndef VSCODE
    #ifndef __FAR
        #error Far pointer support is required!
    #endif
    #define far __far
#else
    #define far
    #define __builtin_ia16_FP_OFF AS_WORD
#endif

#define __NATDOS_TO_STR(x) #x
#define TO_STR(x) __NATDOS_TO_STR(x)

#define COMPILER_VERSION TO_STR(__GNUC__) "." TO_STR(__GNUC_MINOR__) "." TO_STR(__GNUC_PATCHLEVEL__)
#define COMPILER_NAME "GCC " COMPILER_VERSION

#define INLINE inline
#define STATIC static

#define AS(t, ...) ((t) __VA_ARGS__)
#define AS_BYTE(...) AS(BYTE, (__VA_ARGS__))
#define AS_WORD(...) AS(WORD, (__VA_ARGS__))
#define AS_DWORD(...) AS(DWORD, (__VA_ARGS__))
#define AS_SIZE(...) AS(SIZE, (__VA_ARGS__))
#define AS_SBYTE(...) AS(SBYTE, (__VA_ARGS__))
#define AS_SWORD(...) AS(SWORD, (__VA_ARGS__))
#define AS_SDWORD(...) AS(SDWORD, (__VA_ARGS__))
#define AS_LPVOID(...) AS(LPVOID, (__VA_ARGS__))

#define FP_SEGMENT(p) AS_WORD(AS_DWORD(p) >> 16)

// Turns out this version may straight up crash the compiler
//#define FP_OFFSET(p) __builtin_ia16_FP_OFF(p)
#define FP_OFFSET(p) AS_WORD(AS_DWORD(p) & 0xffff)

#define MAKE_FAR(s, o) AS_LPVOID((AS_DWORD(s) << 16) | AS_WORD(o))
#define MAKE_FAR_AS(t, s, o) ((t)MAKE_FAR(s, o))

#define NEAR(name) *P##name
#define FAR(name) far*LP##name

#define TYPEDEFS(type, name) typedef type name, *P##name; typedef type far *LP##name;
#define TYPEDEF_POINTERS(type, name) typedef type *P##name; typedef type far *LP##name;
#define TYPEDEF_STRUCT(name) name; TYPEDEF_POINTERS(name, name);

#define PACKED __attribute__((packed))

TYPEDEFS(void, VOID);
TYPEDEFS(uint8_t, BYTE);
TYPEDEFS(int8_t, SBYTE);
TYPEDEFS(uint16_t, WORD);
TYPEDEFS(int16_t, SWORD);
TYPEDEFS(uint32_t, DWORD);
TYPEDEFS(int32_t, SDWORD);
TYPEDEFS(uint16_t, SIZE);
TYPEDEFS(BYTE, BOOL);
TYPEDEFS(char, CHAR);
TYPEDEF_POINTERS(char, STR);
TYPEDEF_POINTERS(const char, CSTR);

#define TRUE AS(BOOL, 1)
#define FALSE AS(BOOL, 0)

typedef union {
    WORD W;
    struct {
        BYTE L;
        BYTE H;
    } B;
} GCC_WORDREGISTER;

typedef struct {
    GCC_WORDREGISTER A;
    GCC_WORDREGISTER B;
    GCC_WORDREGISTER C;
    GCC_WORDREGISTER D;
    WORD Si;
    WORD Di;
} TYPEDEF_STRUCT(REGISTERS);

typedef struct {
    WORD Es;
} TYPEDEF_STRUCT(SEGMENTREGS);

STATIC INLINE SEGMENTREGS
GetCurrentSegmentRegisters(VOID)
{
    SEGMENTREGS Result;
    __asm__ volatile ("mov %%es, %%ax" : "=a" (Result.Es) :);
    return Result;
}

/// VOID
/// CallInterrupt(
///     BYTE Interrupt,
///     PREGISTERS Registers)
#define CallInterrupt(Interrupt, Registers)  \
    __asm__ volatile (                       \
        "int $" TO_STR(Interrupt)            \
        :                                    \
            "=a" ((Registers)->A.W),         \
            "=b" ((Registers)->B.W),         \
            "=c" ((Registers)->C.W),         \
            "=d" ((Registers)->D.W),         \
            "=S" ((Registers)->Si),          \
            "=D" ((Registers)->Di)           \
        :                                    \
            "a" ((Registers)->A.W),          \
            "b" ((Registers)->B.W),          \
            "c" ((Registers)->C.W),          \
            "d" ((Registers)->D.W),          \
            "S" ((Registers)->Si),           \
            "D" ((Registers)->Di)            \
    );

// Big big warning: This is like, suuuuper unstable and like it like DEFINITELY
// shouldn't be used with Segments being a far pointer. Near pointers only
// pweeeease
// 
/// VOID
/// CallInterruptWithSegments(
///     BYTE Interrupt,
///     PREGISTERS Registers,
///     PSEGMENTREGS Segments);
#define CallInterruptWithSegments(Interrupt, Registers, Segments)  \
    __asm__ volatile ("push %%es" ::);                             \
    __asm__ volatile ("mov %%ax, %%es" :: "a" ((Segments)->Es));   \
    CallInterrupt(Interrupt, Registers);                           \
    __asm__ volatile ("mov %%es, %%ax" : "=a" ((Segments)->Es) :); \
    __asm__ volatile ("pop %%es" ::);

#define BACKEND_IMPLEMENTS_MEMORY_FUNCTIONS

#define ASM_NEWLINE "\n"

STATIC INLINE VOID
CopyMemory(
    PVOID Destination,
    PVOID Source,
    SIZE Size)
{
    __asm__ volatile (
        "rep movsb"
        ::
            "c" (Size),
            "S" (Source),
            "D" (Destination)
    );
}

STATIC INLINE VOID
FarCopyMemory(
    LPVOID Destination,
    LPVOID Source,
    SIZE Size)
{
    __asm__ volatile (
        "push %%ds"     ASM_NEWLINE
        "push %%es"     ASM_NEWLINE
        "mov %0, %%ds"  ASM_NEWLINE
        "mov %1, %%es"  ASM_NEWLINE
        "rep movsb"     ASM_NEWLINE
        "pop %%es"      ASM_NEWLINE
        "pop %%ds"      ASM_NEWLINE
        ::
            "R" (FP_SEGMENT(Source)),
            "R" (FP_SEGMENT(Destination)),
            "c" (Size),
            "S" (FP_OFFSET(Source)),
            "D" (FP_OFFSET(Destination))
    );
}

STATIC INLINE VOID
SetMemoryBytes(
    PBYTE Destination,
    BYTE Value,
    SIZE Size)
{
    __asm__ volatile (
        "rep stosb"
        ::
            "c" (Size),
            "D" (Destination),
            "a" (Value)
    );
}

STATIC INLINE VOID
SetMemoryWords(
    PWORD Destination,
    WORD Value,
    SIZE Size)
{
    __asm__ volatile (
        "rep stosw"
        ::
            "c" (Size),
            "D" (Destination),
            "a" (Value)
    );
}

STATIC INLINE VOID
SetFarMemoryBytes(
    LPBYTE Destination,
    BYTE Value,
    SIZE Size)
{
    __asm__ volatile (
        "push %%es"     ASM_NEWLINE
        "mov %0, %%es"  ASM_NEWLINE
        "rep stosb"     ASM_NEWLINE
        "pop %%es"      ASM_NEWLINE
        ::
            "R" (FP_SEGMENT(Destination)),
            "D" (FP_OFFSET(Destination)),
            "c" (Size),
            "a" (Value)
    );
}

STATIC INLINE VOID
SetFarMemoryWords(
    LPWORD Destination,
    WORD Value,
    SIZE Size)
{
    __asm__ volatile (
        "push %%es"      ASM_NEWLINE
        "mov %%bx, %%es" ASM_NEWLINE
        "rep stosw"      ASM_NEWLINE
        "pop %%es"       ASM_NEWLINE
        ::
            "b" (FP_SEGMENT(Destination)),
            "D" (FP_OFFSET(Destination)),
            "c" (Size),
            "a" (Value)
    );
}

#endif
