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
    SIZE Index = 0;
    SIZE TotalWritten = 0;

    while (Format[Index] != 0)
    {
        CHAR Next = Format[Index++];

        if (ParsingParameter)
        {
            switch (Next)
            {
                case 's':
                {
                    LPCSTR S = va_arg(Variadic, LPCSTR);
                    while (*S) Callback(UserData, TotalWritten++, *(S++));
                    break;
                }
                case 'u':
                {
                    WORD Value = va_arg(Variadic, WORD);
                    if (Value != 0)
                    {
                        CHAR Buffer[] = { '6', '5', '5', '3', '5' };
                        SIZE Current = sizeof(Buffer) - 1;

                        while (Value != 0)
                        {
                            WORD Digit = Value % 10;
                            Value /= 10;
                            Buffer[Current--] = Digit + '0';
                        }
                        Current++;

                        for (; Current < sizeof(Buffer); Current++)
                        {
                            Callback(UserData, TotalWritten++, Buffer[Current]);
                        }
                    }
                    else
                    {
                        Callback(UserData, TotalWritten++, '0');
                    }
                    break;
                }
                case 'c':
                {
                    // Minimum size for a stack value is WORD, hence we fetch
                    // a word and then just cast it to CHAR
                    CHAR Value = AS(CHAR, va_arg(Variadic, WORD));
                    Callback(UserData, TotalWritten++, Value);
                    break;
                }
                case 'x':
                {
                    WORD Value = va_arg(Variadic, WORD);
                    Callback(UserData, TotalWritten++, Digits[(Value >> 12) & 0xf]);
                    Callback(UserData, TotalWritten++, Digits[(Value >> 8) & 0xf]);
                    Callback(UserData, TotalWritten++, Digits[(Value >> 4) & 0xf]);
                    Callback(UserData, TotalWritten++, Digits[(Value >> 0) & 0xf]);
                    break;
                }
            }
            ParsingParameter = FALSE;
        }
        else if (Next == '%')
        {
            ParsingParameter = TRUE;
        }
        else
        {
            Callback(UserData, TotalWritten++, Next);
        }
    }

    if (ParsingParameter)
    {
        CRASH("EOF while parsing a format specifier");
    }

    // Final NUL byte
    Callback(UserData, TotalWritten++, 0);

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
        CRASH("Buffer overflow in FormatStringCallback");
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
