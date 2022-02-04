#include <natdos/core/core.h>
#include "memory.h"
#include "screen.h"
#include "fat.h"

VOID
BootloaderMain(BYTE BootDeviceId)
{
    MEDIA BootDevice;
    FATDENTRY BootLogoFile;
    PWORD BootLogoBuffer;

    InitScreen();
    InitAllocator();
    InitMedia(0x00, &BootDevice);
    InitFat(&BootDevice);

    if (!FindRootFile("BOOTLOGO.SYS", &BootLogoFile))
    {
        PrintCriticalError("BOOTLOGO.SYS not found");
        FREEZE();
    }

    BootLogoBuffer = AllocateMemory(80 * 25 * 2);
    SetScreenEnabled(FALSE);
    ReadFile(&BootLogoFile, BootLogoBuffer);
    FarCopyMemory(MAKE_FAR(0xb800, 0), BootLogoBuffer, 80 * 25 * 2);
    FreeMemory(BootLogoBuffer);
    SetScreenEnabled(TRUE);

    FREEZE();
}
