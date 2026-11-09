/*
 * ArduPilot / ChibiOS CH32H417 (QingKe V5F, RISC-V) linker rules.
 *
 * Shared by BOTH images; the generated ldscript.ld decides where
 * "default_flash" points through a REGION_ALIAS:
 *
 *   flight firmware : default_flash -> QSPI2 XIP window, 0x70000000
 *                     (the external NOR flash on this board, 4MB)
 *   bootloader      : default_flash -> "flash", the internal CodeFlash
 *                     V5F window at 0x00010000 (zero wait states)
 *
 * Memory layout (see CH32H417xx.py / ch32h417_memory.h for the numbers):
 *
 *   default_flash : .crt0/.vectors, .text, .rodata, initialisers (XIP)
 *   instram       : ITCM, 0x200A0000, 128K
 *                   -> hot code explicitly placed in .itcm* (copied at boot)
 *   flashram      : upper shared SRAM, 0x20120000, 384K
 *                   -> larger hot code in .sram*/.ramfunc* (copied at boot)
 *   ram0          : DTCM, 0x200C0200 (first 512B of DTCM are hardware owned)
 *                   -> .data/.sdata, .bss/.sbss, heap, all stacks
 *
 * The boot path is: internal-flash bootloader initialises QSPI2 and enables
 * its memory mapped mode, then jumps to APP_START_ADDRESS (0x70000000) in
 * the flight image.  This way a single set of section rules serves both
 * images.
 *
 * The ChibiOS adapt-417 crt0.S/crt1.c do all runtime relocation:
 *   - crt0 copies .data and clears .bss, using __textdata_base__,
 *     __data_base__/__data_end__, __bss_base__/__bss_end__ below.
 *   - crt1 __init_ram_areas() copies/zeroes the three areas 0..2 using
 *     __ramN_init_text__, __ramN_init__, __ramN_clear__, __ramN_noinit__.
 *     Area 0 = DTCM, area 1 = ITCM, area 2 = shared SRAM.
 *
 * IMPORTANT: build with -DCRT0_INIT_TEXT=0.  adapt-417 defaults it to TRUE
 * so a standalone demo can copy its whole .text into ITCM; here the whole
 * image is XIP in QSPI flash and only the opted-in sections are relocated.
 *
 * This file is copied to build/common.ld and INCLUDEd by the hwdef-generated
 * ldscript.ld, which defines the MEMORY regions and, for the bootloader,
 * REGION_ALIAS("default_flash", flash) before the INCLUDE.
 *
 * Required linker --defsym (supplied by boards.py LINKFLAGS):
 *   __process_stack_size__, __main_stack_size__
 */

/* Every variable, the heap and both stacks live in DTCM. */
REGION_ALIAS("MAIN_STACK_RAM", ram0);
REGION_ALIAS("PROCESS_STACK_RAM", ram0);
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("BSS_RAM", ram0);
REGION_ALIAS("HEAP_RAM", ram0);

__ram0_start__  = ORIGIN(ram0);
__ram0_size__   = LENGTH(ram0);
__ram0_end__    = __ram0_start__ + __ram0_size__;
__itcm_start__  = ORIGIN(instram);
__itcm_size__   = LENGTH(instram);
__itcm_end__    = __itcm_start__ + __itcm_size__;
__sram_start__  = ORIGIN(flashram);
__sram_size__   = LENGTH(flashram);
__sram_end__    = __sram_start__ + __sram_size__;
__flash_start__ = ORIGIN(default_flash);
__flash_size__  = LENGTH(default_flash);
__flash_end__   = __flash_start__ + __flash_size__;

ENTRY(_start)

SECTIONS
{
    /* Reset entry and the ECLIC vector table.  _start must be the very
       first byte of the external flash image: the bootloader jumps to
       ORIGIN(default_flash).  The 512 byte alignment is required by the
       QingKe V5F ECLIC for tables with more than 64 interrupt sources. */
    .vectors : ALIGN(4) SUBALIGN(4)
    {
        __vectors_base__ = .;
        KEEP(*(SORT_NONE(.crt0)))
        . = ALIGN(512);
        KEEP(*(SORT_NONE(.vectors)))
        __vectors_end__ = .;
    } > default_flash
    __firmware_crc_start__ = LOADADDR(.vectors);
    __firmware_crc_ext_start__ = 0;
    __firmware_crc_ext_end__ = 0;

    constructors : ALIGN(4) SUBALIGN(4)
    {
        __init_array_base__ = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end__ = .;
    } > default_flash

    destructors : ALIGN(4) SUBALIGN(4)
    {
        __fini_array_base__ = .;
        KEEP(*(.fini_array))
        KEEP(*(SORT(.fini_array.*)))
        __fini_array_end__ = .;
    } > default_flash

    /* Main body of the firmware: XIP from the external QSPI flash.
       app_descriptor stays near the start so the bootloader finds it
       before any embedded ROMFS can produce a false positive. */
    .text : ALIGN(4) SUBALIGN(4)
    {
        __text_base__ = .;
        KEEP(*(.app_descriptor))
        KEEP(*(.apsec_data))
        PROVIDE(__usb_debug_text_start = .);
        *USB_Debug.cpp*.o(.text .text.*)
        PROVIDE(__usb_debug_text_end = .);
        *(.text)
        *(.text.*)
        *(.gcc*)
        __text_end__ = .;
    } > default_flash
    __text_lma = LOADADDR(.text);

    .rodata : ALIGN(4)
    {
        __rodata_base__ = .;
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)
        . = ALIGN(4);
        __rodata_end__ = .;
    } > default_flash

    .eh_frame_hdr : { *(.eh_frame_hdr) } > default_flash
    .eh_frame : ONLY_IF_RO { *(.eh_frame) } > default_flash
    .gcc_except_table : { *(.gcc_except_table .gcc_except_table.*) } > default_flash
    .textalign : ONLY_IF_RO { . = ALIGN(8); } > default_flash

    PROVIDE(_etext = .);

    /* ---------------- CRT1 area 1: ITCM hot code ----------------
       Only explicitly tagged functions are relocated: use __ITCMFUNC__ /
       __FASTRAMFUNC__ (or section(".itcm")) in C/C++ sources.  This keeps
       crt0/crt1 and the rest of the startup path safely in flash, and
       avoids a wildcard .text* rule competing with the XIP .text below. */
    .itcm_init : ALIGN(4) SUBALIGN(4)
    {
        __ram1_init_text__ = LOADADDR(.itcm_init);
        __ram1_init__ = .;
        KEEP(*(.itcm))
        KEEP(*(.itcm.*))
        KEEP(*(.fastramfunc))
        KEEP(*(.fastramfunc.*))
        . = ALIGN(4);
    } > instram AT > default_flash

    .ram1 (NOLOAD) : ALIGN(4)
    {
        __ram1_clear__ = .;
        *(.ram1_clear)
        *(.ram1_clear.*)
        . = ALIGN(4);
        __ram1_noinit__ = .;
        *(.ram1)
        *(.ram1.*)
        . = ALIGN(4);
        __ram1_free__ = .;
    } > instram

    /* ---------------- CRT1 area 2: shared SRAM hot code ----------------
       Larger but still performance sensitive code (drivers, filters,
       EKF) is copied into the upper 384K of shared SRAM.  Tag functions
       with __SRAMFUNC__ / __RAMFUNC__ (or section(".sram")). */
    .sram_init : ALIGN(4) SUBALIGN(4)
    {
        __ram2_init_text__ = LOADADDR(.sram_init);
        __ram2_init__ = .;
        KEEP(*(.sram))
        KEEP(*(.sram.*))
        KEEP(*(.ramfunc))
        KEEP(*(.ramfunc.*))
        . = ALIGN(4);
    } > flashram AT > default_flash

    .ram2 (NOLOAD) : ALIGN(4)
    {
        __ram2_clear__ = .;
        *(.ram2_clear)
        *(.ram2_clear.*)
        . = ALIGN(4);
        __ram2_noinit__ = .;
        *(.ram2)
        *(.ram2.*)
        . = ALIGN(4);
        __ram2_free__ = .;
    } > flashram

    /* ---------------- CRT1 area 0: DTCM explicitly initialised data ----
       Application objects may be placed in .ram0_init to have their
       initial value copied from flash into DTCM by crt1. */
    .ram0_init : ALIGN(4)
    {
        __ram0_init_text__ = LOADADDR(.ram0_init);
        __ram0_init__ = .;
        KEEP(*(.ram0_init))
        KEEP(*(.ram0_init.*))
        . = ALIGN(4);
    } > ram0 AT > default_flash
    __firmware_crc_end__ = LOADADDR(.ram0_init) + SIZEOF(.ram0_init);

    .ram0 (NOLOAD) : ALIGN(4)
    {
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

    /* Exception/main stack.  The QingKe V5F uses software stacking, so the
       main stack is a normal RAM reservation in DTCM. */
    .mstack (NOLOAD) : ALIGN(16)
    {
        __main_stack_base__ = .;
        . += __main_stack_size__;
        . = ALIGN(16);
        __main_stack_end__ = .;
    } > MAIN_STACK_RAM

    /* Process stack, also used by crt0 as the initial sp. */
    .pstack (NOLOAD) : ALIGN(16)
    {
        __process_stack_base__ = .;
        __main_thread_stack_base__ = .;
        . += __process_stack_size__;
        . = ALIGN(16);
        __process_stack_end__ = .;
        __main_thread_stack_end__ = .;
    } > PROCESS_STACK_RAM

    /* All mutable variables (data/sdata/bss/sbss) in DTCM. */
    .data : ALIGN(4)
    {
        PROVIDE(_textdata = LOADADDR(.data));
        PROVIDE(_data = .);
        __textdata_base__ = LOADADDR(.data);
        __data_base__ = .;
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
        . = ALIGN(8);
        PROVIDE(__global_pointer$ = . + 0x800);
        *(.sdata)
        *(.sdata.*)
        *(.ramtext)
        *(.gnu.linkonce.s.*)
        . = ALIGN(4);
        PROVIDE(_edata = .);
        __data_end__ = .;
    } > DATA_RAM AT > default_flash

    .bss (NOLOAD) : ALIGN(4)
    {
        __bss_base__ = .;
        *(.sbss)
        *(.sbss.*)
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        PROVIDE(end = .);
    } > BSS_RAM

    .heap (NOLOAD) : ALIGN(8)
    {
        __heap_base__ = .;
        . = ORIGIN(HEAP_RAM) + LENGTH(HEAP_RAM);
        __heap_end__ = .;
    } > HEAP_RAM

    /* Crash log occupies the unused tail of the external flash. */
    .crash_log (NOLOAD) : ALIGN(32)
    {
        __crash_log_base__ = .;
        . = ORIGIN(default_flash) + LENGTH(default_flash);
        __crash_log_end__ = .;
    } > default_flash
}
