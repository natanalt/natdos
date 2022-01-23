cpu 8086
bits 16

section .text

    global CallInterruptWithSegments
    CallInterruptWithSegments:
        push bp
        mov bp, sp

        ; Saved for later
        sub sp, 2

        push bx
        push cx
        push dx
        push si
        push di
        push ds
        push es

        mov al, byte [bp + 8]
        mov byte [cs:.Vector], al

        mov si, word [bp + 12]
        mov ds, word [si + 0]
        mov es, word [si + 2]

        mov di, word [bp + 10]
        push di

        mov ax, word [di + 0]
        mov bx, word [di + 2]
        mov cx, word [di + 4]
        mov dx, word [di + 6]
        mov si, word [di + 8]
        mov di, word [di + 10]

        push ax
        mov ah, 0x0e
        mov al, '!'
        int 0x10
        pop ax
        jmp $

        .Opcode: db 0xcd ; int iw
        .Vector: db 0x03

        mov word [bp - 2], di
        pop di

        mov word [di + 0], ax
        mov word [di + 2], bx
        mov word [di + 4], cx
        mov word [di + 6], dx
        mov word [di + 8], si
        mov ax, word [bp - 2]
        mov word [di + 10], ax

        pop es
        pop ds
        pop di
        pop si
        pop dx
        pop cx
        pop bx

        pop bp
        ret
    
    global GetCurrentSegmentRegisters
    GetCurrentSegmentRegisters:
        mov ax, ds
        mov dx, es
        ret
