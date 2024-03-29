#ifndef BOOT_DISK_H
#define BOOT_DISK_H

#include <natdos/core/core.h>

TYPEDEFS(DWORD, LBA);

#define ID_FLOPPY_A AS(BYTE, 0) // aka. A:
#define ID_FLOPPY_B AS(BYTE, 1) // aka. B:

#define HDD_BIT 0x80
#define ID_HDD_A AS(BYTE, 0x80) // aka. C:
#define ID_HDD_B AS(BYTE, 0x81)

// The filesystem driver will detect such a low value and try fetching data
// from the boot sector. Not great, not terrible
#define UNKNOWN_TOTAL_SECTORS AS_DWORD(-1)

typedef struct {
    BYTE BiosID;
    WORD SectorSizeShift; // Sector size = 1 << SectorSizeShift
    WORD TrackSectors;
    WORD HeadAmount;
    DWORD TotalSectors;
} TYPEDEF_STRUCT(MEDIA);

#define INVALID_CHS_SECTOR 0xff

typedef struct {
    BYTE Sector;
    BYTE Head;
    WORD Cylinder;
} TYPEDEF_STRUCT(CHS);

typedef enum {
    DISK_SUCCESS = 0x00,
    DISK_BAD_COMMAND = 0x01,
    DISK_BAD_SECTOR = 0x02,
    DISK_WRITE_PROTECT_ERROR = 0x03,
    DISK_SECTOR_NOT_FOUND = 0x04,
    DISK_FIXED_RESET_FAILED = 0x05,
    DISK_DISKETTE_CHANGED = 0x06,
    DISK_BAD_FIXED_DPT = 0x07,
    DISK_DMA_OVERRUN = 0x08,
    DISK_DMA_ACCESS_OVER_BOUNDARY = 0x09,
    DISK_BAD_FIXED_SECTOR_FLAG = 0x0A,
    DISK_BAD_FIXED_CYLINDER = 0x0B,
    DISK_INVALID_MEDIA = 0x0C,
    DISK_INVALID_SECTOR_AMOUNT = 0x0D,
    DISK_CONTROLLED_DATA_ADDRESS_MARK = 0x0E,
    DISK_DMA_ARBITRATION_OUT_OF_RANGE = 0x0F,
    DISK_CRC_ERROR = 0x10,
    DISK_CRC_ERROR_FIXED = 0x11,
    DISK_CONTROLLER_ERROR = 0x20,
    DISK_SEEK_FAILURE = 0x40,
    DISK_DRIVE_NOT_READY = 0x80,
    DISK_FIXED_NOT_READY = 0xAA,
    DISK_FIXED_UNKNOWN_ERROR = 0xBB,
    DISK_FIXED_WRITE_FAULT = 0xCC,
    DISK_FIXED_STATUS_ERROR = 0xE0,
    DISK_SENSE_OPERATION_FAILED = 0xFF
} DISKSTATUS;

#define IS_DISK_SUCCESS(x) (AS(DISKSTATUS, x) == DISK_SUCCESS)

VOID
InitMedia(BYTE BiosID, PMEDIA Target);

DISKSTATUS
ResetMedia(PMEDIA Media);

CHS
LbaToChs(PMEDIA Media, LBA Lba);

DISKSTATUS
ReadSectors(
    PMEDIA Media,
    LBA Lba,
    WORD SectorAmount,
    PVOID Target);

#endif
