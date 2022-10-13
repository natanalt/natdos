#include "disk.h"
#include "screen.h"

VOID
InitMedia(
    BYTE BiosID,
    PMEDIA Target)
{
    CHAR FormatBuffer[128];
    REGISTERS Registers = {0};

    // TODO: just straight up rework media detection stuff
    //       old PCs are fucking wild
    //       i want my pci back ;-;

    // NOTE: According to OSDev Wiki, getting floppy info from this BIOS service
    //       isn't a good idea. I'm doing it anyway.
    //       ALSO this is IBM XT+
    Registers.A.B.H = 0x08;
    Registers.D.B.L = BiosID;
    CallInterrupt(0x13, &Registers);

    if (Registers.A.B.H != DISK_SUCCESS)
    {
        FormatString(
            "Could not initialize device %x: reported 0x%x",
            PBUFFER(FormatBuffer),
            AS_WORD(BiosID),
            AS_WORD(Registers.A.B.H)
        );
        PrintCriticalError(FormatBuffer);
        FREEZE();
    }

    Target->BiosID = BiosID;
    Target->SectorSizeShift = 9;
    Target->TrackSectors = Registers.C.B.L & 0x3f;
    Target->HeadAmount = Registers.D.B.H + 1;

    if (BiosID == ID_FLOPPY_A)
        Target->TotalSectors = 1440; // standard 3,5" floppy
    else
        Target->TotalSectors = UNKNOWN_TOTAL_SECTORS;
}

DISKSTATUS
ResetMedia(PMEDIA Media)
{
    REGISTERS Registers = {0};
    Registers.A.B.H = 0x00;
    Registers.D.B.L = Media->BiosID;
    CallInterrupt(0x13, &Registers);
    return AS(DISKSTATUS, Registers.A.B.H);
}

CHS
LbaToChs(PMEDIA Media, LBA Lba)
{   
    CHS Result;    
    Result.Sector = (Lba % Media->TrackSectors) + 1;
    DWORD Temp = Lba / Media->TrackSectors;
    Result.Head = Temp % Media->HeadAmount;
    Result.Cylinder = Temp / Media->HeadAmount;

    return Result;
}

DISKSTATUS
ReadSectors(
    PMEDIA Media,
    LBA Lba,
    WORD SectorAmount,
    LPVOID Target)
{
    REGISTERS Registers = {0};
    SEGMENTREGS Segments = GetCurrentSegmentRegisters();

    for (WORD Current = 0; Current < SectorAmount; Current++, Lba++)
    {
        CHS Chs = LbaToChs(Media, Lba);

        if (Chs.Sector == INVALID_CHS_SECTOR)
            return DISK_BAD_SECTOR;

        Registers.A.B.H = 0x02;
        Registers.A.B.L = 1;
        Registers.C.W = Chs.Cylinder << 6;
        Registers.C.W |= Chs.Sector & 0x3f;
        Registers.D.B.H = Chs.Head;
        Registers.D.B.L = Media->BiosID;
        Registers.B.W = FP_OFFSET(Target) + Current * 512;
        Segments.Es = FP_SEGMENT(Target);
        
        CallInterruptWithSegments(0x13, &Registers, &Segments);

        if (Registers.A.B.H != DISK_SUCCESS)
            return AS(DISKSTATUS, Registers.A.B.H);
    }

    return DISK_SUCCESS;
}
