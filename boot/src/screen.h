#ifndef BOOT_SCREEN_H
#define BOOT_SCREEN_H

#include <natdos/core/core.h>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define ATTRIBUTE(Fg, Bg) \
    ((Bg) << 4 | (Fg))

#define CELL(Char, Attr) \
    (AS_WORD(Char) | AS_WORD(Attr) << 8)

typedef enum {
    BLACK,
    DARK_BLUE,
    DARK_GREEN,
    DARK_CYAN,
    DARK_RED,
    DARK_MAGENTA,
    BROWN,
    LIGHT_GRAY,
    DARK_GRAY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGHT_MAGENTA,
    YELLOW,
    WHITE
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
SetScreenEnabled(BOOL Enabled);

BOOL
IsScreenEnabled(VOID);

VOID
MoveCursor(CONPOS Position);

CONPOS
GetCursorPosition(VOID);

BOOL
IsCursorVisible(VOID);

VOID
SetCursorVisible(BOOL Visible);

VOID
WriteAt(
    CONPOS Position,
    BYTE Attribute,
    PCSTR Text);

VOID
PrintCriticalError(PCSTR String);

#endif
