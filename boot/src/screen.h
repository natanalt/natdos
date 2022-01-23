#ifndef BOOT_SCREEN_H
#define BOOT_SCREEN_H

#include <natdos/core/core.h>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define MAKE_CHAR(Char, Color) \
    (AS_WORD(Char) | (AS_WORD(Color) << 8))

typedef enum {
    COLOR_BLACK,
    COLOR_DARK_BLUE,
    COLOR_DARK_GREEN,
    COLOR_DARK_CYAN,
    COLOR_DARK_RED,
    COLOR_DARK_MAGENTA,
    COLOR_BROWN,
    COLOR_LIGHT_GRAY,
    COLOR_DARK_GRAY,
    COLOR_LIGHT_BLUE,
    COLOR_LIGHT_GREEN,
    COLOR_LIGHT_CYAN,
    COLOR_LIGHT_RED,
    COLOR_LIGHT_MAGENTA,
    COLOR_YELLOW,
    COLOR_WHITE
} COLOR;

#define POS(X, Y) ((CONPOS){(X), (Y)})
typedef struct {
    BYTE X;
    BYTE Y;
} TYPEDEF_STRUCT(CONPOS);

VOID
InitScreen(VOID);

VOID
ClearScreen(VOID);

VOID
MoveCursor(CONPOS Position);

CONPOS
GetCursorPosition(VOID);

VOID
PrintString(PCSTR String);

#endif
