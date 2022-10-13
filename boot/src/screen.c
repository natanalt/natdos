#include "screen.h"

STATIC CONPOS CurrentPosition;
STATIC BOOL CursorVisible;
STATIC BOOL ScreenEnabled;

STATIC LPWORD
GetBufferAddressFor(CONPOS Position)
{
    LPWORD Base = MAKE_FAR(0xb800, 0);
    return Base + (Position.Y * SCREEN_WIDTH + Position.X);
}

VOID
InitScreen(VOID)
{
    // Enable high intensity colour
    // TODO: configure the CGA manually instead of using the BIOS
    REGISTERS Registers = {0};
    Registers.A.W = 0x1003;
    Registers.B.W = 0;
    CallInterrupt(0x10, &Registers);

    ClearScreen();
    SetCursorVisible(FALSE);
    MoveCursor(POS(0, 0));
    SetScreenEnabled(TRUE);
}

VOID
ClearScreen(VOID)
{
    SetFarMemoryWords(
        MAKE_FAR(0xb800, 0),
        CELL(' ', ATTRIBUTE(WHITE, BLACK)),
        SCREEN_WIDTH * SCREEN_HEIGHT
    );
}

VOID
SetScreenEnabled(BOOL Enabled)
{
    // TODO: enable/disable the screen on the CGA
    ScreenEnabled = Enabled;
}

BOOL
IsScreenEnabled(VOID)
{
    return ScreenEnabled;
}

VOID
MoveCursor(CONPOS Position)
{
    REGISTERS Registers = {0};
    Registers.A.W = 0x0200;
    Registers.B.W = 0;
    Registers.D.B.H = Position.Y;
    Registers.D.B.L = Position.X;
    CallInterrupt(0x10, &Registers);

    CurrentPosition = Position;
}

CONPOS
GetCursorPosition(VOID)
{
    return CurrentPosition;
}

VOID
SetCursorVisible(BOOL Visible)
{
    REGISTERS Registers = {0};
    Registers.A.W = 0x0100;
    Registers.C.W = Visible ? 0x0607 : 0x2000;
    CallInterrupt(0x10, &Registers);

    CursorVisible = Visible;
}

BOOL
IsCursorVisible(VOID)
{
    return CursorVisible;
}

VOID
WriteAt(
    CONPOS Position,
    BYTE Attribute,
    PCSTR Text)
{
    LPWORD CurrentAddr = GetBufferAddressFor(Position);
    CHAR CurrentChar;

    while ((CurrentChar = *(Text++)))
        *(CurrentAddr++) = CELL(CurrentChar, Attribute);
}

VOID
PrintCriticalError(PCSTR String)
{
    WORD Top = 11;

    SIZE Length = GetStringSize(String);
    BYTE LeftX = (SCREEN_WIDTH / 2) - (Length / 2);

    WriteAt(POS(LeftX, Top), ATTRIBUTE(LIGHT_RED, BLACK), String);
    SetFarMemoryWords(
        GetBufferAddressFor(POS(LeftX - 1, Top + 1)),
        CELL(223, ATTRIBUTE(DARK_RED, BLACK)),
        Length + 2);
}
