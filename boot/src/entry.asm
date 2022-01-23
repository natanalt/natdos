cpu 8086
bits 16

global AsmEntry
extern BootloaderMain

section .init
    AsmEntry:
        cli
        mov ax, cs
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov sp, Stack.Top
        sti

        xor dh, dh
        push dx
        call BootloaderMain

        jmp $


section .bss
    Stack:
        resb 4096
        .Top:
