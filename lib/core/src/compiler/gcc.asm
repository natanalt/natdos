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
        %push
        %stacksize flat
        %assign %$localsize 0
        
        %arg Interrupt:word, \
             Registers:word, \
             Segments:word
    
        %local SavedDi:word

        push bp
        mov bp, sp
        sub sp, %$localsize

        push ds
        push es
        push si
        push di

        mov ax, word [Interrupt]
        mov byte [.Vector], al

        mov di, word [Segments]
        mov ds, word [di + 0]
        mov es, word [di + 2]

        mov di, word [Registers]
        mov ax, word [di + 0]
        mov bx, word [di + 2]
        mov cx, word [di + 4]
        mov dx, word [di + 6]
        mov si, word [di + 8]
        mov di, word [di + 10]

        .Opcode: db 0xcd
        .Vector: db 0x03

        mov word [SavedDi], di
        mov di, word [Segments]
        mov word [di + 0], ax
        mov word [di + 2], bx
        mov word [di + 4], cx
        mov word [di + 6], dx
        mov word [di + 8], si
        mov ax, word [SavedDi]
        mov word [di + 10], ax

        mov di, word [Segments]
        mov word [di + 0], ds
        mov word [di + 2], es

        pop si
        pop di
        pop es
        pop ds

        add sp, %$localsize
        pop bp
        ret
        %pop
