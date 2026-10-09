/*
    CCH-v6c V3F stub linker script: the stub occupies the head of the
    CodeFlash alias window (0x00000000); the V5F bootloader is linked
    256KB in (0x00040000) and the rest of the CodeFlash belongs to the
    stub's reserved area per the DUAL dual-core convention.
*/

ENTRY(_start)

SECTIONS
{
    . = 0x00000000;

    .vectors : ALIGN(4)
    {
        KEEP(*(.vectors))
        /* only cares about text + rodata: keep nothing else */
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    }
}

/* no RAM: the stub is register-only */
