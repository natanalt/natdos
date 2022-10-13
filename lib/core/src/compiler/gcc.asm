cpu 8086
bits 16

section .text

    ;; VOID
    ;; CallInterruptWithSegments(
    ;;     BYTE Interrupt,
    ;;     PREGISTERS Registers,
    ;;     PSEGMENTREGS Segments);
    global CallInterruptWithSegments
    CallInterruptWithSegments:
        %define Interrupt (bp + 4)
        %define Registers (bp + 6)
        %define Segments (bp + 8)
        %define SavedDi (bp - 2)

        push bp
        mov bp, sp
        sub sp, 2 ; SavedDi

        push ds
        push es
        push si
        push di
        
        push dx
        push cx
        push bx
        push ax

        mov ax, word [Interrupt]
        mov byte [.Vector], al

        mov di, word [Segments]
        mov ds, word [ss:di + 0]
        mov es, word [ss:di + 2]

        mov di, word [Registers]
        mov ax, word [ss:di + 0]
        mov bx, word [ss:di + 2]
        mov cx, word [ss:di + 4]
        mov dx, word [ss:di + 6]
        mov si, word [ss:di + 8]
        mov di, word [ss:di + 10]

        .Opcode: db 0xcd
        .Vector: db 0x03

        mov word [SavedDi], di
        mov di, word [Segments]
        mov word [ss:di + 0], ax
        mov word [ss:di + 2], bx
        mov word [ss:di + 4], cx
        mov word [ss:di + 6], dx
        mov word [ss:di + 8], si
        mov ax, word [SavedDi]
        mov word [ss:di + 10], ax

        mov di, word [Segments]
        mov word [ss:di + 0], ds
        mov word [ss:di + 2], es

        pop ax
        pop bx
        pop cx
        pop dx

        pop si
        pop di
        pop es
        pop ds

        add sp, 2 ; SavedDi
        pop bp
        ret
        
        %undef Interrupt
        %undef Registers
        %undef Segments
        %undef SavedDi
