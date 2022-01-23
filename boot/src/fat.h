#ifndef BOOT_FAT_H
#define BOOT_FAT_H

#include <natdos/core/core.h>
#include "disk.h"

#define FAT_READ_ONLY (1 << 0)
#define FAT_HIDDEN    (1 << 1)
#define FAT_SYSTEM    (1 << 2)
#define FAT_VOLUME_ID (1 << 3)
#define FAT_DIRECTORY (1 << 4)
#define FAT_ARCHIVE   (1 << 5)
#define FAT_LFN       (FAT_READ_ONLY | FAT_HIDDEN | FAT_SYSTEM | FAT_VOLUME_ID)

typedef struct {
    CHAR FileName[8];
    CHAR FileExt[3];
    BYTE Attributes;
    BYTE Reserved;
    BYTE CreationTimeSecs;
    WORD CreationTime;
    WORD CreationDate;
    WORD LastAccessDate;
    WORD ClusterHi;
    WORD LastModificationTime;
    WORD LastModificationDate;
    WORD ClusterLo;
    DWORD FileSize; 
} PACKED TYPEDEF_STRUCT(FATDENTRY);

VOID
OpenFat(PMEDIA Media);

BOOL
FindRootFile(PCSTR Name, PFATDENTRY Result);

VOID
ReadFile(PFATDENTRY Entry, VOID* Target);

#endif
