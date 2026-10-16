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

    CH32H417 (QingKe V5F) application: XIP from the external QSPI flash,
    which the bootloader has already mapped and clocked.

      0x70000000  1K fw_describer, then .text/.rodata run from flash
      instram    0x200A0000  128K ITCM, hot code
      flashram   0x20100000  64K  AXI SRAM, next most critical code
      dataram    0x200C0000  256K DTCM, stacks/.data/.bss (first 512B lost)
      ram0       0x20110000  448K shared SRAM, DMA buffers and heap

    Three images are copied into RAM before main(), and the V5F startup
    only has two copy loops, so a new RAM image has to be folded into one
    of them:

      .fastcode  ITCM      _highcode_lma  / _highcode_vma_start/_end
      .ramfunc   flashram  _highcode_lma1 / _highcode_vma_start1/_end1
                                 both walked by crt1.c loadcode2vma()
      .data      DTCM      _data_lma      / _data_vma/_edata
                                 walked by crt1.c loaddata2vma()

    .bss is cleared by crt1.c clearbss(), and __init_ram_areas() (board.c)
    only zeroes the .ram0_clear range. The ChibiOS __ramN_init__ /
    __ramN_clear__ / __ramN_noinit__ mechanism is deliberately NOT used
    here: nothing on the RISC-V port reads those symbols.
*/

/* the first 512B of DTCM are unusable on this part */
__dtcm_reserved__ = 0x200;

/* stacks/.data/.bss in DTCM: zero wait, and not reachable by DMA */
/* heap in the shared SRAM: large and DMA capable */
REGION_ALIAS("MAIN_STACK_RAM", dataram);
REGION_ALIAS("PROCESS_STACK_RAM", dataram);
REGION_ALIAS("DATA_RAM", dataram);
REGION_ALIAS("BSS_RAM", dataram);
REGION_ALIAS("HEAP_RAM", ram0);

__ram0_start__ = ORIGIN(ram0);
__ram0_size__  = LENGTH(ram0);
__ram0_end__   = __ram0_start__ + __ram0_size__;

__ram1_start__ = ORIGIN(flashram);
__ram1_size__  = LENGTH(flashram);
__ram1_end__   = __ram1_start__ + __ram1_size__;

__ram2_start__ = ORIGIN(dataram);
__ram2_size__  = LENGTH(dataram);
__ram2_end__   = __ram2_start__ + __ram2_size__;

__ram3_start__ = ORIGIN(instram);
__ram3_size__  = LENGTH(instram);
__ram3_end__   = __ram3_start__ + __ram3_size__;

ENTRY( _start )

SECTIONS
{
    /* The bootloader reads word 0 as the initial SP and word 1 as the
       entry point, then refuses to boot if either is 0xFFFFFFFF or if the
       entry is below APP_START_ADDRESS. */
    .fw_describer ORIGIN(default_flash) : ALIGN(4)
    {
        . = ALIGN(4);
        __fw_describer_base__ = .;
        LONG(_eusrstack);
        LONG(_start);
        KEEP(*(.app_descriptor))
        KEEP(*(.apsec_data))
        . = ALIGN(1024);
        __fw_describer_end__ = .;
    } > default_flash

    . = ALIGN(1024);
    _text = .;

    /* .start/.handle_reset run before anything is copied into RAM */
    .start :
    {
        . = ALIGN(4);
        _sinit = .;
        KEEP(*(SORT_NONE(.start)))
        . = ALIGN(4);
        _einit = .;
    } > default_flash

    /* _port_irq_handler, the unified interrupt entry: the hardware needs
       it on a 1K boundary and it runs on every interrupt, so it goes in
       ITCM. It sits ahead of .fastcode so that the loadcode2vma() window
       below is still one unpadded contiguous run. */
    .irq_entry : ALIGN(4) SUBALIGN(4)
    {
        PROVIDE(irq_entry_lma = LOADADDR(.irq_entry));
        PROVIDE(irq_entry_vma = .);
        . = ALIGN(4);
        KEEP(*(.irq_entry))
        . = ALIGN(4);
        PROVIDE(irq_entry_vma_end = .);
    } > instram AT > default_flash

    /* ITCM. Keep the list tight: 128K only, and the QSPI prefetcher is
       already good enough for the bulk of the code. */
    .fastcode : ALIGN(4) SUBALIGN(4)
    {
        . = ALIGN(4);
        /* loadcode2vma() copies word by word from _highcode_lma, so this
           must be one tightly packed run: same input sections, same order
           in the VMA and in the LMA, nothing in between */
        PROVIDE(_highcode_vma_start = .);
        *libch.a:ch*.*(.text*)
        *libch.a:hal*.*(.text*)
        *libch.a:hrt.*(.text*)
        *libch.a:bouncebuffer.*(.text*)
        *libch.a:memstreams.*(.text*)
        *libch.a:malloc.*(.text*)
        lib/lib*.a:Semaphores.*(.text*)
        lib/lib*.a:AP_Math.*(.text*)
        lib/lib*.a:vector3.*(.text*)
        lib/lib*.a:matrix3.*(.text*)
        *(.fastramfunc)         /* also PORT_IRQ_ATTRIBUTES */
        *(.fastcode*)
        . = ALIGN(4);
        PROVIDE(_highcode_vma_end = .);
    } > instram AT > default_flash
    PROVIDE(_highcode_lma = LOADADDR(.fastcode));

    /* AXI SRAM. Only 64K on this part, so the EKF and the filter
       libraries are deliberately left XIP. */
    .ramfunc : ALIGN(4) SUBALIGN(4)
    {
        . = ALIGN(4);
        PROVIDE(_highcode_vma_start1 = .);
        lib/lib*.a:Semaphores.*(.text* .rodata*)
        lib/lib*.a:AP_Math.*(.text* .rodata*)
        lib/lib*.a:vector3.*(.text* .rodata*)
        lib/lib*.a:RingBuffer.*(.text* .rodata*)
        lib/lib*.a:crc.*(.text* .rodata*)
        lib/lib*.a:Util.*(.text* .rodata*)
        lib/lib*.a:Device.*(.text* .rodata*)
        lib/lib*.a:SPIDevice.*(.text* .rodata*)
        lib/lib*.a:I2CDevice.*(.text* .rodata*)
        lib/lib*.a:Scheduler.*(.text* .rodata*)
        *libm.a:*(.text*)
        *(.ramfunc*)            /* __RAMFUNC__ */
        . = ALIGN(4);
        PROVIDE(_highcode_vma_end1 = .);
    } > flashram AT > default_flash
    PROVIDE(_highcode_lma1 = LOADADDR(.ramfunc));

    /* burn the unusable first 512B so the DTCM users below start at +0x200 */
    .dtcm_reserved (NOLOAD) :
    {
        __dtcm_reserved_base__ = ORIGIN(dataram);
        . = . + __dtcm_reserved__;
        __dtcm_reserved_end__ = .;
    } > dataram

    .text : ALIGN(4) SUBALIGN(4)
    {
        . = ALIGN(4);
        KEEP(*(SORT_NONE(.handle_reset)))
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.glue_7)
        *(.glue_7t)
        *(.gnu.linkonce.t.*)
        *(.srodata.cst16)
        *(.srodata.cst8)
        *(.srodata.cst4)
        *(.srodata.cst2)
        . = ALIGN(4);
        __usb_debug_text_start = .;
        *USB_Debug.cpp*.o(.text .text.*)
        __usb_debug_text_end = .;
        . = ALIGN(4);
    } > default_flash

    .eh_frame_hdr :
    {
        *(.eh_frame_hdr)
    } > default_flash

    .eh_frame : ONLY_IF_RO
    {
        *(.eh_frame)
    } > default_flash

    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > default_flash

    .init_array :
    {
        . = ALIGN(4);
        __init_array_base__ = .;
        __init_array_start = .;
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP(*(SORT(.init_array)))
        KEEP(*(EXCLUDE_FILE(*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o) .ctors))
        . = ALIGN(4);
        __init_array_end = .;
    } > default_flash

    .fini_array :
    {
        . = ALIGN(4);
        __fini_array_base__ = .;
        __fini_array_start = .;
        KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
        KEEP(*(SORT(.fini_array)))
        KEEP(*(EXCLUDE_FILE(*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o) .dtors))
        . = ALIGN(4);
        __fini_array_end = .;
    } > default_flash

    . = ALIGN(4);
    PROVIDE(_etext = .);
    PROVIDE(_eitcm = .);

    /* firmware CRC window for the crash dump */
    __firmware_crc_start__ = LOADADDR(.fw_describer);
    __firmware_crc_ext_start__ = 0;
    __firmware_crc_ext_end__ = 0;

    /* Special section for process stack.*/
    .pstack (NOLOAD) :
    {
        __main_stack_base__ = .;
        __process_stack_base__ = .;
        __main_thread_stack_base__ = .;
        . += __process_stack_size__;
        . += __main_stack_size__;
        . = ALIGN(16);
        __process_stack_end__ = .;
        __main_thread_stack_end__ = .;
        __main_stack_end__ = .;
        PROVIDE(_eusrstack = .);
        __main_stack_top__ = .;
    } > PROCESS_STACK_RAM

    /* RAM data and the vector table are folded in here so that
       loaddata2vma() copies both into DTCM in one pass */
    .data : ALIGN(4)
    {
        PROVIDE(_data_vma = .);
        __data_base__ = .;
        . = ALIGN(1024);
        KEEP(*(.vector))
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.r.*)
        *(.gnu.linkonce.d.*)
        *(.ramtext)
        *(.ramdata*)
        . = ALIGN(8);
        PROVIDE(__global_pointer$ = . + 0x800);
        *(.sdata)
        *(.sdata.*)
        *(.sdata2.*)
        *(.gnu.linkonce.s.*)
        *(.srodata.cst16)
        *(.srodata.cst8)
        *(.srodata.cst4)
        *(.srodata.cst2)
        *(.srodata .srodata.*)
        . = ALIGN(4);
        PROVIDE(_edata = .);
        __data_end__ = .;
    } > DATA_RAM AT > default_flash
    PROVIDE(_data_lma = LOADADDR(.data));

    /* clearbss() zeroes _sbss.._ebss, so _sbss must be the first content */
    .bss (NOLOAD) : ALIGN(4)
    {
        . = ALIGN(4);
        __bss_base__ = .;
        PROVIDE(_sbss = .);
        *(.sbss)
        *(.sbss.*)
        *(.gnu.linkonce.sb.*)
        *(.bss)
        *(.bss.*)
        *(.gnu.linkonce.b.*)
        *(.scommon)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        PROVIDE(_ebss = .);
        PROVIDE(end = .);
    } > BSS_RAM

    /* DMA capable buffers in the shared SRAM. Nothing in the tree uses
       .ram0_init/.ram0/.ram0_clear today; __ram0_clear__..__ram0_noinit__
       is the only range __init_ram_areas() zeroes (board.c). */
    .ram0_init : ALIGN(4)
    {
        . = ALIGN(4);
        *(.ram0_init)
        *(.ram0_init.*)
        . = ALIGN(4);
    } > ram0 AT > default_flash

    __firmware_crc_end__ = LOADADDR(.ram0_init) + SIZEOF(.ram0_init);

    .ram0 (NOLOAD) : ALIGN(4)
    {
        . = ALIGN(4);
        __ram0_clear__ = .;
        *(.ram0_clear)
        *(.ram0_clear.*)
        . = ALIGN(4);
        __ram0_noinit__ = .;
        *(.ram0)
        *(.ram0.*)
        . = ALIGN(4);
    } > ram0

    /* The default heap uses the (statically) unused part of a RAM section.*/
    .heap (NOLOAD) : ALIGN(8)
    {
        . = ALIGN(8);
        __heap_base__ = .;
        . = ORIGIN(HEAP_RAM) + LENGTH(HEAP_RAM);
        __heap_end__ = .;
    } > HEAP_RAM

    /* The crash log uses the unused part of a flash section.*/
    .crash_log (NOLOAD) : ALIGN(32)
    {
        . = ALIGN(32);
        __crash_log_base__ = .;
        . = ORIGIN(default_flash) + LENGTH(default_flash);
        __crash_log_end__ = .;
    } > default_flash
}
