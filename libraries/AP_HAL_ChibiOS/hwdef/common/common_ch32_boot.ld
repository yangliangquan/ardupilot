/*
 * ArduPilot CH32H417 (QingKe V5F) BOOTLOADER linker rules.
 *
 * Simplified sibling of common_ch32.ld (which the flight image keeps):
 * the bootloader has no hot-path relocation lists, no XIP budget games
 * and no crash log.  The whole text segment runs from ITCM and the
 * whole data segment runs from DTCM:
 *
 *   default_flash ("flash", generated ldscript.ld): 0x00001000, 60K
 *       -> .crt0 only, plus the load images (AT >) of .text/.data
 *   instram  (ITCM, 0x200A0000, 128K)
 *       -> VMA of the whole .text output section: ECLIC vectors,
 *          init/fini arrays, code, rodata, exception tables
 *   ram0     (DTCM, 0x200C0200; first 512B of DTCM are hardware owned)
 *       -> .data/.sdata, .bss/.sbss, both stacks; no heap
 *   flashram (shared SRAM, 0x20100000, 512K)
 *       -> the default heap (HEAP_RAM alias, RAM_MAP_BOOTLOADER[0]);
 *          the crt1 area 2 collector normally lands here empty
 *
 * The image boots XIP at ORIGIN(default_flash): the V3F stub releases
 * the V5F core there, crt0 (.crt0 section) then copies the .text image
 * from its flash LMA into ITCM (the always-compiled crt0.S copy loop
 * triggers on VMA != LMA) and continues execution from ITCM.  Running from ITCM
 * also keeps the bootloader off the CodeFlash bus while it programs
 * that same flash.
 *
 * crt0/crt1 symbols consumed by the startup code:
 *   crt0.S: __text_lma/__text_base__/__text_end__ (text copy),
 *           __textdata_base__/__data_base__/__data_end__ (data copy),
 *           __bss_base__/__bss_end__ (bss clear),
 *           __init_array_base__/__init_array_end__,
 *           __fini_array_base__/__fini_array_end__,
 *           __process_stack_base__/__end__, __irq_stack_base__/__end__
 *   crt1.c (CRT1_AREAS_NUMBER=3): __ramN_init_text__/__ramN_init__/
 *           __ramN_clear__/__ramN_noinit__ for areas 0 (DTCM),
 *           1 (ITCM) and 2 (shared SRAM)
 *
 * Required linker --defsym (supplied by boards.py LINKFLAGS):
 *   __process_stack_size__, __main_stack_size__
 */

/* Variables and stacks live in DTCM; the heap lives in the shared SRAM
   (flashram), matching RAM_MAP_BOOTLOADER[0] for malloc.c. */
REGION_ALIAS("MAIN_STACK_RAM", ram0);
REGION_ALIAS("PROCESS_STACK_RAM", ram0);
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("BSS_RAM", ram0);
REGION_ALIAS("HEAP_RAM", flashram);

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
    /* Reset code: the only part executed from CodeFlash.  _start must be
       the very first byte of the image; the V3F stub releases V5F here. */
    .crt0 : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(SORT_NONE(.crt0)))
    } > default_flash
    __firmware_crc_start__ = LOADADDR(.crt0);
    __firmware_crc_ext_start__ = 0;
    __firmware_crc_ext_end__ = 0;

    /* The whole text segment: VMA = ITCM, load image in CodeFlash,
       copied by crt0 before the first C call.  Members:
       - .vectors first: mtvec needs the 1024-byte alignment that
         ORIGIN(instram) (0x200A0000) already provides; interrupts stay
         masked (MIE=0) until after the copy.
       - init/fini arrays: read by crt0 after the copy.
       - .rodata/.srodata are inside the copied image, so the bootloader
         reads its constants from ITCM too (nothing fetches from the
         flash bus while it is being programmed).
       Halfword-granular copy in crt0 tolerates any internal alignment;
       everything is 4-aligned anyway. */
    .text : ALIGN(4) SUBALIGN(4)
    {
        __text_base__ = .;
        __vectors_base__ = .;
        KEEP(*(SORT_NONE(.vectors)))
        __vectors_end__ = .;
        /* .vectors has no 4-byte multiple size; SUBALIGN(4) aligns the
           array input sections but not the base symbols, so align the
           location counter before taking each base.  Without this
           crt0's initloop lw faults misaligned on __init_array_base__. */
        . = ALIGN(4);
        __init_array_base__ = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end__ = .;
        . = ALIGN(4);
        __fini_array_base__ = .;
        KEEP(*(.fini_array))
        KEEP(*(SORT(.fini_array.*)))
        __fini_array_end__ = .;
        *(.text)
        *(.text.*)
        *(.gcc*)
        /* __FASTRAMFUNC__ / CH32H417_ITCM_CODE: IRQ entry etc.  They
           must stay INSIDE __text_base__..__text_end__ or crt0 would
           leave them uncopied in ITCM. */
        KEEP(*(.itcm))
        KEEP(*(.itcm.*))
        KEEP(*(.fastramfunc))
        KEEP(*(.fastramfunc.*))
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.gcc_except_table .gcc_except_table.*)
        . = ALIGN(4);
        __text_end__ = .;
    } > instram AT > default_flash
    __text_lma = LOADADDR(.text);
    PROVIDE(_etext = .);

    /* ---------------- crt1 area 1: ITCM (after the .text image) ------ */
    .ram1 (NOLOAD) : ALIGN(4)
    {
        __ram1_init_text__ = __text_lma + SIZEOF(.text);
        __ram1_init__ = .;
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

    /* ---------------- crt1 area 2: shared SRAM (normally empty) ------ */
    .ram2 (NOLOAD) : ALIGN(4)
    {
        __ram2_init_text__ = LOADADDR(.text);
        __ram2_init__ = .;
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

    /* ---------------- crt1 area 0: DTCM explicitly initialised data -- */
    .ram0_init : ALIGN(4)
    {
        __ram0_init_text__ = LOADADDR(.ram0_init);
        __ram0_init__ = .;
        KEEP(*(.ram0_init))
        KEEP(*(.ram0_init.*))
        . = ALIGN(4);
        __ram0_clear__ = .;
    } > ram0 AT > default_flash

    .ram0 (NOLOAD) : ALIGN(4)
    {
        *(.ram0_clear)
        *(.ram0_clear.*)
        . = ALIGN(4);
        __ram0_noinit__ = .;
        *(.ram0)
        *(.ram0.*)
        . = ALIGN(4);
        __ram0_free__ = .;
    } > ram0

    /* Independent interrupt stack (see PORT_RISCV_USE_IRQ_STACK). */
    .istack (NOLOAD) :
    {
        . = ALIGN(16);
        __irq_stack_base__ = .;
        __main_stack_base__ = .;
        . += __main_stack_size__;
        . = ALIGN(16);
        __irq_stack_end__ = .;
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

    /* Initialised data: VMA DTCM, initialiser image in CodeFlash,
       copied by crt0. */
    .data : ALIGN(4)
    {
        __textdata_base__ = LOADADDR(.data);
        __data_base__ = .;
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
        . = ALIGN(8);
        PROVIDE(__global_pointer$ = . + 0x800);
        *(.sdata)
        *(.sdata.*)
        *(.gnu.linkonce.s.*)
        . = ALIGN(4);
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

    /* End of the CodeFlash load image (the last AT > section). */
    __firmware_crc_end__ = LOADADDR(.data) + SIZEOF(.data);
    ASSERT(__firmware_crc_end__ <= __flash_end__, "ERROR: CH32H417 bootloader image overflows the flash window")
    ASSERT(__text_lma + SIZEOF(.text) <= __flash_end__, "ERROR: CH32H417 bootloader .text image overflows the flash window")
    ASSERT(__text_base__ + SIZEOF(.text) <= __itcm_end__, "ERROR: CH32H417 bootloader .text overflows ITCM")
}
