#include <natdos/core/core.h>
#include "memory.h"
#include "screen.h"
#include "fat.h"

VOID
CrashHandler(PCSTR Message)
{
    PrintCriticalError(Message);
    FREEZE();
}

VOID
BootloaderMain(BYTE BootDeviceId)
{
    MEDIA BootDevice;
    FATDENTRY BootLogoFile;

    InitScreen();
    InitAllocator();
    InitMedia(BootDeviceId, &BootDevice);
    InitFat(&BootDevice);

    if (!FindRootFile("BOOTLOGO.SYS", &BootLogoFile))
    {
        PrintCriticalError("BOOTLOGO.SYS not found");
        FREEZE();
    }

    PWORD BootLogoBuffer = AllocateMemory(4096);
    SetScreenEnabled(FALSE);
    ReadFile(&BootLogoFile, BootLogoBuffer);
    FarCopyMemory(MAKE_FAR(0xb800, 0), BootLogoBuffer, 80 * 25 * 2);
    FreeMemory(BootLogoBuffer);
    SetScreenEnabled(TRUE);

    WriteAtCentered(12, 0x0f, "Bootloader");
    WriteAtCentered(13, 0x0f, "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD");

    FREEZE();
}
