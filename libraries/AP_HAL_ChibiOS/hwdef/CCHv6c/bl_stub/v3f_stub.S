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

    File Name          : v3f_stub.S
    Description        : CCH-v6c Core0 (WCH QingKe V3F) reset stub.

    The CCH-v6c dual-core boot convention (mirrors the WCH CH32H417
    EVT 'DUAL' demo and the WCH EXAM 'Core_V3F' pattern):

      - after reset the V3F core (core0) executes from the CodeFlash
        alias window at 0x00000000 while the V5F core (core1) is held;
      - this stub performs the minimal V3F bring-up and then releases
        the V5F core with its PC pointing at the ArduPilot bootloader
        (linked at 0x00040000, 1024-byte aligned as required by the
        PFIC wakeup mechanism);
      - the stub then parks forever in WFI so the V3F never touches
        SRAM or peripherals owned by the V5F application.

    Releasing V5F per the CH32H417RM 4.7.5:
      R32_PFIC_WAKEIP1 (0xE000E724) = wake address (1024-aligned)
      R32_PFIC_SCTLR  (0xE000ED10) |= (1<<5)

    A slow blink on the red LED (PD10) is emitted before the release
    so a dead bootloader is distinguishable from a dead stub with a
    scope or meter.

*******************************************************************************/

/* RCC / GPIO constants (see ch32h417.h for the derivation) */
#define RCC_BASE                0x40021000
#define RCC_HB2PCENR_OFFSET     0x1C            /* RCC_HB2PCENR */
#define RCC_IOPDEN              0x00000020      /* I/O port D clock enable */

#define GPIOD_BASE              0x40011400      /* HBPERIPH_BASE + 0x11400 */
#define GPIO_CFGHR_OFFSET       0x04            /* pins 8..15 */
#define GPIO_OUTDR_OFFSET       0x0C
#define GPIO_BSHR_OFFSET        0x10
#define PIN_10_CFG              (0x3 << (4 * 2)) /* CFGHR nibble2: OUT_PP 50MHz */
#define PD10_BSHR_SET           0x0A             /* BSHR SET[15:0] = 10 */
#define PD10_BSHR_CLR           (0x0A << 16)     /* BSHR CLR[31:16] = 10 */

/* PFIC release registers (CH32H417RM 4.7.5) */
#define PFIC_WAKEIP1            0xE000E724
#define PFIC_SCTLR              0xE000ED10

/* the ArduPilot V5F bootloader entry (common_boot_ch32.ld flash org) */
#define V5F_BOOT_ADDR           0x00040000

/* roughly 0.4s per half-blink at the HSI reset clock */
#define BLINK_HALF              1000000

.section .vectors,"ax",@progbits
    .global _start
    .align  2
_start:
    /* ---- heartbeat: blink the red LED (PD10) three times ---- */

    /* enable the GPIOD clock: RCC_HB2PCENR |= RCC_IOPDEN */
    li      t0, RCC_BASE
    lw      t1, RCC_HB2PCENR_OFFSET(t0)
    li      t2, RCC_IOPDEN
    or      t1, t1, t2
    sw      t1, RCC_HB2PCENR_OFFSET(t0)

    /* PD10 as output push-pull: CFGHR nibble2 = 0x3 */
    li      t0, GPIOD_BASE
    lw      t1, GPIO_CFGHR_OFFSET(t0)
    li      t2, PIN_10_CFG
    or      t1, t1, t2
    sw      t1, GPIO_CFGHR_OFFSET(t0)

    li      s0, 3                          /* blink count */
    li      t0, GPIOD_BASE
    li      s1, PD10_BSHR_SET
    li      s2, PD10_BSHR_CLR

blink_half_on:
    sw      s1, GPIO_BSHR_OFFSET(t0)
    li      a0, BLINK_HALF
1:  addi    a0, a0, -1
    bnez    a0, 1b
blink_half_off:
    sw      s2, GPIO_BSHR_OFFSET(t0)
    li      a0, BLINK_HALF
1:  addi    a0, a0, -1
    bnez    a0, 1b
    sw      s1, GPIO_BSHR_OFFSET(t0)
    addi    s0, s0, -1
    bnez    s0, blink_half_off

    /* ---- release the V5F core into the bootloader ---- */

    /* R32_PFIC_WAKEIP1 = V5F_BOOT_ADDR */
    li      t0, PFIC_WAKEIP1
    li      t1, V5F_BOOT_ADDR
    sw      t1, 0(t0)

    /* R32_PFIC_SCTLR |= (1<<5) */
    li      t0, PFIC_SCTLR
    lw      t1, 0(t0)
    li      t2, 0x20
    or      t1, t1, t2
    sw      t1, 0(t0)

    /* ---- park: never touch anything the V5F application owns ---- */
1:  wfi
    j       1b

    /* pad the reset region with RISC-V 32-bit nops out to 1KB; the
       padding is never executed - the V5F wake address (0x00040000)
       is the one that must be 1024-aligned */
    .rept   253
    nop
    .endr
