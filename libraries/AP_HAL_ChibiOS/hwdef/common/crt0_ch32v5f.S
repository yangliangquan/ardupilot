/*****************************************************************************

    Copyright (C) 2026. All rights reserved.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

*****************************************************************************

    File Name          : crt0_ch32v5f.S
    Description        : WCH CH32H417 V5F application startup for ArduPilot.

    Modification      : The application is linked to run in place (XIP) from
                         the external QSPI flash window. It therefore must
                         NOT reconfigure the system clock - the bootloader
                         has already set up the clock tree and the QSPI XIP
                         window before jumping here. There is also no
                         ".load" zero-wait-flash copy: the WCH symbols
                         (_loadcode_*, .load section) are provided by the
                         linker script with empty ranges, and no copy is
                         performed.

                         The hot code and the data are still copied out of
                         the flash before main(): loadcode2vma() walks the
                         two _highcode_* windows (ITCM and AXI SRAM, see
                         common_extf_ch32.ld) and loaddata2vma() walks
                         .data.

                         The PART is released by the core included with the
                         ChibiOS/RT RISC-V port and the WCH crt1.c startup
                         stubs (_data_lma/_data_vma, _sbss/_ebss,
                         __process_stack_base__/__process_stack_end__).

*****************************************************************************/

#include "riscvparams.h"

#if !defined(FALSE) || defined(__DOXYGEN__)
#define FALSE                               0
#endif

#if !defined(TRUE) || defined(__DOXYGEN__)
#define TRUE                                1
#endif

/**
 * @brief   CODE segments initialization switch.
 * @details Copies the _highcode_* windows (see common_extf_ch32.ld) from
 *          the external flash into ITCM / AXI SRAM.
 */
#if !defined(CRT0_INIT_CODE) || defined(__DOXYGEN__)
#define CRT0_INIT_CODE                      TRUE
#endif

/**
 * @brief   Core initialization switch.
 */
#if !defined(CRT0_INIT_CORE) || defined(__DOXYGEN__)
#define CRT0_INIT_CORE                      TRUE
#endif

/**
 * @brief   Stack segments initialization switch.
 */
#if !defined(CRT0_STACKS_FILL_PATTERN) || defined(__DOXYGEN__)
#define CRT0_STACKS_FILL_PATTERN            0x55555555
#endif

/**
 * @brief   Stack segments initialization switch.
 */
#if !defined(CRT0_INIT_STACKS) || defined(__DOXYGEN__)
#define CRT0_INIT_STACKS                    TRUE
#endif

/**
 * @brief   DATA segment initialization switch.
 */
#if !defined(CRT0_INIT_DATA) || defined(__DOXYGEN__)
#define CRT0_INIT_DATA                      TRUE
#endif

/**
 * @brief   BSS segment initialization switch.
 */
#if !defined(CRT0_INIT_BSS) || defined(__DOXYGEN__)
#define CRT0_INIT_BSS                       TRUE
#endif

/**
 * @brief   RAM areas initialization switch.
 */
#if !defined(CRT0_INIT_RAM_AREAS) || defined(__DOXYGEN__)
#define CRT0_INIT_RAM_AREAS                 TRUE
#endif

/**
 * @brief   Constructors invocation switch.
 */
#if !defined(CRT0_CALL_CONSTRUCTORS) || defined(__DOXYGEN__)
#define CRT0_CALL_CONSTRUCTORS              TRUE
#endif

/**
 * @brief   Destructors invocation switch.
 */
#if !defined(CRT0_CALL_DESTRUCTORS) || defined(__DOXYGEN__)
#define CRT0_CALL_DESTRUCTORS               TRUE
#endif

.section	.start,"ax",@progbits
	.global	_start
	.align	2
_start:
	/* Clear private-user-mode flags the same way as the WCH crt0. */
	li t0, 0x88
	csrc 0x800, t0
	fence.i

#ifdef HARDWARE_STACK
	/* QingKe hardware prologue stack setup, same as the port crt0. */
	li t0, 0x07
	csrw 0x804, t0
	fence.i
#endif

	/* Jump straight to the real startup code; under XIP there is no
	   .load section copy to perform. */
	la t0, handle_reset
	jr t0

	.section	.handle_reset,"ax",@progbits
	.weak	handle_reset
	.align	1

.section	.handle_reset,"ax",@progbits
.option push
handle_reset:
.option	norelax
	/* gp-relative addressing support (small data) */
	la gp, __global_pointer$

	/* Initial stack pointer. */
	la sp, _eusrstack

	/* Configure the XIP instruction prefetch. */
	li t0, 0x123703E1
	csrw 0xBC0, t0

	/* Enable the FPU and global interrupts, privileged mode; also set
	   MPIE so the kernel mret into _main keeps interrupts enabled. This
	   is owned by this image from here on: any interrupt or exception
	   taken now is served by this image's vector table (the bootloader
	   keeps all PFIC enables cleared for this window). */
	li t0, 0x7888
	csrw mstatus, t0

	/* Configure the interrupt vector table recognition mode and entry
	   address mode as the port crt0 does: mtvec points to the (data
	   copied) _vector_table with hardware prologue mode. No interrupt
	   source is enabled yet (PFIC enables came up cleared), so the
	   window with the .vector copy not yet loaded by loaddata2vma()
	   below cannot deliver an IRQ. */
	.extern _vector_table
	la t0, _vector_table
	ori t0, t0, 3
	csrw mtvec, t0

	/* NOTE: the system clock is NOT touched here. The bootloader has
	   already configured SYSCLK and the QSPI2 XIP window. The ChibiOS
	   hal_lld clock configuration must verify and keep the tree. */

	/* Copy the two RAM code windows (.fastcode -> ITCM and .ramfunc ->
	   AXI SRAM) out of the external flash. Everything else is XIP, so
	   this is the only code copy besides .data (loaddata2vma, below). */
#if CRT0_INIT_CODE == TRUE
	call	loadcode2vma
#endif

	/* VMDA-special core init hook. */
#if CRT0_INIT_CORE == TRUE
	call	__core_init
#endif

	/* Board early init: board clocks to peripheral tree, overrides. */
	call	__early_init

#if CRT0_INIT_STACKS
	call	colorize_stack
#endif

2:
#if CRT0_INIT_DATA
	call	loaddata2vma
#endif

#if CRT0_INIT_BSS
	call	clearbss
#endif

#if CRT0_INIT_RAM_AREAS == TRUE
	/* RAM areas initialization ($area when CRT0_AREAS_NUMBER > 1). */
	call	__init_ram_areas
#endif

	call	__late_init

#if CRT0_CALL_CONSTRUCTORS
	call	call_constructors
#endif

la t0, _main
	csrw mepc, t0
	mret

_main:
	call	main
#if CRT0_CALL_DESTRUCTORS
	call	call_destructors
#endif

	/* Jump to the exit handler. */
	la t0, __default_exit
	csrw mepc, t0
	mret
