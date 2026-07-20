/*
    CH32H417 linker script for ArduPilot CCH-v6c board (XIP from external flash).
    Replaces hwdef/common/common_extf.ld -- do not use the common version.

    CH32H417 memory map:
      Internal Flash : 0x00000000, 512K (4K pages)
      ITCM            : 0x200A0000, 128K (INSTRUCTION_RAM, fast code execution)
      DTCM            : 0x200C0000, 256K (DATA_RAM, fast data, tightly coupled)
      SRAM2/XIP       : 0x20100000, 512K (FLASH_RAM, DMA-capable, flash operation RAM)
      External Flash  : 0x90000000,  2M  (XIP via QUADSPI)
*/

/* Use external flash as the default code flash source. */
REGION_ALIAS("DEFAULT_FLASH", default_flash)

/* RAM region to be used for fast code (ITCM). */
REGION_ALIAS("FASTCODE_RAM", instram)

/* Stack areas in SRAM2 (flash operation RAM) for compatibility with XIP boot. */
REGION_ALIAS("MAIN_STACK_RAM", flashram)
REGION_ALIAS("PROCESS_STACK_RAM", flashram)

/* Data/BSS/heap in SRAM1 (primary RAM for external flash mode). */
REGION_ALIAS("DATA_RAM", ram0)
REGION_ALIAS("BSS_RAM", ram0)
REGION_ALIAS("HEAP_RAM", ram0)

/* ChibiOS crt RAM area descriptors. */
__ram0_start__          = ORIGIN(ram0);
__ram0_size__           = LENGTH(ram0);
__ram0_end__            = __ram0_start__ + __ram0_size__;

__ram1_start__          = ORIGIN(flashram);
__ram1_size__           = LENGTH(flashram);
__ram1_end__            = __ram1_start__ + __ram1_size__;

__ram2_start__          = ORIGIN(dataram);
__ram2_size__           = LENGTH(dataram);
__ram2_end__            = __ram2_start__ + __ram2_size__;

__ram3_start__          = ORIGIN(instram);
__ram3_size__           = LENGTH(instram);
__ram3_end__            = __ram3_start__ + __ram3_size__;

ENTRY(_start)

SECTIONS
{
    . = 0;
    _text = .;

    /* Vector table: copied to DTCM by crt0 startup code */
    vectors : ALIGN(1024) SUBALIGN(16)
    {
        __textvectors_base__ = LOADADDR(vectors);
        __vectors_base__ = .;
        PROVIDE(vector_start = .);
        KEEP(*(.vectors))
        __vectors_end__ = .;
    } > dataram AT > default_flash

    constructors : ALIGN(4) SUBALIGN(4)
    {
        __init_array_base__ = .;
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end__ = .;
        __init_array_end = .;
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

    /* V3F crt0-required sections: loadcode (startup code copied to DTCM) */
    .loadcodelalign :
    {
        . = ALIGN(4);
        PROVIDE(_loadcode_lma = .);
    } > default_flash AT > default_flash

    .loadcode :
    {
        . = ALIGN(4);
        PROVIDE(_loadcode_vma_start = .);
        *(.load);
        KEEP(*(SORT_NONE(.handle_reset)))
        . = ALIGN(4);
        PROVIDE(_loadcode_vma_end = .);
    } > dataram AT > default_flash

    /* V3F crt0-required sections: highcode (main code, kept in flash for XIP) */
    .highcodelalign :
    {
        . = ALIGN(4);
        PROVIDE(_highcode_lma = .);
    } > default_flash AT > default_flash

    .highcode :
    {
        . = ALIGN(4);
        PROVIDE(_highcode_vma_start = .);
    } > default_flash AT > default_flash

    .highcodelalign1 :
    {
        . = ALIGN(4);
        PROVIDE(_highcode_lma1 = .);
    } > default_flash AT > default_flash

    .fini_v3f :
    {
        . = ALIGN(4);
        PROVIDE(_highcode_vma_start1 = .);
        . = ALIGN(4);
        PROVIDE(_highcode_vma_end1 = .);
    } > default_flash AT > default_flash

    PROVIDE(_highcode_vma_end = _etext);

    /* ITCM area: performance-critical ChibiOS and library code */
    .fastramfunc : ALIGN(4) SUBALIGN(4)
    {
        . = ALIGN(4);
        __ram3_init_text__ = LOADADDR(.fastramfunc);
        __ram3_init__ = .;
        EXCLUDE_FILE (*vectors.o *crt0_v7m.o *crt1.o *board.o *chprintf.o)
        *libch.a:ch*.*(.text*)
        *libch.a:nvic.*(.text*)
        *libch.a:bouncebuffer.*(.text*)
        *libch.a:stm32_util.*(.text*)
        *libch.a:stm32_dma.*(.text*)
        *libch.a:memstreams.*(.text*)
        *libch.a:malloc.*(.text*)
        *libch.a:hrt.*(.text*)
        *libch.a:hal*.*(.text*)
        lib/lib*.a:Semaphores.*(.text*)
        lib/lib*.a:AP_Math.*(.text*)
        lib/lib*.a:vector3.*(.text*)
        lib/lib*.a:matrix3.*(.text*)
        *libg_nano.a:*memset*(.text*)
        *libg_nano.a:*memcpy*(.text*)
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

    /* SRAM2 area for RAM-based code (flash operations, filter code, etc.) */
    .ramfunc : ALIGN(4) SUBALIGN(4)
    {
        . = ALIGN(4);
        __ram1_init_text__ = LOADADDR(.ramfunc);
        __ram1_init__ = .;
        EXCLUDE_FILE (*vectors.o *crt0_v7m.o *crt1.o *board.o)
        lib/lib*.a:*Filter.*(.text* .rodata*)
        lib/lib*.a:*Filter2p.*(.text* .rodata*)
        lib/lib*.a:SPIDevice.*(.text* .rodata*)
        lib/lib*.a:I2CDevice.*(.text* .rodata*)
        lib/lib*.a:Util.*(.text* .rodata*)
        lib/lib*.a:Device.*(.text* .rodata*)
        lib/lib*.a:Scheduler.*(.text* .rodata*)
        lib/lib*.a:shared_dma.*(.text* .rodata*)
        lib/lib*.a:RingBuffer.*(.text* .rodata*)
        lib/lib*.a:crc.*(.text* .rodata*)
        lib/lib*.a:matrixN.*(.text* .rodata*)
        lib/lib*.a:matrix_alg.*(.text* .rodata*)
        lib/lib*.a:AP_NavEKF3*.*(.text* .rodata*)
        lib/lib*.a:AP_NavEKF_*.*(.text* .rodata*)
        lib/lib*.a:EKF*.*(.text* .rodata*)
        lib/lib*.a:AP_Compass_Backend.*(.text* .rodata*)
        lib/lib*.a:AP_RCProtocol_Backend.*(.text* .rodata*)
        lib/lib*.a:AP_RCProtocol.*(.text* .rodata*)
        lib/lib*.a:AP_RCProtocol_CRSF.*(.text* .rodata*)
        lib/lib*.a:AP_RCTelemetry.*(.text* .rodata*)
        lib/lib*.a:AP_CRSF_Telem.*(.text* .rodata*)
        lib/lib*.a:vector2.*(.text* .rodata*)
        lib/lib*.a:quaternion.*(.text* .rodata*)
        lib/lib*.a:polygon.*(.text* .rodata*)
        lib/lib*.a:flash.*(.text* .rodata*)
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

    /* DTCM area: performance-critical read-only data */
    .ramdata : ALIGN(4)
    {
        . = ALIGN(4);
        __ram2_init_text__ = LOADADDR(.ramdata);
        __ram2_init__ = .;
        EXCLUDE_FILE (*vectors.o *crt0_v7m.o *crt1.o *board.o)
        *libch.a:ch*.*(.rodata*)
        *libch.a:nvic.*(.rodata*)
        *libch.a:bouncebuffer.*(.rodata*)
        *libch.a:stm32_util.*(.rodata*)
        *libch.a:stm32_dma.*(.rodata*)
        *libch.a:memstreams.*(.rodata*)
        *libch.a:malloc.*(.rodata*)
        *libch.a:hrt.*(.rodata*)
        *libch.a:hal*.*(.rodata*)
        lib/lib*.a:Semaphores.*(.rodata*)
        lib/lib*.a:AP_Math.*(.rodata*)
        lib/lib*.a:vector3.*(.rodata*)
        lib/lib*.a:matrix3.*(.rodata*)
        *libm.a:*(.rodata*)
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
        KEEP(*libch.a:vectors.o);
        KEEP(*libch.a:crt0_v7m.o);
        KEEP(*libch.a:crt1.o);
        KEEP(*libch.a:board.o);
        /* we want app_descriptor near the start of flash so a false
           positive isn't found by the bootloader (eg. ROMFS) */
        KEEP(*(.apsec_data));
        KEEP(*(.app_descriptor));
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

    /* Legacy symbol, not used anywhere. */
    . = ALIGN(4);
    PROVIDE(_etext = .);

    /* Special section for exceptions stack. */
    .mstack :
    {
        . = ALIGN(8);
        __main_stack_base__ = .;
        . += __main_stack_size__;
        . = ALIGN(8);
        __main_stack_end__ = .;
    } > MAIN_STACK_RAM

    /* Special section for process stack. */
    .pstack :
    {
        __process_stack_base__ = .;
        __main_thread_stack_base__ = .;
        . += __process_stack_size__;
        . = ALIGN(8);
        __process_stack_end__ = .;
        __main_thread_stack_end__ = .;
        PROVIDE(_eusrstack = .);
    } > PROCESS_STACK_RAM

    .data : ALIGN(4)
    {
        . = ALIGN(4);
        PROVIDE(_textdata = LOADADDR(.data));
        PROVIDE(_data = .);
        PROVIDE(_data_vma = .);
        __textdata_base__ = LOADADDR(.data);
        __data_base__ = .;
        *(.data)
        *(.data.*)
        *(.ramtext)
        . = ALIGN(8);
        PROVIDE( __global_pointer$ = . + 0x800 );
        *(.sdata .sdata.*)
        *(.sdata2.*)
        . = ALIGN(8);
        *(.srodata.cst16)
        *(.srodata.cst8)
        *(.srodata.cst4)
        *(.srodata.cst2)
        *(.srodata .srodata.*)
        . = ALIGN(4);
        PROVIDE(_edata = .);
        __data_end__ = .;
    } > DATA_RAM AT > default_flash

    .dlalign :
    {
        . = ALIGN(4);
        PROVIDE(_data_lma = .);
    } > default_flash AT > default_flash

    .bss (NOLOAD) : ALIGN(4)
    {
        . = ALIGN(4);
        PROVIDE( _sbss = .);
        __bss_base__ = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        PROVIDE(_ebss = .);
        PROVIDE(end = .);
    } > BSS_RAM

    .ram0_init : ALIGN(4)
    {
        . = ALIGN(4);
        __ram0_init_text__ = LOADADDR(.ram0_init);
        __ram0_init__ = .;
        *(.ram0_init)
        *(.ram0_init.*)
        . = ALIGN(4);
    } > ram0 AT > default_flash

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

    /* The default heap uses the (statically) unused part of a RAM section. */
    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        __heap_base__ = .;
        . = ORIGIN(HEAP_RAM) + LENGTH(HEAP_RAM);
        __heap_end__ = .;
    } > HEAP_RAM

    /* The crash log uses the unused part of a flash section. */
    .crash_log (NOLOAD) :
    {
        . = ALIGN(32);
        __crash_log_base__ = .;
        . = ORIGIN(default_flash) + LENGTH(default_flash);
        __crash_log_end__ = .;
    } > default_flash
}
