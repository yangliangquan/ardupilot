/*
 * CH32H417 (QingKe V5F) memory layout, shared by the bootloader, the HAL
 * and the linker script documentation.
 *
 *   Boot image : internal CodeFlash, V5F zero-wait window at 0x00010000
 *   App image  : external QSPI2 NOR flash, memory mapped (XIP) at
 *                0x70000000, 4 MiB
 *   Hot code   : ITCM 0x200A0000 (128K) and upper shared SRAM
 *                0x20120000 (384K), copied from the app image by crt1
 *   Variables  : DTCM 0x200C0200 (.data/.bss/heap/stacks); the first
 *                512B of DTCM are hardware owned (HW stacking area)
 *
 * Keep these values in sync with:
 *   - hwdef/scripts/CH32H417xx.py  (CH32_MEMORY, EXT_FLASH_BASE)
 *   - hwdef/common/common_ch32.ld  (section rules)
 */

#pragma once

#include <stdint.h>

/* Internal CodeFlash (XIP, zero wait states).  V3F owns the first 64K. */
#define CH32H417_CODEFLASH_BASE        0x00000000UL
#define CH32H417_CODEFLASH_SIZE        (960U * 1024U)
#define CH32H417_V5F_FLASH_BASE        0x00010000UL
#define CH32H417_V5F_FLASH_SIZE        (768U * 1024U)
#define CH32H417_EFL_BASE              0x000D0000UL
#define CH32H417_EFL_SIZE              (128U * 1024U)

/* External QSPI2 NOR flash, memory mapped (XIP) window. */
#define CH32H417_QSPI1_XIP_BASE        0x90000000UL
#define CH32H417_QSPI2_XIP_BASE        0x70000000UL
#define CH32H417_EXT_FLASH_BASE        CH32H417_QSPI2_XIP_BASE
#define CH32H417_EXT_FLASH_SIZE        (4U * 1024U * 1024U)
#define CH32H417_APP_START_ADDRESS     CH32H417_EXT_FLASH_BASE

/* Tightly coupled / shared RAM. */
#define CH32H417_ITCM_BASE             0x200A0000UL
#define CH32H417_ITCM_SIZE             (128U * 1024U)
#define CH32H417_DTCM_BASE             0x200C0000UL
#define CH32H417_DTCM_HW_RESERVED      512U
#define CH32H417_DTCM_RAM_BASE         (CH32H417_DTCM_BASE + CH32H417_DTCM_HW_RESERVED)
#define CH32H417_DTCM_RAM_SIZE         ((256U * 1024U) - CH32H417_DTCM_HW_RESERVED)
#define CH32H417_SRAM_BASE             0x20100000UL
#define CH32H417_V5F_SRAM_BASE         0x20120000UL
#define CH32H417_V5F_SRAM_SIZE         (384U * 1024U)

/* Section attributes for code that must run at core speed.  These match the
   .itcm/.sram collectors in common_ch32.ld and the __*_FUNC__ macros that
   chibios_hwdef.py emits into hwdef.h. */
#define CH32H417_ITCM_CODE  __attribute__((section(".itcm"), used))
#define CH32H417_SRAM_CODE  __attribute__((section(".sram"), used))

#ifdef __cplusplus
extern "C" {
#endif

/* Symbols produced by the generated ldscript.ld / common_ch32.ld. */
extern uint8_t __itcm_start__, __itcm_end__;
extern uint8_t __sram_start__, __sram_end__;
extern uint8_t __ram0_start__, __ram0_end__;
extern uint8_t __flash_start__, __flash_end__;

#ifdef __cplusplus
}
#endif
