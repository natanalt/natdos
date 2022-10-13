#include "fat.h"
#include "screen.h"

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
    FatState.RootDirSectors = (BootRecord.DentryCount * sizeof(FATDENTRY)) / 512;
}

STATIC BOOL
CompareFilenames(PCSTR Name, PFATDENTRY Dentry)
{
    SIZE Index;
    SIZE NameIndex = 0;

    Index = 0;
    do
    {
        if (Name[NameIndex++] != Dentry->FileName[Index++])
        {
            return FALSE;
        }
    } while (Name[NameIndex] != '.' && Index < 8);

    NameIndex++;

    TODO("finish this");
    return TRUE;
}

BOOL
FindRootFile(PCSTR Name, PFATDENTRY Result)
{
    UNREFERENCED_PARAMETER(Name);
    UNREFERENCED_PARAMETER(Result);
    return FALSE;
}

VOID
ReadFile(PFATDENTRY Entry, PVOID Target)
{
    UNREFERENCED_PARAMETER(Entry);
    UNREFERENCED_PARAMETER(Target);
}
