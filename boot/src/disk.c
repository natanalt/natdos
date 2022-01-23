#include "disk.h"

DISKSTATUS
InitMedia(
    BYTE BiosID,
    PMEDIA Target)
{
    REGISTERS Registers;

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
        return AS(DISKSTATUS, Registers.A.B.H);

    Target->BiosID = BiosID;
    Target->SectorSizeShift = 9;
    Target->TrackSectors = Registers.C.B.L & 0x3f;
    Target->HeadAmount = Registers.D.B.H + 1;
    Target->TotalSectors = UNKNOWN_TOTAL_SECTORS;

    return DISK_SUCCESS;
}

DISKSTATUS
ResetMedia(PMEDIA Media)
{
    REGISTERS Registers;
    Registers.A.B.H = 0x00;
    Registers.D.B.L = Media->BiosID;
    CallInterrupt(0x13, &Registers);
    return AS(DISKSTATUS, Registers.A.B.H);
}

CHS
LbaToChs(PMEDIA Media, LBA Lba)
{
    if (Lba >= Media->TotalSectors)
        return AS(CHS, { INVALID_CHS_SECTOR, -1, -1 });
    
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
    LPVOID Target,
    PWORD SectorsWritten)
{
    REGISTERS Registers;
    SEGMENTREGS Segments = GetCurrentSegmentRegisters();
    Segments.Es = FP_SEGMENT(Target);
    Registers.B.W = FP_OFFSET(Target);

    while (SectorAmount--)
    {
        CHS Chs = LbaToChs(Media, Lba++);
        if (Chs.Sector == INVALID_CHS_SECTOR)
            return DISK_BAD_SECTOR;

        Registers.A.B.H = 0x02;
        Registers.A.B.L = 1;
        Registers.C.W = Chs.Cylinder << 6;
        Registers.C.W |= Chs.Sector & 0x3f;
        Registers.D.B.H = Chs.Head;
        Registers.D.B.L = Media->BiosID;
        Segments.Es += (512 / 16);
        
        CallInterruptWithSegments(0x13, &Registers, &Segments);

        if (Registers.A.B.H != DISK_SUCCESS)
            return AS(DISKSTATUS, Registers.A.B.H);
    }

    return DISK_SUCCESS;
}
