#include <natdos/core/core.h>

// The formatter is currently very limited, and as such it only supports:
//  * %u - WORD sized unsigned base 10 integers
//  * %x - WORD sized unsigned base 16 integers
//  * %c - BYTE sized characters
//  * %s - LPCSTR strings (make sure to cast them properly)

STATIC CHAR Digits[16] = "0123456789abcdef";

SIZE
FormatWithCallback(
    LPCSTR Format,
    FMTCALLBACK Callback,
    LPVOID UserData,
    va_list Variadic)
{
    BOOL ParsingParameter = FALSE;
    SIZE TotalWritten = 0;
    for (CHAR Current = *Format; *Format; Format++)
    {
        if (Current == '%')
        {
            if (ParsingParameter)
            {
                Callback(UserData, TotalWritten, '%');
                TotalWritten += 1;
                ParsingParameter = FALSE;
            }
            else
            {
                ParsingParameter = TRUE;
                continue;
            }
        }
        else if (ParsingParameter)
        {
            WORD Word;
            LPCSTR String;
            switch (Current)
            {
                case 'd':
                    Word = va_arg(Variadic, WORD);
                    if (Word == 0)
                    {
                        Callback(UserData, TotalWritten++, '0');
                    }
                    else
                    {
                        CHAR Buffer[sizeof("65535") - 1] = {'0'};
                        SIZE Current = 4;

                        while (Word != 0)
                        {
                            WORD Digit = Word % 10;
                            Word /= 10;
                            Buffer[Current--] = Digit + '0';
                        }

                        Current++;

                        for (; Current < sizeof(Buffer); Current++)
                        {
                            Callback(UserData, TotalWritten++, Buffer[Current]);
                        }
                    }
                    break;
                case 'x':
                    Word = va_arg(Variadic, WORD);
                    Callback(UserData, TotalWritten++, Digits[(Word >> 12) & 0xf]);
                    Callback(UserData, TotalWritten++, Digits[(Word >> 8) & 0xf]);
                    Callback(UserData, TotalWritten++, Digits[(Word >> 4) & 0xf]);
                    Callback(UserData, TotalWritten++, Digits[(Word >> 0) & 0xf]);
                    break;
                case 'c':
                    Callback(UserData, TotalWritten++, va_arg(Variadic, CHAR));
                    break;
                case 's':
                    String = va_arg(Variadic, LPCSTR);
                    while (*String)
                    {
                        Callback(UserData, TotalWritten++, *(String++));
                    }
                    break;
                default:
                    Panic("Unknown format parameter")
                    break;
            }
            ParsingParameter = FALSE;
        }
        else
        {
            Callback(UserData, TotalWritten++, Current);
        }
    }

    if (ParsingParameter)
    {
        Panic("EOF while parsing a format specifier");
    }

    return TotalWritten;
}

/// UserData structure for FormatStringCallback
typedef struct {
    PSTR Buffer;
    SIZE BufferSize;
} TYPEDEF_STRUCT(FSTRDATA);

/// Callback used by FormatString right below
STATIC VOID
FormatStringCallback(
    LPVOID UserData,
    SIZE Index,
    CHAR Next)
{
    LPFSTRDATA Data = AS(LPFSTRDATA, UserData);
    if (Index >= Data->BufferSize)
    {
        Panic("Buffer overflow in FormatStringCallback");
    }
    Data->Buffer[Index] = Next;
}

SIZE
FormatString(
    LPCSTR Format,
    PSTR Buffer,
    SIZE BufferSize,
    ...)
{
    va_list Variadic;
    SIZE Result;
    FSTRDATA Data = AS(FSTRDATA, { Buffer, BufferSize });
    va_start(Variadic, BufferSize);
    Result = FormatWithCallback(Format, FormatStringCallback, &Data, Variadic);
    va_end(Variadic);
    return Result;
}
