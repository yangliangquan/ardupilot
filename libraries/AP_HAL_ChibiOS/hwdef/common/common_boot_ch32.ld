/*
    ChibiOS - Copyright (C) 2006-2015 Giovanni Di Sirio.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0
*/

/*
    this file is included by the board specific ldscript.ld which is
    generated from hwdef.dat

    CH32H417 BOOTLOADER layout (WCH EVT core1 conventions):

    The V5F bootloader image is linked into the zero-wait CodeFlash
    alias window (org 0x00000000 + 256K; the core0/V3F stub occupies
    the first 256KB of CodeFlash). The .start/.handle_reset sections
    run in place from the zero-wait window; all hot code and rodata
    (.highcode) are copied into ITCM (ram_code) at boot by the WCH
    crt0 (loadcode2vma); the .load section goes into ram_load; the
    .data/.bss and stacks live in DTCM (ram).
*/

/* stacks/data live in DTCM (region ram) */
REGION_ALIAS("MAIN_STACK_RAM", ram);
REGION_ALIAS("PROCESS_STACK_RAM", ram);
REGION_ALIAS("DATA_RAM", ram);
REGION_ALIAS("BSS_RAM", ram);
REGION_ALIAS("HEAP_RAM", ram);

/* ITCM for copied code */
__ram_code_start__ = ORIGIN(ram_code);
__ram_code_size__ = LENGTH(ram_code);
__ram_code_end__ = __ram_code_start__ + __ram_code_size__;

__ram0_start__ = ORIGIN(ram);
__ram0_size__ = LENGTH(ram);
__ram0_end__ = __ram0_start__ + __ram0_size__;

ENTRY( _start )

SECTIONS
{
    . = 0;
    _text = .;

    /* V5F crt0 entry: runs from the zero-wait flash window */
    .start :
    {
        _sinit = .;
        . = ALIGN(4);
        KEEP(*(SORT_NONE(.start)))
        . = ALIGN(4);
        _einit = .;
    } > flash AT> flash

    .loadcodelalign :
    {
        . = ALIGN(4);
        PROVIDE(_loadcode_lma = .);
    } > flash AT> flash

    .loadcode :
    {
        . = ALIGN(4);
        PROVIDE(_loadcode_vma_start = .);
        *(.load);
        . = ALIGN(4);
        PROVIDE(_loadcode_vma_end = .);
    } > ram_load AT> flash

    .highcodelalign :
    {
        . = ALIGN(4);
        PROVIDE(_highcode_lma = .);
    } > flash AT> flash

    /* hot code + rodata: copied to ITCM at boot */
    .highcode :
    {
        . = ALIGN(4);
        PROVIDE(_highcode_vma_start = .);
        PROVIDE(vector_start = .);
        *(.vector);
        KEEP(*(SORT_NONE(.vector_handler)))
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        *(.gnu.linkonce.t.*)
        . = ALIGN(4);
        PROVIDE(_highcode_vma_end = .);
    } > ram_code AT> flash

    /* app descriptor + constructors also run from ITCM */
    .appdesc : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.app_descriptor));
    } > ram_code AT> flash

    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > ram_code AT> flash

    .init_array :
    {
        __init_array_base__ = .;
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;
        __init_array_end__ = .;
    } > ram_code AT> flash

    .fini_array :
    {
        __fini_array_base__ = .;
        __fini_array_start = .;
        KEEP(*(.fini_array))
        KEEP(*(SORT(.fini_array.*)))
        __fini_array_end = .;
        __fini_array_end__ = .;
    } > ram_code AT> flash

    /* second .fini/highcode window that the WCH crt1 loadcode2vma()
       also walks; keep it empty but well-formed */
    .highcodelalign1 :
    {
        . = ALIGN(4);
        PROVIDE(_highcode_lma1 = .);
    } > flash AT> flash

    .highcode1 :
    {
        . = ALIGN(4);
        PROVIDE(_highcode_vma_start1 = .);
        . = ALIGN(4);
        PROVIDE(_highcode_vma_end1 = .);
    } > ram_code AT> flash

    . = ALIGN(4);
    PROVIDE(_etext = .);
    PROVIDE(_eitcm = .);

    /* Special section for exceptions stack.*/
    .mstack :
    {
        . = ALIGN(16);
        __main_stack_base__ = .;
        . += __main_stack_size__;
        . = ALIGN(16);
        __main_stack_end__ = .;
        PROVIDE(_eusrstack = .);
        __main_stack_top__ = .;
    } > MAIN_STACK_RAM

    /* Special section for process stack.*/
    .pstack :
    {
        __process_stack_base__ = .;
        __main_thread_stack_base__ = .;
        . += __process_stack_size__;
        . = ALIGN(16);
        __process_stack_end__ = .;
        __main_thread_stack_end__ = .;
    } > PROCESS_STACK_RAM

    .data : ALIGN(4)
    {
        . = ALIGN(4);
        PROVIDE(_data_vma = .);
        __data_base__ = .;
        *(.data)
        *(.data.*)
        *(.sdata)
        *(.sdata.*)
        . = ALIGN(8);
        PROVIDE(__global_pointer$ = . + 0x800);
        *(.srodata.cst16)
        *(.srodata.cst8)
        *(.srodata.cst4)
        *(.srodata.cst2)
        *(.srodata .srodata.*)
        . = ALIGN(4);
        PROVIDE(_edata = .);
        __data_end__ = .;
    } > DATA_RAM AT > flash

    .dlalign :
    {
        . = ALIGN(4);
        PROVIDE(_data_lma = LOADADDR(.data));
    } > flash AT> flash

    .bss (NOLOAD) : ALIGN(4)
    {
        . = ALIGN(4);
        __bss_base__ = .;
        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(.scommon)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        PROVIDE(_sbss = .);
        PROVIDE(_ebss = .);
        PROVIDE(end = .);
    } > BSS_RAM

    /* The default heap uses the (statically) unused part of a RAM section.*/
    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        __heap_base__ = .;
        . = ORIGIN(HEAP_RAM) + LENGTH(HEAP_RAM);
        __heap_end__ = .;
    } > HEAP_RAM

    /* The crash log uses the unused part of a flash section.*/
    .crash_log (NOLOAD) :
    {
        . = ALIGN(32);
        __crash_log_base__ = .;
        . = ORIGIN(flash) + LENGTH(flash);
        __crash_log_end__ = .;
    } > flash
}
