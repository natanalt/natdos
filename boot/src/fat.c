#include "fat.h"

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

VOID
InitFat(PMEDIA Media)
{

}

BOOL
FindRootFile(PCSTR Name, PFATDENTRY Result)
{
    return FALSE;
}

VOID
ReadFile(PFATDENTRY Entry, PVOID Target)
{

}
