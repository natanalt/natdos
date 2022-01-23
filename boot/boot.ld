OUTPUT_FORMAT(binary)
ENTRY(AsmEntry)

SECTIONS
{
    . = 0x0000;
    .init : {
        *(.init)
    }
    .prog : {
        *(.text)
        *(.data)
        *(.rodata)
        *(.bss)
        FreeStart = ALIGN(2);
    }
}
