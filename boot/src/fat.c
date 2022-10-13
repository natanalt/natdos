#include "fat.h"
#include "screen.h"
#include <natdos/core/text.h>

static struct {
    PMEDIA Media;

    LBA CachedBuffer;
    BYTE SectorBuffer[512];

    WORD SectorsPerCluster;

    WORD ReservedSectors;
    WORD FatAmount;
    WORD SectorsPerFat;
    WORD RootDirSectors;
} FatState;

typedef struct {
    BYTE JumpShortNop[3];
    CHAR OemID[8];
    WORD BytesPerSector;
    BYTE SectorsPerCluster;
    WORD ReservedSectors;
    BYTE FatTableCount;
    WORD DentryCount;
    WORD TotalSectors16;
    BYTE MediaType;
    WORD SectorsPerFat;
    WORD SectorsPerTrack;
    WORD HeadCount;
    DWORD HiddenSectors;
    DWORD TotalSectors32;
    BYTE DriveNumber;
    BYTE NtReserved;
    BYTE Signature;
    BYTE VolumeID[4];
    CHAR VolumeLabel[11];
    CHAR FilesystemID[8];
    BYTE BootCode[448];
    WORD PartitionSignature;
} PACKED TYPEDEF_STRUCT(BOOTRECORD);
STATIC_ASSERT(sizeof(BOOTRECORD) == 512, "Invalid boot record size");

VOID
InitFat(PMEDIA Media)
{
    CHAR FormatBuffer[128];
    BOOTRECORD BootRecord;
    DISKSTATUS DiskStatus;

    DiskStatus = ReadSectors(Media, 0, 1, &BootRecord);
    if (!IS_DISK_SUCCESS(DiskStatus))
    {
        FormatString(
            "Could not read the boot sector: reported 0x%x (device %x)",
            PBUFFER(FormatBuffer),
            AS_WORD(DiskStatus),
            AS_WORD(Media->BiosID)
        );
        PrintCriticalError(FormatBuffer);
        FREEZE();
    }

    if (Media->TotalSectors == UNKNOWN_TOTAL_SECTORS)
    {
        if (BootRecord.TotalSectors16 != 0)
        {
            Media->TotalSectors = AS_DWORD(BootRecord.TotalSectors16);
        }
        else
        {
            Media->TotalSectors = BootRecord.TotalSectors32;
        }
    }

    FatState.Media = Media;
    FatState.CachedBuffer = -1;
    FatState.SectorsPerCluster = BootRecord.SectorsPerCluster;
    FatState.ReservedSectors = BootRecord.ReservedSectors;
    FatState.SectorsPerFat = BootRecord.SectorsPerFat;
    FatState.FatAmount = BootRecord.FatTableCount;
    FatState.RootDirSectors = BootRecord.DentryCount / 16;
}

STATIC INLINE VOID
ConvertFilenameToRaw(PCSTR Name, PSTR Target)
{
    PSTR CurrentOut = Target;
    while (*Name)
    {
        CHAR Next = *(Name++);

        if (Next == '.')
        {
            CurrentOut = Target + 8;
            continue;
        }

        *(CurrentOut++) = AsciiToUpper(Next);
    }
}

BOOL
FindRootFile(PCSTR Name, PFATDENTRY Result)
{
    FATDENTRY Dentries[512 / sizeof(FATDENTRY)];

    CHAR RawFilename[11];
    ConvertFilenameToRaw(Name, RawFilename);

    WORD Base = FatState.ReservedSectors;
    Base += FatState.FatAmount * FatState.SectorsPerFat;

    for (WORD Sector = 0; Sector < FatState.RootDirSectors; Sector++)
    {
        DISKSTATUS Status = ReadSectors(
            FatState.Media,
            Base + Sector,
            1,
            Dentries
        );
        
        if (!IS_DISK_SUCCESS(Status))
        {
            PrintCriticalError("couldn't read a dentry sector");
            FREEZE();
        }

        for (WORD Index = 0; Index < 512 / 32; Index++)
        {
            BOOL AreEqual = CompareMemoryBytes(
                Dentries[Index].FileName,
                RawFilename,
                sizeof(RawFilename)
            );

            if (AreEqual)
            {
                *Result = Dentries[Index];
                return TRUE;
            }
        }
    }

    return FALSE;
}

STATIC VOID
ReadCacheSector(LBA Lba)
{
    if (FatState.CachedBuffer == Lba)
        return;
    FatState.CachedBuffer = Lba;

    DISKSTATUS Status = ReadSectors(
        FatState.Media,
        Lba,
        1,
        &FatState.SectorBuffer
    );

    if (!IS_DISK_SUCCESS(Status))
    {
        PrintCriticalError("couldn't read a sector");
        FREEZE();
    }
}

STATIC WORD
ReadClusterValue(WORD Index)
{
    WORD AbsoluteOffset = (Index * 3) / 2;
    LBA SectorLba = AS(LBA, FatState.ReservedSectors) + AbsoluteOffset / 512;
    WORD InnerOffset = AbsoluteOffset % 512;

    ReadCacheSector(SectorLba);

    WORD Value = *AS(PWORD, FatState.SectorBuffer + InnerOffset);

    if (Index % 2 == 0)
        return Value & 0xfff;
    else
        return Value >> 4;
}

STATIC VOID
ReadCluster(WORD Index, PVOID Target)
{
    LBA Lba = AS(LBA, Index)
        - 2
        + FatState.SectorsPerFat * FatState.FatAmount
        + FatState.RootDirSectors
        + FatState.ReservedSectors;
    
    DISKSTATUS Status = ReadSectors(
        FatState.Media,
        Lba,
        FatState.SectorsPerCluster,
        Target
    );

    if (!IS_DISK_SUCCESS(Status))
    {
        CHAR FormatBuffer[128];
        FormatString(
            "Cluster read error. (%x)",
            PBUFFER(FormatBuffer),
            AS_WORD(Status)
        );

        PrintCriticalError(FormatBuffer);
        FREEZE();
    }
}

VOID
ReadFile(PFATDENTRY Entry, PVOID VTarget)
{
    WORD CurrentCluster = Entry->ClusterLo;
    PBYTE Target = AS(PBYTE, VTarget);

    while (CurrentCluster < 0xff8)
    {
        if (CurrentCluster == 0xff7)
        {
            PrintCriticalError("Encoutered a bad sector.");
            FREEZE();
        }

        ReadCluster(CurrentCluster, Target);
        CurrentCluster = ReadClusterValue(CurrentCluster);

        Target += FatState.SectorsPerCluster * 512;
    }
}
