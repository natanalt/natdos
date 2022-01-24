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

    //InitScreen();
    InitAllocator();
    InitMedia(0x00, &BootDevice);
    InitFat(&BootDevice);

    BYTE Sector[512];
    ResetMedia(&BootDevice);
    ReadSectors(
        &BootDevice,
        1,
        2,
        Sector);
    //Sector[10] = 0;
    WriteAt(POS(1, 1), 0xcf, &Sector[3]);

    LPWORD Buf = MAKE_FAR(0xb800, 0);
    for (int i = 0; i < 512; i++) {
        Buf[i] = CELL(Sector[i], 0x0f);
    }

    FREEZE();

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
