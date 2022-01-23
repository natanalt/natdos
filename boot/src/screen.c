#include "screen.h"

STATIC CONPOS CurrentPosition;

VOID
InitScreen(VOID)
{
    // TODO: configure the CGA manually instead of using the BIOS 
    REGISTERS Registers;
    Registers.A.W = 0x1003;
    Registers.B.W = 0;
    CallInterrupt(0x10, &Registers);

    ClearScreen();
    MoveCursor(POS(0, 0));
}

VOID
ClearScreen(VOID)
{
    SetFarMemoryWords(
        MAKE_FAR(0xb800, 0),
        MAKE_CHAR(' ', COLOR_BLACK),
        80 * 25);
}

VOID
MoveCursor(CONPOS Position)
{
    REGISTERS Registers;
    Registers.A.B.H = 2;
    Registers.B.B.H = 0;
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

STATIC VOID
ScrollScreen(VOID)
{
    TODO("ScrollScreen");
}

STATIC VOID
AdvanceInternalCursor(VOID)
{
    CurrentPosition.X += 1;
    if (CurrentPosition.X >= SCREEN_WIDTH)
    {
        CurrentPosition.X = 0;
        CurrentPosition.Y += 1;
        if (CurrentPosition.Y >= SCREEN_HEIGHT)
        {
            CurrentPosition.Y = SCREEN_HEIGHT - 1;
            ScrollScreen();
        }
    }
}

STATIC VOID
PlaceAt(
    CONPOS Position,
    WORD Value)
{
    WORD Index = AS_WORD(Position.Y) * SCREEN_WIDTH + Position.X;
    MAKE_FAR_AS(LPWORD, 0xb800, 0)[Index] = Value;
}

VOID
PrintString(PCSTR String)
{
    while (*String)
    {
        CHAR Current = *(String++);

        PlaceAt(CurrentPosition, MAKE_CHAR(Current, COLOR_WHITE));      
        AdvanceInternalCursor();
    }
}
