; NatDOS Bootloader 3.5" floppy boot sector
;
; Loads A:\NATBOOT.SYS into 0x1000:0000 and jumps there. This file contains
; stage 2 of the bootloader, with proper UI, better filesystem drivers and
; all the other nice stuff, while also being written in C.
;
; Limitations of stage 1:
;  * can only search root directory
;  * assumes RAM size to be "large" (128KB+), as stage 2 is loaded at 64KB
;    RAM offset, with max stage 2 size also being 64KB.
;

%use ifunc ; for ilog2

cpu 8086
org 0x7c00

%define STAGE2_SEGMENT 0x1000

%define DENTRY_SIZE 32
%define DENTRIES_PER_SECTOR (512 / DENTRY_SIZE)

section .text
    jmp short Main
    nop

    FatInfo:
        .OemId:            db "NATDOS  "
        .SectorBytes:      dw 512
        .ClusterSectors:   db 1
        .ReservedSectors:  dw 1
        .FatAmount:        db 2
        .RootDentryAmount: dw 224
        .SectorAmount:     dw 2880
        .MediaDescriptor:  db 0xF0
        .FatSectors:       dw 9
        .TrackSectors:     dw 18
        .HeadAmount:       dw 2
        .HiddenSectors:    dd 0
        .LargeSectors:     dd 0
        .DriveNumber:      db 0
        .NtFlags:          db 0
        .Signature:        db 0x28
        .VolumeSerial:     dd 0xB00B1234
        .VolumeLabel:      db "NatDOS Live"
        .FsId:             db "FAT12   "
    
    ;;
    ; Stage 1's entry point
    ; Does not return, obviously, as it doesn't have a place to return.
    ; Very tragic indeed.
    ;
    Main:
        cli
        xor ax, ax
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov sp, 0x7c00
        sti
        
        mov byte [BootDrive], dl
        mov word [Sector.LoadedLBA], 0xffff

        call FindStageTwo
        jc FileNotFoundError

        call LoadStageTwo
        
        mov dl, byte [BootDrive]
        jmp STAGE2_SEGMENT:0000

    ;;
    ; Looks for StageTwoFile
    ; Return ax as the cluster, or 0xffff if not found (+ carry set on error)
    ;
    FindStageTwo:

        ; Get the root directory sector amount
        mov ax, word [FatInfo.RootDentryAmount]
        mov cl, ilog2(DENTRIES_PER_SECTOR)
        shr ax, cl
        mov cx, ax

        ; Get the first root directory sector
        mov ax, word [FatInfo.FatSectors]
        xor bh, bh
        mov bl, byte [FatInfo.FatAmount]
        mul bx
        add ax, word [FatInfo.ReservedSectors]

        .SectorLoop:
            push cx
            push ax
            call ReadSector
            
            mov si, Sector.Buffer 
            mov cx, DENTRIES_PER_SECTOR
            .DentryLoop:
                push cx
                push si

                cld
                mov di, StageTwoFile
                mov cx, 8+3
                repe cmpsb
                je .FinishNoError

                pop si
                add si, DENTRY_SIZE
                pop cx
                loop .DentryLoop

            pop ax
            inc ax
            pop cx
            loop .SectorLoop

        .FinishError:
            mov ax, 0xffff
            stc
            ret

        .FinishNoError:
            ; Continue from .DentryLoop

            pop si ; Get the dentry offset
            mov ax, word [si + 26]

            add sp, 6 ; Pop cx, ax, cx

            clc
            ret

    ;;
    ; Loads stage 2 starting at cluster in ax
    ; Should return or panic on error
    ;
    ; Causes the inevitable destruction of all and any values written into
    ; registers of this 8086 or 8088 compatible microprocessor, which may
    ; be an unwanted side behavior of this function, therefore anyone
    ; who wants to use this function should be wary of the machine state
    ; modification caused upon a function return to not end up spending
    ; unnecessary amounts of time trying to debug the ineviatble mess that
    ; will happen from sudden and unexpected register value changes.
    ;
    LoadStageTwo:

        mov dx, STAGE2_SEGMENT
        mov es, dx

        .ReadLoop:
            cmp ax, 0xff7
            je BadClusterError
            cmp ax, 0xff8
            jge .Finish

            push ax
            call TranslateClusterIntoLBA
            xor bx, bx
            call ReadSectorTo
            pop ax

            mov dx, es
            add dx, 512 / 16
            mov es, dx

            push es
            push ds
            pop es
            call ReadFatEntry
            pop es

            jmp .ReadLoop

        .Finish:
            ret

    ;;
    ; Input: ax - cluster
    ; Output: ax - BLA
    ; Performs ax = ax - 2 + FatSectors + DentrySectors + ReservedSectors, ending up
    ; with an LBA suitable for disk reading
    ;
    ; will do the wiping on registers
    ;
    TranslateClusterIntoLBA:
        mov si, ax

        sub si, 2

        mov ax, word [FatInfo.FatSectors]
        xor bh, bh
        mov bl, byte [FatInfo.FatAmount]
        mul bx
        add si, ax

        mov ax, word [FatInfo.RootDentryAmount]
        mov cl, ilog2(DENTRIES_PER_SECTOR)
        shr ax, cl
        add si, ax
        
        add si, word [FatInfo.ReservedSectors]

        mov ax, si
        ret

    ;;
    ; Input: ax - cluster index
    ; Output: ax - cluster index but from the table
    ; Trashes them regs
    ; https://wiki.osdev.org/FAT#FAT_12_2
    ;
    ReadFatEntry:

        push ax

        ; FatOffset -> ax = ax + ax / 2
        mov bx, ax
        shr bx, 1
        add ax, bx

        ; Read the table sector
        push ax
        mov cl, ilog2(512)
        shr ax, cl
        add ax, word [FatInfo.ReservedSectors]
        call ReadSector
        pop ax

        and ax, 511
        mov si, ax
        add si, Sector.Buffer
        mov bx, word [si]

        pop ax

        test ax, 1
        jz .Even

        .Odd:
            mov cl, 4
            shr bx, cl
            mov ax, bx
            ret

        .Even:
            and bx, 0xfff
            mov ax, bx
            ret

    ;;
    ; Prints a 0-terminated string at ds:si
    ; Modifies ax and si
    ;
    PrintString:
        cld
        .PrintLoop:
            lodsb
            test al, al
            jz .Finish
            mov ah, 0x0e
            int 0x10
            jmp .PrintLoop
        .Finish:
            ret
    
    ;;
    ; ax - LBA
    ; All registers may be modified
    ;
    ReadSector:
        cmp ax, word [Sector.LoadedLBA]
        je .Finish
        
        mov word [Sector.LoadedLBA], ax

        mov bx, Sector.Buffer
        call LbaToChs
        int 0x13
        jc ReadError

        .Finish:
            ret
    
    ;;
    ; ax - LBA
    ; es:bx - buffer
    ; All registers may be modified
    ;
    ReadSectorTo:
        call LbaToChs
        int 0x13
        jc ReadError
        ret

    ;;
    ; ax - LBA
    ; https://wiki.osdev.org/Disk_access_using_the_BIOS_(INT_13h)#The_Algorithm
    ; Additionally sets up registers for a single sector disk read
    ;
    LbaToChs:
        push bx

        ; ax = temp value, bx = sector
        xor dx, dx
        div word [FatInfo.TrackSectors]
        inc dx
        mov bx, dx

        ; ax = cylinder, dx = head
        xor dx, dx
        div word [FatInfo.HeadAmount]
        
        ; cl = sector | ((cylinder >> 2) & 0xC0)
        mov cl, bl
        mov bx, ax
        shr bx, 1
        shr bx, 1
        and bx, 0xC0
        or cl, bl

        mov ch, al ; ch = cylinder & 0xFF
        mov dh, dl ; dh = head

        mov dl, byte [BootDrive]
        mov ax, 0x0201

        pop bx
        ret
    
    ;;
    ; Calls PrintString, then freezes
    ;
    Panic:
        call PrintString
    Freeze:
        cli
        jmp Freeze
    
    ReadError:
        mov si, Errors.ReadError
        jmp Panic
    FileNotFoundError:
        mov si, Errors.NotFound
        jmp Panic
    BadClusterError:
        mov si, Errors.BadCluster
        jmp Panic
    StageTwoFile: db "NATBOOT SYS"
    Errors:
        .ReadError: db "Read error", 0
        .NotFound: db "File not found", 0
        .BadCluster: db "Bad cluster", 0

    jmp $
    times 510 - ($ - $$) int3
    dw 0xAA55
    
section .bss

    BootDrive: resb 1

    Sector:
        .LoadedLBA: resw 1
        .Buffer: resb 512
