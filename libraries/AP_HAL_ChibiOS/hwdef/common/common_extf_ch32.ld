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

    CH32H417 variant of the external-flash application layout:
    the whole application (text/rodata/const/loads) lives in the
    QSPI flash memory-mapped window and executes in place (XIP).
    ITCM (instram) holds hot code, DTCM (dataram) holds hot data,
    part of the AXI SRAM (ram1/flashram) holds RAM-copied functions,
    the rest of the AXI SRAM (ram0) holds data/stacks/heap.

    The symbols _loadcode_*, _highcode_*, _data_lma/_data_vma,
    _sbss/_ebss, __process_stack_base__/_end follow the WCH
    RISCV-QINGKE crt0.S / crt1.c conventions used by the CH32 port,
    so the stock V5F startup files can be used unchanged. The .load
    copy is unused (VTF zero-wait flash trick is not needed when
    running from QSPI) and is given an empty range.
*/

/* RAM region to be used for fast code. */
REGION_ALIAS("FASTCODE_RAM", flashram)

/* RAM region to be used for Main stack. This stack accommodates the processing
   of all exceptions and interrupts*/
REGION_ALIAS("MAIN_STACK_RAM", ram0);

/* RAM region to be used for the process stack. This is the stack used by
   the main() function.*/
REGION_ALIAS("PROCESS_STACK_RAM", ram0);

/* RAM region to be used for data segment.*/
REGION_ALIAS("DATA_RAM", ram0);

/* RAM region to be used for BSS segment.*/
REGION_ALIAS("BSS_RAM", ram0);

/* RAM region to be used for the default heap.*/
REGION_ALIAS("HEAP_RAM", ram0);

/* The ChibiOS crt code looks at all of the __ramx_xxxx__ sections
   it will copy from __ramx_init_text__ to __ramx_init__ until __ramx_clear__ is hit
   it will then clear from __ramx_clear__ to __ramx_no_init__
   the number of areas actually used is determined by CRT0_AREAS_NUMBER
 */

/* AXI SRAM */
__ram0_start__          = ORIGIN(ram0);
__ram0_size__           = LENGTH(ram0);
__ram0_end__            = __ram0_start__ + __ram0_size__;

/* AXI for RAM functions */
__ram1_start__          = ORIGIN(flashram);
__ram1_size__           = LENGTH(flashram);
__ram1_end__            = __ram1_start__ + __ram1_size__;

/* DTCM */
__ram2_start__          = ORIGIN(dataram);
__ram2_size__           = LENGTH(dataram);
__ram2_end__            = __ram2_start__ + __ram2_size__;

/* ITCM for RAM functions */
__ram3_start__          = ORIGIN(instram);
__ram3_size__           = LENGTH(instram);
__ram3_end__            = __ram3_start__ + __ram3_size__;

ENTRY( _start )

SECTIONS
{
    . = 0;
    _text = .;

    /* The .start and .handle_reset sections come from the WCH V5F
       crt0.S; they execute in place from the QSPI window */
    .start : ALIGN(4) SUBALIGN(4)
    {
        __textvectors_base__ = LOADADDR(.start);
        __vectors_base__ = .;
        KEEP(*(.start))
        __vectors_end__ = .;
    } > default_flash
    __firmware_crc_start__ = LOADADDR(.start);
    __firmware_crc_ext_start__ = 0;
    __firmware_crc_ext_end__ = 0;

    constructors : ALIGN(4) SUBALIGN(4)
    {
        __init_array_base__ = .;
        /* WCH crt1.c expects GCC-provided start/end names */
        __init_array_start__ = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end__ = .;
        __init_array_end = .;
        __init_array_start = .;
    } > default_flash

    destructors : ALIGN(4) SUBALIGN(4)
    {
        __fini_array_base__ = .;
        __fini_array_start = .;
        KEEP(*(.fini_array))
        KEEP(*(SORT(.fini_array.*)))
        __fini_array_end__ = .;
        __fini_array_end = .;
    } > default_flash

    /* INSTRUCTION_RAM area is ITCM, used for hot code */
    .fastramfunc : ALIGN(4) SUBALIGN(4)
    {
        . = ALIGN(4);
        __ram3_init_text__ = LOADADDR(.fastramfunc);
        __ram3_init__ = .;
        /* performance critical sections of ChibiOS */
        EXCLUDE_FILE (*vectors.o *crt0.o *crt1.o  *chprintf.o) *libch.a:ch*.*(.text*)
        *libch.a:nvic.*(.text*)
        *libch.a:bouncebuffer.*(.text*)
        *libch.a:stm32_util.*(.text*)
        *libch.a:ch32_dma.*(.text*)
        *libch.a:memstreams.*(.text*)
        *libch.a:malloc.*(.text*)
        *libch.a:hrt.*(.text*)
        EXCLUDE_FILE (*hal_lld.o) *libch.a:hal*.*(.text*)
        /* a selection of performance critical functions driven CPUInfo results */
        lib/lib*.a:Semaphores.*(.text*)
        lib/lib*.a:AP_Math.*(.text*)
        lib/lib*.a:vector3.*(.text*)
        lib/lib*.a:matrix3.*(.text*)
        /* the attitude controller runs at loop rate and needs to be optimized */
        lib/lib*.a:AC_AttitudeControl*.*(.text*)
        lib/lib*.a:AC_PID*.*(.text*)
        *libm.a:*(.text*)
        *(.fastramfunc)
        . = ALIGN(4);
    } > instram AT > default_flash

    .ram3 (NOLOAD) : ALIGN(4)
    {
        . = ALIGN(4);
        __ram3_clear__ = .;
        . = ALIGN(4);
        __ram3_noinit__ = .;
        . = ALIGN(4);
        __ram3_free__ = .;
        . = ALIGN(4);
    } > instram

    /* FLASH_RAM area is an AXI SRAM bank used for large RAM-copied functions */
    .ramfunc : ALIGN(4) SUBALIGN(4)
    {
        . = ALIGN(4);
        __ram1_init_text__ = LOADADDR(.ramfunc);
        __ram1_init__ = .;
        /* a selection of larger performance critical functions */
        lib/lib*.a:LowPassFilter2p.*(.text* .rodata*)
        lib/lib*.a:LowPassFilter.*(.text* .rodata*)
        lib/lib*.a:NotchFilter.*(.text* .rodata*)
        lib/lib*.a:HarmonicNotchFilter.*(.text* .rodata*)
        lib/lib*.a:Util.*(.text* .rodata*)
        lib/lib*.a:Device.*(.text* .rodata*)
        lib/lib*.a:Scheduler.*(.text* .rodata*)
        lib/lib*.a:RingBuffer.*(.text* .rodata*)
        lib/lib*.a:shared_dma.*(.text* .rodata*)
        lib/lib*.a:crc.*(.text* .rodata*)
        lib/lib*.a:matrixN.*(.text* .rodata*)
        lib/lib*.a:matrix_alg.*(.text* .rodata*)
        lib/lib*.a:vector2.*(.text* .rodata*)
        lib/lib*.a:quaternion.*(.text* .rodata*)
        lib/lib*.a:polygon.*(.text* .rodata*)
        *(.ramfunc*)
        . = ALIGN(4);
    } > flashram AT > default_flash

    .ram1 (NOLOAD) : ALIGN(4)
    {
        . = ALIGN(4);
        __ram1_clear__ = .;
        . = ALIGN(4);
        __ram1_noinit__ = .;
        . = ALIGN(4);
        __ram1_free__ = .;
        . = ALIGN(4);
    } > flashram

    /* DATA_RAM area is DTCM primarily used for RAM-based data, e.g. vtables */
    .ramdata : ALIGN(4)
    {
        . = ALIGN(4);
        __ram2_init_text__ = LOADADDR(.ramdata);
        __ram2_init__ = .;
        /* hot rodata of ChibiOS */
        EXCLUDE_FILE (*vectors.o *crt0.o *crt1.o) *libch.a:ch*.*(.rodata*)
        *libch.a:ch*.*(.rodata*)
        *libch.a:nvic.*(.rodata*)
        *libch.a:bouncebuffer.*(.rodata*)
        *libch.a:stm32_util.*(.rodata*)
        *libch.a:ch32_dma.*(.rodata*)
        *libch.a:memstreams.*(.rodata*)
        *libch.a:malloc.*(.rodata*)
        *libch.a:hrt.*(.rodata*)
        EXCLUDE_FILE (*hal_lld.o) *libch.a:hal*.*(.rodata*)
        /* a selection of performance critical functions driven CPUInfo results */
        lib/lib*.a:Semaphores.*(.rodata*)
        lib/lib*.a:AP_Math.*(.rodata*)
        lib/lib*.a:vector3.*(.rodata*)
        lib/lib*.a:matrix3.*(.rodata*)
        *libm.a:*(.rodata*)
        lib/lib*.a:AC_AttitudeControl*.*(.rodata*)
        lib/lib*.a:AC_PID*.*(.rodata*)
        /* fill up remaining DTCM with other hot functions, esp EKF */
        lib/lib*.a:ModeFilter.*(.text* .rodata*)
        lib/lib*.a:DerivativeFilter.*(.text* .rodata*)
        lib/lib*.a:SlewLimiter.*(.text* .rodata*)
        lib/lib*.a:AP_NavEKF3_core.*(.text* .rodata*)
        lib/lib*.a:AP_NavEKF3_Outputs.*(.text* .rodata*)
        lib/lib*.a:AP_NavEKF3_Measurements.*(.text* .rodata*)
        lib/lib*.a:EKF*.*(.text* .rodata*)
        /* flash ops in RAM so that both banks can be erased */
        lib/lib*.a:flash.*(.text* .rodata*)
        *(.ramdata*)
        . = ALIGN(4);
    } > dataram AT > default_flash

    .ram2 (NOLOAD) : ALIGN(4)
    {
        . = ALIGN(4);
        __ram2_clear__ = .;
        . = ALIGN(4);
        __ram2_noinit__ = .;
        . = ALIGN(4);
        __ram2_free__ = .;
        . = ALIGN(4);
    } > dataram

    .text : ALIGN(4) SUBALIGN(4)
    {
        /* we want app_descriptor near the start of flash so a false
           positive isn't found by the bootloader (eg. ROMFS) */
        KEEP(*libch.a:vectors.o);
        KEEP(*libch.a:crt0.o);
        KEEP(*libch.a:crt1.o);
        KEEP(*libch.a:board.o);
        KEEP(*(.app_descriptor));
        PROVIDE(__usb_debug_text_start = .);
        *USB_Debug.cpp*.o(.text .text.*)
        PROVIDE(__usb_debug_text_end = .);
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.glue_7t)
        *(.glue_7)
        *(.gcc*)
    } > default_flash

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > default_flash

    .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
     } > default_flash

    .eh_frame_hdr :
    {
        *(.eh_frame_hdr)
    } > default_flash

    .eh_frame : ONLY_IF_RO
    {
        *(.eh_frame)
    } > default_flash

    .textalign : ONLY_IF_RO
    {
        . = ALIGN(8);
    } > default_flash

    /* Legacy symbol, not used anywhere.*/
    . = ALIGN(4);
    PROVIDE(_etext = .);

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
        PROVIDE(_textdata = LOADADDR(.data));
        PROVIDE(_data = .);
        __textdata_base__ = LOADADDR(.data);
        __data_base__ = .;
        *(.data)
        *(.data.*)
        *(.ramtext)
        /* RISC-V small-data section handling */
        *(.sdata)
        *(.sdata.*)
        . = ALIGN(8);
        PROVIDE(__global_pointer$ = . + 0x800);
        *(.srodata.cst16)
        *(.srodata.cst8)
        *(.srodata.cst4)
        *(.srodata.cst2)
        *(.srodata .srodata.*)
        /* WCH crt1.c conventions */
        PROVIDE(_data_vma = .);
        PROVIDE(_data_lma = LOADADDR(.data));
        . = ALIGN(4);
        PROVIDE(_edata = .);
        __data_end__ = .;
    } > DATA_RAM AT > default_flash

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
        /* WCH crt1.c conventions */
        PROVIDE(_sbss = .);
        . = ALIGN(4);
        __bss_end__ = .;
        PROVIDE(_ebss = .);
        PROVIDE(end = .);
    } > BSS_RAM

    /* WCH crt0.S .load handling: not used when running from the QSPI
       window, but the symbols must exist as an empty range */
    .loadcodelalign :
    {
        . = ALIGN(4);
        PROVIDE(_loadcode_lma = .);
    } > default_flash

    .loadcode :
    {
        . = ALIGN(4);
        PROVIDE(_loadcode_vma_start = .);
        *(.load)
        . = ALIGN(4);
        PROVIDE(_loadcode_vma_end = .);
    } > default_flash

    /* WCH crt1.c .highcode handling: no RAM copy needed under XIP,
       empty ranges make the copy loops no-ops */
    .highcodelalign :
    {
        . = ALIGN(4);
        PROVIDE(_highcode_lma = .);
    } > default_flash

    .highcode :
    {
        . = ALIGN(4);
        PROVIDE(_highcode_vma_start = .);
        . = ALIGN(4);
        PROVIDE(_highcode_vma_end = .);
    } > default_flash

    .highcodelalign1 :
    {
        . = ALIGN(4);
        PROVIDE(_highcode_lma1 = .);
    } > default_flash

    .highcode1 :
    {
        . = ALIGN(4);
        PROVIDE(_highcode_vma_start1 = .);
        . = ALIGN(4);
        PROVIDE(_highcode_vma_end1 = .);
    } > default_flash

    .ram0_init : ALIGN(4)
    {
        . = ALIGN(4);
        __ram0_init_text__ = LOADADDR(.ram0_init);
        __ram0_init__ = .;
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
        __ram0_free__ = .;
    } > ram0

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
        . = ORIGIN(default_flash) + LENGTH(default_flash);
        __crash_log_end__ = .;
    } > default_flash
}
