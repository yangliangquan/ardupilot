/*
    This file is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along
    with this program.  If not, see <http://www.gnu.org/licenses/>.

    CH32H417 (WCH QingKe V5F) mcu configuration for ArduPilot.
    Modelled on the WCH ChibiOS EVT demo mcuconf.h, adjusted for the
    AP HAL requirements.
*/

#pragma once

#include "ch32h417.h"

/*
 * Applications are built to run in place from the external QSPI flash
 * window; the bootloader owns the clock tree. Used by hal_lld to skip
 * clock switching when executing from the window.
 */
#define CH32_XIP_PROG_BASE                 0x70000000U

/*
 * AP code (and newlib) probe these; the QingKe V5F has a single
 * precision FPU. The address is the F1 style unique-id location and
 * needs verification against the datasheet.
 */
#ifndef __FPU_PRESENT
#define __FPU_PRESENT                       1
#endif
#define __FPU_USED                          1
#ifndef UDID_START
#define UDID_START                          0x1FFFF7E0U /* FIXME(CH32): verify from CH32H417DS */
#endif
// (UDID_START is also generated into hwdef.h by chibios_hwdef.py; keep them equal)

/*===========================================================================*/
/* Compatibility bridges: the AP hwdef generator emits STM32_* names;         */
/* map them onto the CH32 port equivalents so shared AP HAL code may be       */
/* reused unchanged.                                                          */
/*===========================================================================*/

/* DMA stream id macros from the CH32 DMA driver */
#define STM32_DMA_STREAM_ID(dma, stream)      CH32_DMA_STREAM_ID(dma, stream)
#define STM32_DMA_STREAM_ID_MSK(dma, stream)  CH32_DMA_STREAM_ID_MSK(dma, stream)
#define STM32_DMA_STREAM_ID_ANY               CH32_DMA_STREAM_ID_ANY
#define STM32_DMA_STREAM_ID_ANY_DMA1          CH32_DMA_STREAM_ID_ANY_DMA1
#define STM32_DMA_STREAM_ID_ANY_DMA2          CH32_DMA_STREAM_ID_ANY_DMA2
#define STM32_DMA_STREAMS                     CH32_DMA_STREAMS

/* DMAMUX request line (DMA request source). The CH32 DMAMUX request
   numbers are provided by the ch32_dmamux.h enumerations. */
#define CH32_DMA_ERROR_HOOK(devp) osalSysHalt("DMA failure")

/* DMA CCR mode bits (F1/GP style layout). Use the WCH DMA_CFGR1_* bits. */
#define STM32_DMA_CR_EN              DMA_CFGR1_EN
#define STM32_DMA_CR_TEIE            DMA_CFGR1_TEIE
#define STM32_DMA_CR_HTIE            DMA_CFGR1_HTIE
#define STM32_DMA_CR_TCIE            DMA_CFGR1_TCIE
#define STM32_DMA_CR_DIR_P2M         0U
#define STM32_DMA_CR_DIR_M2P         DMA_CFGR1_DIR
#define STM32_DMA_CR_DIR_M2M         DMA_CFGR1_MEM2MEM
#define STM32_DMA_CR_CIRC            DMA_CFGR1_CIRC
#define STM32_DMA_CR_PINC            DMA_CFGR1_PINC
#define STM32_DMA_CR_MINC            DMA_CFGR1_MINC
#define STM32_DMA_CR_PSIZE_MASK      DMA_CFGR1_PSIZE
#define STM32_DMA_CR_PSIZE_BYTE      0U
#define STM32_DMA_CR_PSIZE_HWORD     DMA_CFGR1_PSIZE_0
#define STM32_DMA_CR_PSIZE_WORD      DMA_CFGR1_PSIZE_1
#define STM32_DMA_CR_MSIZE_MASK      DMA_CFGR1_MSIZE
#define STM32_DMA_CR_MSIZE_BYTE      0U
#define STM32_DMA_CR_MSIZE_HWORD     DMA_CFGR1_MSIZE_0
#define STM32_DMA_CR_MSIZE_WORD      DMA_CFGR1_MSIZE_1
#define STM32_DMA_CR_SIZE_MASK       (DMA_CFGR1_PSIZE | DMA_CFGR1_MSIZE)
#define STM32_DMA_CR_PL_MASK         DMA_CFGR1_PL
#define STM32_DMA_CR_PL(n)           ((n) << 12U)
/* DMAMUX based; the request source is set with dmaSetRequestSource() */
#define STM32_DMA_CR_CHSEL_MASK      0U
#define STM32_DMA_CR_CHSEL(n)        0U
#define STM32_DMA_CR_DMEIE           0U
/* status flags passed to ISR callbacks */
#define STM32_DMA_ISR_FEIF           0U
#define STM32_DMA_ISR_DMEIF          0U
#define STM32_DMA_ISR_TEIF           0x8U
#define STM32_DMA_ISR_HTIF           0x4U
#define STM32_DMA_ISR_TCIF           0x2U

/* DMAMUX request enumeration bridge: resolver emits STM32_DMAMUX1_*, the
   CH32 DMAMUX conditional enums. */
#define STM32_DMAMUX1_TIM1_CH1       DMA_MUX_TIM1_CH1
#define STM32_DMAMUX1_TIM1_CH2       DMA_MUX_TIM1_CH2
#define STM32_DMAMUX1_TIM1_CH3       DMA_MUX_TIM1_CH3
#define STM32_DMAMUX1_TIM1_CH4       DMA_MUX_TIM1_CH4
#define STM32_DMAMUX1_TIM1_UP        DMA_MUX_TIM1_UP
#define STM32_DMAMUX1_TIM1_TRIG      DMA_MUX_TIM1_TRIG
#define STM32_DMAMUX1_TIM2_CH1       DMA_MUX_TIM2_CH1
#define STM32_DMAMUX1_TIM2_CH2       DMA_MUX_TIM2_CH2
#define STM32_DMAMUX1_TIM2_CH3       DMA_MUX_TIM2_CH3
#define STM32_DMAMUX1_TIM2_CH4       DMA_MUX_TIM2_CH4
#define STM32_DMAMUX1_TIM2_UP        DMA_MUX_TIM2_UP
#define STM32_DMAMUX1_TIM2_TRIG      DMA_MUX_TIM2_TRIG
#define STM32_DMAMUX1_TIM3_CH1       DMA_MUX_TIM3_CH1
#define STM32_DMAMUX1_TIM3_CH2       DMA_MUX_TIM3_CH2
#define STM32_DMAMUX1_TIM3_CH3       DMA_MUX_TIM3_CH3
#define STM32_DMAMUX1_TIM3_CH4       DMA_MUX_TIM3_CH4
#define STM32_DMAMUX1_TIM3_UP        DMA_MUX_TIM3_UP
#define STM32_DMAMUX1_TIM3_TRIG      DMA_MUX_TIM3_TRIG
#define STM32_DMAMUX1_TIM4_CH1       DMA_MUX_TIM4_CH1
#define STM32_DMAMUX1_TIM4_CH2       DMA_MUX_TIM4_CH2
#define STM32_DMAMUX1_TIM4_CH3       DMA_MUX_TIM4_CH3
#define STM32_DMAMUX1_TIM4_CH4       DMA_MUX_TIM4_CH4
#define STM32_DMAMUX1_TIM4_UP        DMA_MUX_TIM4_UP
#define STM32_DMAMUX1_TIM4_TRIG      DMA_MUX_TIM4_TRIG
#define STM32_DMAMUX1_TIM5_CH1       DMA_MUX_TIM5_CH1
#define STM32_DMAMUX1_TIM5_CH2       DMA_MUX_TIM5_CH2
#define STM32_DMAMUX1_TIM5_CH3       DMA_MUX_TIM5_CH3
#define STM32_DMAMUX1_TIM5_CH4       DMA_MUX_TIM5_CH4
#define STM32_DMAMUX1_TIM5_UP        DMA_MUX_TIM5_UP
#define STM32_DMAMUX1_TIM5_TRIG      DMA_MUX_TIM5_TRIG
#define STM32_DMAMUX1_TIM6_UP        DMA_MUX_TIM6_UP
#define STM32_DMAMUX1_TIM7_UP        DMA_MUX_TIM7_UP
#define STM32_DMAMUX1_TIM8_CH1       DMA_MUX_TIM8_CH1
#define STM32_DMAMUX1_TIM8_CH2       DMA_MUX_TIM8_CH2
#define STM32_DMAMUX1_TIM8_CH3       DMA_MUX_TIM8_CH3
#define STM32_DMAMUX1_TIM8_CH4       DMA_MUX_TIM8_CH4
#define STM32_DMAMUX1_TIM8_UP        DMA_MUX_TIM8_UP
#define STM32_DMAMUX1_TIM8_TRIG      DMA_MUX_TIM8_TRIG
#define STM32_DMAMUX1_TIM9_CH1       DMA_MUX_TIM9_CH1
#define STM32_DMAMUX1_TIM9_CH2       DMA_MUX_TIM9_CH2
#define STM32_DMAMUX1_TIM9_CH3       DMA_MUX_TIM9_CH3
#define STM32_DMAMUX1_TIM9_CH4       DMA_MUX_TIM9_CH4
#define STM32_DMAMUX1_TIM9_UP        DMA_MUX_TIM9_UP
#define STM32_DMAMUX1_TIM9_TRIG      DMA_MUX_TIM9_TRIG
#define STM32_DMAMUX1_SPI1_RX        DMA_MUX_SPI1_RX
#define STM32_DMAMUX1_SPI1_TX        DMA_MUX_SPI1_TX
#define STM32_DMAMUX1_SPI2_RX        DMA_MUX_SPI2_RX
#define STM32_DMAMUX1_SPI2_TX        DMA_MUX_SPI2_TX
#define STM32_DMAMUX1_SPI3_RX        DMA_MUX_SPI3_RX
#define STM32_DMAMUX1_SPI3_TX        DMA_MUX_SPI3_TX
#define STM32_DMAMUX1_SPI4_RX        DMA_MUX_SPI4_RX
#define STM32_DMAMUX1_SPI4_TX        DMA_MUX_SPI4_TX
#define STM32_DMAMUX1_I2C2_RX        DMA_MUX_I2C2_RX
#define STM32_DMAMUX1_I2C2_TX        DMA_MUX_I2C2_TX
#define STM32_DMAMUX1_I2C1_RX        DMA_MUX_I2C1_RX
#define STM32_DMAMUX1_I2C1_TX        DMA_MUX_I2C1_TX
#define STM32_DMAMUX1_I2C3_RX        DMA_MUX_I2C3_RX
#define STM32_DMAMUX1_I2C3_TX        DMA_MUX_I2C3_TX
#define STM32_DMAMUX1_I2C4_RX        DMA_MUX_I2C4_RX
#define STM32_DMAMUX1_I2C4_TX        DMA_MUX_I2C4_TX
#define STM32_DMAMUX1_QSPI1_DMA      DMA_MUX_QSPI1_DMA
#define STM32_DMAMUX1_QSPI2_DMA      DMA_MUX_QSPI2_DMA
#define STM32_DMAMUX1_USART1_RX      DMA_MUX_USART1_RX
#define STM32_DMAMUX1_USART1_TX      DMA_MUX_USART1_TX
#define STM32_DMAMUX1_USART2_RX      DMA_MUX_USART2_RX
#define STM32_DMAMUX1_USART2_TX      DMA_MUX_USART2_TX
#define STM32_DMAMUX1_USART3_RX      DMA_MUX_USART3_RX
#define STM32_DMAMUX1_USART3_TX      DMA_MUX_USART3_TX
#define STM32_DMAMUX1_USART4_RX      DMA_MUX_USART4_RX
#define STM32_DMAMUX1_USART4_TX      DMA_MUX_USART4_TX
#define STM32_DMAMUX1_USART5_RX      DMA_MUX_USART5_RX
#define STM32_DMAMUX1_USART5_TX      DMA_MUX_USART5_TX
#define STM32_DMAMUX1_USART6_RX      DMA_MUX_USART6_RX
#define STM32_DMAMUX1_USART6_TX      DMA_MUX_USART6_TX
#define STM32_DMAMUX1_USART7_RX      DMA_MUX_USART7_RX
#define STM32_DMAMUX1_USART7_TX      DMA_MUX_USART7_TX
#define STM32_DMAMUX1_USART8_RX      DMA_MUX_USART8_RX
#define STM32_DMAMUX1_USART8_TX      DMA_MUX_USART8_TX
#define STM32_DMAMUX1_SDIO           DMA_MUX_SDIO
#define STM32_DMAMUX1_ADC1           DMA_MUX_ADC1

/* pin mode names shared with the AP PAL layer */
#define PAL_STM32_MODE_INPUT                  PAL_MODE_INPUT
#define PAL_STM32_MODE_OUTPUT                 PAL_MODE_OUTPUT_PUSHPULL
#define PAL_MODE_STM32_ALTERNATE_PUSHPULL(n)  PAL_CH32_ALTERNATE_PUSHPULL(n)
#define PAL_MODE_STM32_ALTERNATE_OPENDRAIN(n) PAL_CH32_ALTERNATE_OPENDRAIN(n)
#define PAL_MODE_ALTERNATE(n)                 PAL_CH32_ALTERNATE_PUSHPULL(n)

/*===========================================================================*/
/* Driver enable bridge: upstream AP code tests STM32_<DRV>_USE_<PERIPH>      */
/* switches from mcuconf; route them to the CH32 port switch names.           */
/*===========================================================================*/

#ifndef STM32_SPI_USE_SPI1
#define STM32_SPI_USE_SPI1                   CH32_SPI_USE_SPI1
#endif
#ifndef STM32_SPI_USE_SPI2
#define STM32_SPI_USE_SPI2                   CH32_SPI_USE_SPI2
#endif
#ifndef STM32_SPI_USE_SPI3
#define STM32_SPI_USE_SPI3                   CH32_SPI_USE_SPI3
#endif
#ifndef STM32_SPI_USE_SPI4
#define STM32_SPI_USE_SPI4                   CH32_SPI_USE_SPI4
#endif

#ifndef STM32_I2C_USE_I2C1
#define STM32_I2C_USE_I2C1                   CH32_I2C_USE_I2C1
#endif
#ifndef STM32_I2C_USE_I2C2
#define STM32_I2C_USE_I2C2                   CH32_I2C_USE_I2C2
#endif
#ifndef STM32_I2C_USE_I2C3
#define STM32_I2C_USE_I2C3                   CH32_I2C_USE_I2C3
#endif
#ifndef STM32_I2C_USE_I2C4
#define STM32_I2C_USE_I2C4                   CH32_I2C_USE_I2C4
#endif

#ifndef STM32_CAN_USE_CAN1
#define STM32_CAN_USE_CAN1                   CH32_CAN_USE_CAN1
#endif
#ifndef STM32_CAN_USE_CAN2
#define STM32_CAN_USE_CAN2                   CH32_CAN_USE_CAN2
#endif
#ifndef STM32_CAN_USE_CAN3
#define STM32_CAN_USE_CAN3                   CH32_CAN_USE_CAN3
#endif

#ifndef STM32_SERIAL_USE_USART1
#define STM32_SERIAL_USE_USART1              FALSE
#endif
#define CH32_SERIAL_USE_USART1              STM32_SERIAL_USE_USART1
#ifndef STM32_SERIAL_USE_USART2
#define STM32_SERIAL_USE_USART2              FALSE
#endif
#define CH32_SERIAL_USE_USART2              STM32_SERIAL_USE_USART2
#ifndef STM32_SERIAL_USE_USART3
#define STM32_SERIAL_USE_USART3              FALSE
#endif
#define CH32_SERIAL_USE_USART3              STM32_SERIAL_USE_USART3
#ifndef STM32_SERIAL_USE_USART4
#define STM32_SERIAL_USE_USART4              FALSE
#endif
#define CH32_SERIAL_USE_USART4              STM32_SERIAL_USE_USART4
#ifndef STM32_SERIAL_USE_USART5
#define STM32_SERIAL_USE_USART5              FALSE
#endif
#define CH32_SERIAL_USE_USART5              STM32_SERIAL_USE_USART5
#ifndef STM32_SERIAL_USE_USART6
#define STM32_SERIAL_USE_USART6              FALSE
#endif
#define CH32_SERIAL_USE_USART6              STM32_SERIAL_USE_USART6
#ifndef STM32_SERIAL_USE_USART7
#define STM32_SERIAL_USE_USART7              FALSE
#endif
#define CH32_SERIAL_USE_USART7              STM32_SERIAL_USE_USART7
#ifndef STM32_SERIAL_USE_USART8
#define STM32_SERIAL_USE_USART8              FALSE
#endif
#define CH32_SERIAL_USE_USART8              STM32_SERIAL_USE_USART8

#ifndef STM32_UART_USE_USART1
#define STM32_UART_USE_USART1              FALSE
#endif
#define CH32_UART_USE_USART1              STM32_UART_USE_USART1
#ifndef STM32_UART_USE_USART2
#define STM32_UART_USE_USART2              FALSE
#endif
#define CH32_UART_USE_USART2              STM32_UART_USE_USART2
#ifndef STM32_UART_USE_USART3
#define STM32_UART_USE_USART3              FALSE
#endif
#define CH32_UART_USE_USART3              STM32_UART_USE_USART3
#ifndef STM32_UART_USE_USART4
#define STM32_UART_USE_USART4              FALSE
#endif
#define CH32_UART_USE_USART4              STM32_UART_USE_USART4
#ifndef STM32_UART_USE_USART5
#define STM32_UART_USE_USART5              FALSE
#endif
#define CH32_UART_USE_USART5              STM32_UART_USE_USART5
#ifndef STM32_UART_USE_USART6
#define STM32_UART_USE_USART6              FALSE
#endif
#define CH32_UART_USE_USART6              STM32_UART_USE_USART6
#ifndef STM32_UART_USE_USART7
#define STM32_UART_USE_USART7              FALSE
#endif
#define CH32_UART_USE_USART7              STM32_UART_USE_USART7
#ifndef STM32_UART_USE_USART8
#define STM32_UART_USE_USART8              FALSE
#endif
#define CH32_UART_USE_USART8              STM32_UART_USE_USART8

#ifndef STM32_PWM_USE_TIM1
#define STM32_PWM_USE_TIM1                   CH32_PWM_USE_TIM1
#endif
#ifndef STM32_PWM_USE_TIM2
#define STM32_PWM_USE_TIM2                   CH32_PWM_USE_TIM2
#endif
#ifndef STM32_PWM_USE_TIM3
#define STM32_PWM_USE_TIM3                   CH32_PWM_USE_TIM3
#endif
#ifndef STM32_PWM_USE_TIM4
#define STM32_PWM_USE_TIM4                   CH32_PWM_USE_TIM4
#endif
#ifndef STM32_PWM_USE_TIM5
#define STM32_PWM_USE_TIM5                   CH32_PWM_USE_TIM5
#endif

#ifndef STM32_ICU_USE_TIM1
#define STM32_ICU_USE_TIM1                   CH32_ICU_USE_TIM1
#endif
#ifndef STM32_ICU_USE_TIM2
#define STM32_ICU_USE_TIM2                   CH32_ICU_USE_TIM2
#endif
#ifndef STM32_ICU_USE_TIM3
#define STM32_ICU_USE_TIM3                   CH32_ICU_USE_TIM3
#endif
#ifndef STM32_ICU_USE_TIM4
#define STM32_ICU_USE_TIM4                   CH32_ICU_USE_TIM4
#endif
#ifndef STM32_ICU_USE_TIM5
#define STM32_ICU_USE_TIM5                   CH32_ICU_USE_TIM5
#endif

#ifndef STM32_SDC_USE_SDC1
#define STM32_SDC_USE_SDC1                   CH32_SDC_USE_SDC1
#endif
#ifndef STM32_SDC_USE_SDMMC1
#define STM32_SDC_USE_SDMMC1                 CH32_SDC_USE_SDIO
#endif
#ifndef STM32_OTG_USE_USB1
#define STM32_OTG_USE_USB1                   CH32_OTG_USE_USB1
#endif
#ifndef STM32_ADC_USE_ADC1
#define STM32_ADC_USE_ADC1                   CH32_ADC_USE_ADC1
#endif
#ifndef STM32_ADC_USE_ADC2
#define STM32_ADC_USE_ADC2                   CH32_ADC_USE_ADC2
#endif

#ifndef STM32_DMA_SUPPORTS_DMAMUX
#define STM32_DMA_SUPPORTS_DMAMUX            CH32_DMA_SUPPORTS_DMAMUX
#endif
#ifndef STM32_DMA_SUPPORTS_CSELR
#define STM32_DMA_SUPPORTS_CSELR             0
#endif
#ifndef STM32_DMA_STREAMS
#define STM32_DMA_STREAMS                    CH32_DMA_STREAMS
#endif
#ifndef STM32_DMA_ADVANCED
#define STM32_DMA_ADVANCED                   0
#endif
#ifndef STM32_SPI_USE_SPI5
#define STM32_SPI_USE_SPI5                   FALSE
#endif
#ifndef STM32_SPI_USE_SPI6
#define STM32_SPI_USE_SPI6                   FALSE
#endif
/* central RISC-V type bridges (the ARM port provides these kinds of names) */
typedef struct ch32_dma_stream stm32_dma_stream_t;
/* the CH32 DMA ISR callback type is a function pointer defined in the LLD
   header (included later by hal.h) */
typedef void (*stm32_dmaisr_t)(void *p, uint32_t flags);
typedef TIM_TypeDef stm32_tim_t;
typedef uint32_t stkalign_t;

/* features the QingKe port does not plan to implement */
#define HAL_USE_EICU                         FALSE

/* NVIC API bridge for the RT8 RISC-V port (QingKe PFIC). The WCH
   core_riscv helper calls take an IRQn and an 8 bit priority (higher
   value == higher priority is handled inside the port). */

/* UART inverter/push-pull-port access bridge macros */
#define nvicEnableVector(irq, prio)  do {                                    \
    NVIC_SetPriority((IRQn_Type)(uint32_t)(irq), (uint8_t)(prio));           \
    NVIC_EnableIRQ((IRQn_Type)(uint32_t)(irq));                              \
} while (0)
#define nvicDisableVector(irq)       NVIC_DisableIRQ((IRQn_Type)(uint32_t)(irq))
#ifndef CORTEX_MAX_KERNEL_PRIORITY
#define CORTEX_MAX_KERNEL_PRIORITY           PORT_HIGHEST_PRIORITY
#endif

/* SPI register-name bridge (WCH uses CTLR1/STATR instead of CR1/SR) */
#define SPI_CR1_CPOL                         SPI_CTLR1_CPOL
#define SPI_CR1_CPHA                         SPI_CTLR1_CPHA
#define SPI_CR1_MSTR                         SPI_CTLR1_MSTR
#define SPI_CR1_SPE                          SPI_CTLR1_SPE
#define SPI_CR1_SSI                          SPI_CTLR1_SSI
#define SPI_CR1_SSM                          SPI_CTLR1_SSM
#define SPI_CR1_LSBFIRST                     SPI_CTLR1_LSBFIRST
#define SPI_CR1_BR                           SPI_CTLR1_BR
#define SPI_CR1_BR_0                         SPI_CTLR1_BR_0
#define SPI_CR1_BR_1                         SPI_CTLR1_BR_1
#define SPI_CR1_BR_2                         SPI_CTLR1_BR_2
#define SPI_CR1_MTXIE                        SPI_CTLR1_MTXIE /* FIXME(CH32): verify */
/* I2C limit flag is an STM32 I2Cv1 concept; PFIC I2C on CH32 doesn't. */
#define I2C_ISR_LIMIT                        ((uint16_t)0x0080)

/* RCC enable bridge helpers for the upstream names used in AP drivers */
#define rccEnableSPI1(lp)  enableHB2(RCC_SPI1EN)
#define rccEnableSPI2(lp)  enableHB1(RCC_SPI2EN)
#define rccEnableSPI3(lp)  enableHB1(RCC_SPI3EN)
#define rccEnableSPI4(lp)  enableHB1(RCC_SPI4EN)
#define rccEnableI2C1(lp)  enableHB2(RCC_I2C1EN)
#define rccEnableI2C2(lp)  enableHB1(RCC_I2C2EN)
#define rccEnableI2C3(lp)  enableHB1(RCC_I2C3EN)
#define rccEnableI2C4(lp)  enableHB1(RCC_I2C4EN)

/* TIM DMA request bits (WCH names) */
#define TIM_DIER_UDE                         TIM_UDE
/* TIM DMA address register (WCH name). Only used as a member token in
   AP code; the WCH headers have no struct fields named DMAR. */
#define DMAR                                 DMAADR

/*===========================================================================*/
/* USART bit-name bridges (WCH chose CTLR/STATR names)                        */
/*===========================================================================*/
#define USART_SR_IDLE                        USART_STATR_IDLE
#define USART_SR_RXNE                        USART_STATR_RXNE
#define USART_SR_TXE                         USART_STATR_TXE
#define USART_SR_TC                          USART_STATR_TC
#define USART_SR_ORE                         USART_STATR_ORE
#define USART_SR_FE                          USART_STATR_FE
#define USART_SR_NE                          USART_STATR_NE
#define USART_CR1_IDLEIE                     USART_CTLR1_IDLEIE
#define USART_CR1_RXNEIE                     USART_CTLR1_RXNEIE
#define USART_CR1_TXEIE                      USART_CTLR1_TXEIE
#define USART_CR1_TCIE                       USART_CTLR1_TCIE
#define USART_CR1_UE                         USART_CTLR1_UE
#define USART_CR2_LBDIE                      USART_CTLR2_LBDIE
#define USART_CR3_DMAR                       USART_CTLR3_DMAR
#define USART_CR3_DMAT                       USART_CTLR3_DMAT
#define USART_CR1_M                          USART_CTLR1_M
#define USART_CR1_PCE                        USART_CTLR1_PCE
#define USART_CR1_PS                         USART_CTLR1_PS
#define USART_CR1_OVER8                      0 /* FIXME(CH32): no single sample per bit mode */
#define USART_CR3_CTSE                       USART_CTLR3_CTSE
#define USART_CR3_RTSE                       USART_CTLR3_RTSE
#define USART_CR2_STOP1_BITS                 0
#define USART_CR2_STOP2_BITS                 0
#define USART_CR2_SWAP                       0  /* FIXME(CH32): no pin swap */
#define USART_CR2_TXINV                      0  /* FIXME(CH32): no invert */
#define USART_CR2_RXINV                      0  /* FIXME(CH32): no invert */
/* WCH uses half-duplex select instead of driver-enable mode */
#define USART_CR3_DEM                        USART_CTLR3_HDSEL

/* WSPI/QSPI clock bridge: the QUADSPI-compatible controller is clocked
   from the shared bus clock on this port. TODO(CH32): model the real
   QSPI clock point in hal_lld. */
#define STM32_QSPICLK                        HCLKClock
#define STM32_OCTOSPICLK                     HCLKClock
#define USART_CR3_HDSEL                      USART_CTLR3_HDSEL

/*===========================================================================*/
/* PAL mode-name bridge for the upstream AP code style                        */
/*===========================================================================*/
#define PAL_STM32_MODE_INPUT                 PAL_MODE_INPUT
#define PAL_STM32_MODE_OUTPUT                PAL_MODE_OUTPUT_PUSHPULL
#define PAL_STM32_MODE_ALTERNATE(n)          PAL_CH32_ALTERNATE_PUSHPULL(n)
#define PAL_STM32_OTYPE_PUSHPULL             0
#define PAL_STM32_OTYPE_OPENDRAIN            PAL_MODE_OUTPUT_OPENDRAIN
#define PAL_STM32_OSPEED_LOWEST              0
#define PAL_STM32_OSPEED_LOW                 PIN_SPEED_LOW
#define PAL_STM32_OSPEED_MID1                PIN_SPEED_MEDIUM
#define PAL_STM32_OSPEED_MEDIUM              PIN_SPEED_MEDIUM
#define PAL_STM32_OSPEED_HIGH                PIN_SPEED_HIGH
#define PAL_STM32_PUPDR_FLOATING             PAL_MODE_INPUT
#define PAL_STM32_PUPDR_PULLUP               PAL_MODE_INPUT_PULLUP
#define PAL_STM32_PUPDR_PULLDOWN             PAL_MODE_INPUT_PULLDOWN

/*===========================================================================*/
/* RISC-V core access bridges (Cortex asm names that AP code references)      */
/*===========================================================================*/
#ifndef __get_PSP
#define __get_PSP()                          __get_SP()
#endif
#define __get_IPSR()                         0
/* QingKe V5F trap vectors are handled by the port; the ARM SCB records
   that AP reads in fault handlers do not exist */


/*===========================================================================*/
/* Clock bridge: the WCH HAL exposes clock frequencies as globals.            */
/* TODO(CH32): add per-bus (HB1/HB2) clock points in the fork hal_lld;        */
/* these bridges currently assume all peripheral buses run at HCLK.           */
/*===========================================================================*/
extern uint32_t SystemClock;
extern uint32_t SystemCoreClock;
extern uint32_t HCLKClock;

#define STM32_SYS_CK                         SystemClock
#define STM32_CORE_CK                        SystemCoreClock
#define STM32_HCLK                           HCLKClock
#define STM32_PCLK1                          HCLKClock
#define STM32_PCLK2                          HCLKClock
#define STM32_PCLK                           HCLKClock

/*===========================================================================*/
/* System configuration                                                      */
/*===========================================================================*/

#define CH32H41x_MCUCONF
#define CH32H417_MCUCONF

#define CH32_NO_INIT                       FALSE
#define CH32_PVD_ENABLE                    FALSE
#define CH32_HSI_ENABLED                   TRUE
#define CH32_LSI_ENABLED                   TRUE
#define CH32_HSE_ENABLED                   TRUE
#define CH32_LSE_ENABLED                   FALSE

#define CH32_SW                            RCC_SW_PLL
#define CH32_SYSPLL                        RCC_SYSPLL_PLL
#define CH32_PLLSRC                        RCC_PLLSRC_HSE
#define CH32_PLL_SRC_DIV                   RCC_PLL_SRC_DIV1
#define CH32_PLL_MUL                       RCC_PLLMUL16

#define CH32_HPRE                          RCC_HPRE_DIV1
#define CH32_HPRF                          RCC_FPRE_DIV4

/*===========================================================================*/
/* ADC driver system settings.                                               */
/*===========================================================================*/
#define CH32_ADC_PRE2_DIV                  RCC_PPRE2_ADCL_DIV16
#define CH32_ADC_ADCPRE                    RCC_ADCPRE_ADCH_DIV8
#define CH32_ADC_USE_ADC1                  TRUE
#define CH32_ADC_USE_ADC2                  FALSE
#define CH32_ADC_IRQ_PRIORITY              6
#define CH32_ADC_ADC1_DMA_STREAM           CH32_DMA_STREAM_ID_ANY
#define CH32_ADC_ADC1_DMA_PRIORITY         2
#define CH32_ADC_ADC2_DMA_STREAM           CH32_DMA_STREAM_ID_ANY
#define CH32_ADC_ADC2_DMA_PRIORITY         2

/*===========================================================================*/
/* CAN driver system settings.                                               */
/*===========================================================================*/
#define CH32_CAN_USE_CAN1                  TRUE
#define CH32_CAN_USE_CAN2                  FALSE
#define CH32_CAN_USE_CAN3                  TRUE
#define CH32_CAN_CAN1_IRQ_PRIORITY         9
#define CH32_CAN_CAN2_IRQ_PRIORITY         9
#define CH32_CAN_CAN3_IRQ_PRIORITY         9

/*===========================================================================*/
/* GPT driver system settings.                                               */
/* note: Timer 2 is free for the HAL (system timer uses TIM7).                */
/*===========================================================================*/
#define CH32_GPT_USE_TIM1                  FALSE
#define CH32_GPT_USE_TIM2                  FALSE
#define CH32_GPT_USE_TIM3                  FALSE
#define CH32_GPT_USE_TIM4                  FALSE
#define CH32_GPT_USE_TIM5                  FALSE
#define CH32_GPT_USE_TIM6                  FALSE
#define CH32_GPT_USE_TIM7                  FALSE
#define CH32_GPT_USE_TIM8                  FALSE
#define CH32_GPT_USE_TIM9                  FALSE
#define CH32_GPT_USE_TIM10                 FALSE
#define CH32_GPT_USE_TIM11                 FALSE
#define CH32_GPT_USE_TIM12                 FALSE
#define CH32_GPT_USE_TIM13                 FALSE
#define CH32_GPT_USE_TIM14                 FALSE
#define CH32_GPT_TIM1_IRQ_PRIORITY         7
#define CH32_GPT_TIM2_IRQ_PRIORITY         7
#define CH32_GPT_TIM3_IRQ_PRIORITY         7
#define CH32_GPT_TIM4_IRQ_PRIORITY         7
#define CH32_GPT_TIM5_IRQ_PRIORITY         7
#define CH32_GPT_TIM6_IRQ_PRIORITY         7
#define CH32_GPT_TIM7_IRQ_PRIORITY         7
#define CH32_GPT_TIM8_IRQ_PRIORITY         7
#define CH32_GPT_TIM9_IRQ_PRIORITY         7
#define CH32_GPT_TIM10_IRQ_PRIORITY        7
#define CH32_GPT_TIM11_IRQ_PRIORITY        7
#define CH32_GPT_TIM12_IRQ_PRIORITY        7
#define CH32_GPT_TIM13_IRQ_PRIORITY        7
#define CH32_GPT_TIM14_IRQ_PRIORITY        7

/*===========================================================================*/
/* PWM driver system settings.                                               */
/*===========================================================================*/
#define CH32_PWM_USE_TIM1                  TRUE
#define CH32_PWM_USE_TIM2                  FALSE
#define CH32_PWM_USE_TIM3                  TRUE
#define CH32_PWM_USE_TIM4                  TRUE
#define CH32_PWM_USE_TIM5                  TRUE
#define CH32_PWM_USE_TIM6                  FALSE
#define CH32_PWM_USE_TIM7                  FALSE
#define CH32_PWM_USE_TIM8                  FALSE
#define CH32_PWM_USE_TIM9                  FALSE
#define CH32_PWM_USE_TIM10                 FALSE
#define CH32_PWM_USE_TIM11                 FALSE
#define CH32_PWM_USE_TIM12                 FALSE
#define CH32_PWM_USE_TIM13                 FALSE
#define CH32_PWM_USE_TIM14                 FALSE
#define CH32_PWM_TIM1_IRQ_PRIORITY         6
#define CH32_PWM_TIM2_IRQ_PRIORITY         6
#define CH32_PWM_TIM3_IRQ_PRIORITY         6
#define CH32_PWM_TIM4_IRQ_PRIORITY         6
#define CH32_PWM_TIM5_IRQ_PRIORITY         6
#define CH32_PWM_TIM1_DMA_PRIORITY         1
#define CH32_PWM_TIM2_DMA_PRIORITY         1
#define CH32_PWM_TIM3_DMA_PRIORITY         1
#define CH32_PWM_TIM4_DMA_PRIORITY         1
#define CH32_PWM_TIM5_DMA_PRIORITY         1
#define CH32_PWM_TIM1_UP_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM2_UP_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM3_UP_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM4_UP_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM5_UP_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM1_CH1_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM1_CH2_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM1_CH3_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM1_CH4_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM2_CH1_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM2_CH2_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM2_CH3_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM2_CH4_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM3_CH1_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM3_CH2_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM3_CH3_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM3_CH4_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM4_CH1_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM4_CH2_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM4_CH3_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM4_CH4_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM5_CH1_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM5_CH2_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM5_CH3_DMA_STREAM       CH32_DMA_STREAM_ID_ANY
#define CH32_PWM_TIM5_CH4_DMA_STREAM       CH32_DMA_STREAM_ID_ANY

/*===========================================================================*/
/* ICU driver system settings.                                               */
/* TIM2 is used for the ICU (RC input).                                      */
/*===========================================================================*/
#define CH32_ICU_USE_TIM1                  FALSE
#define CH32_ICU_USE_TIM2                  FALSE
#define CH32_ICU_USE_TIM3                  FALSE
#define CH32_ICU_USE_TIM4                  FALSE
#define CH32_ICU_USE_TIM5                  FALSE
#define CH32_ICU_USE_TIM6                  FALSE
#define CH32_ICU_USE_TIM7                  FALSE
#define CH32_ICU_USE_TIM8                  FALSE
#define CH32_ICU_USE_TIM9                  FALSE
#define CH32_ICU_USE_TIM10                 FALSE
#define CH32_ICU_USE_TIM11                 FALSE
#define CH32_ICU_USE_TIM12                 FALSE
#define CH32_ICU_USE_TIM13                 FALSE
#define CH32_ICU_USE_TIM14                 FALSE
#define CH32_ICU_TIM1_IRQ_PRIORITY         7
#define CH32_ICU_TIM2_IRQ_PRIORITY         7
#define CH32_ICU_TIM3_IRQ_PRIORITY         7
#define CH32_ICU_TIM4_IRQ_PRIORITY         7
#define CH32_ICU_TIM5_IRQ_PRIORITY         7
#define CH32_ICU_TIM6_IRQ_PRIORITY         7
#define CH32_ICU_TIM7_IRQ_PRIORITY         7
#define CH32_ICU_TIM8_IRQ_PRIORITY         7
#define CH32_ICU_TIM9_IRQ_PRIORITY         7
#define CH32_ICU_TIM10_IRQ_PRIORITY        7
#define CH32_ICU_TIM11_IRQ_PRIORITY        7
#define CH32_ICU_TIM12_IRQ_PRIORITY        7
#define CH32_ICU_TIM13_IRQ_PRIORITY        7
#define CH32_ICU_TIM14_IRQ_PRIORITY        7
#define CH32_ICU_TIM1_DMA_STREAM           CH32_DMA_STREAM_ID_ANY
#define CH32_ICU_TIM2_DMA_STREAM           CH32_DMA_STREAM_ID_ANY
#define CH32_ICU_TIM3_DMA_STREAM           CH32_DMA_STREAM_ID_ANY
#define CH32_ICU_TIM4_DMA_STREAM           CH32_DMA_STREAM_ID_ANY
#define CH32_ICU_TIM5_DMA_STREAM           CH32_DMA_STREAM_ID_ANY
#define CH32_ICU_TIM1_DMA_PRIORITY         1
#define CH32_ICU_TIM2_DMA_PRIORITY         1
#define CH32_ICU_TIM3_DMA_PRIORITY         1
#define CH32_ICU_TIM4_DMA_PRIORITY         1
#define CH32_ICU_TIM5_DMA_PRIORITY         1

/*===========================================================================*/
/* I2C driver system settings.                                               */
/*===========================================================================*/
#define CH32_I2C_USE_I2C1                  TRUE
#define CH32_I2C_USE_I2C2                  FALSE
#define CH32_I2C_USE_I2C3                  TRUE
#define CH32_I2C_USE_I2C4                  TRUE
#define CH32_I2C_I2C1_RX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_I2C_I2C1_TX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_I2C_I2C2_RX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_I2C_I2C2_TX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_I2C_I2C3_RX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_I2C_I2C3_TX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_I2C_I2C4_RX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_I2C_I2C4_TX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_I2C_I2C1_DMA_PRIORITY         1
#define CH32_I2C_I2C2_DMA_PRIORITY         1
#define CH32_I2C_I2C3_DMA_PRIORITY         1
#define CH32_I2C_I2C4_DMA_PRIORITY         1
#define CH32_I2C_I2C1_IRQ_PRIORITY         10
#define CH32_I2C_I2C2_IRQ_PRIORITY         10
#define CH32_I2C_I2C3_IRQ_PRIORITY         10
#define CH32_I2C_I2C4_IRQ_PRIORITY         10
#define CH32_I2C_DMA_ERROR_HOOK(i2cp)      osalSysHalt("DMA failure")

/*===========================================================================*/
/* SPI driver system settings.                                               */
/*===========================================================================*/
#define CH32_SPI_USE_SPI1                  TRUE
#define CH32_SPI_USE_SPI2                  FALSE
#define CH32_SPI_USE_SPI3                  FALSE
#define CH32_SPI_USE_SPI4                  TRUE
#define CH32_SPI_SPI1_RX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_SPI_SPI1_TX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_SPI_SPI2_RX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_SPI_SPI2_TX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_SPI_SPI3_RX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_SPI_SPI3_TX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_SPI_SPI4_RX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_SPI_SPI4_TX_DMA_STREAM        CH32_DMA_STREAM_ID_ANY
#define CH32_SPI_SPI1_DMA_PRIORITY         1
#define CH32_SPI_SPI2_DMA_PRIORITY         1
#define CH32_SPI_SPI3_DMA_PRIORITY         1
#define CH32_SPI_SPI4_DMA_PRIORITY         1
#define CH32_SPI_SPI1_IRQ_PRIORITY         10
#define CH32_SPI_SPI2_IRQ_PRIORITY         10
#define CH32_SPI_SPI3_IRQ_PRIORITY         10
#define CH32_SPI_SPI4_IRQ_PRIORITY         10

/*===========================================================================*/
/* SERIAL driver system settings.                                            */
/*===========================================================================*/
/* SIO driver system settings.                                               */
/*===========================================================================*/
#define CH32_SIO_USE_USART1                FALSE
#define CH32_SIO_USE_USART2                FALSE
#define CH32_SIO_USE_USART3                FALSE
#define CH32_SIO_USE_USART4                FALSE
#define CH32_SIO_USE_USART5                FALSE
#define CH32_SIO_USE_USART6                FALSE
#define CH32_SIO_USE_USART7                FALSE
#define CH32_SIO_USE_USART8                FALSE

/*===========================================================================*/
/* UART driver system settings.                                              */
/* The debug UART (USART3) uses the UART API.                                */
/*===========================================================================*/

#define CH32_UART_USART1_IRQ_PRIORITY      8
#define CH32_UART_USART2_IRQ_PRIORITY      8
#define CH32_UART_USART3_IRQ_PRIORITY      8
#define CH32_UART_USART4_IRQ_PRIORITY      8
#define CH32_UART_USART5_IRQ_PRIORITY      8
#define CH32_UART_USART6_IRQ_PRIORITY      8
#define CH32_UART_USART7_IRQ_PRIORITY      8
#define CH32_UART_USART8_IRQ_PRIORITY      8
#define CH32_UART_USART1_RX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART1_TX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART2_RX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART2_TX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART3_RX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART3_TX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART4_RX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART4_TX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART5_RX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART5_TX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART6_RX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART6_TX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART7_RX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART7_TX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART8_RX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART8_TX_DMA_STREAM     CH32_DMA_STREAM_ID_ANY
#define CH32_UART_USART1_DMA_PRIORITY      2
#define CH32_UART_USART2_DMA_PRIORITY      2
#define CH32_UART_USART3_DMA_PRIORITY      2
#define CH32_UART_USART4_DMA_PRIORITY      2
#define CH32_UART_USART5_DMA_PRIORITY      2
#define CH32_UART_USART6_DMA_PRIORITY      2
#define CH32_UART_USART7_DMA_PRIORITY      2
#define CH32_UART_USART8_DMA_PRIORITY      2
#define CH32_UART_DMA_ERROR_HOOK(uartp)    osalSysHalt("UART DMA failure")

/*===========================================================================*/
/* SDC driver system settings.                                               */
/* NOTE: the browser uses the SDIO peripheral instance.                       */
/*===========================================================================*/
#define CH32_SDC_USE_SDC1                  TRUE
#define CH32_SDC_SDIO_CLOCK                100000000
#define CH32_SDC_SDIO_IRQ_PRIORITY         9

/*===========================================================================*/
/* USB (OTG) driver system settings.                                         */
/*===========================================================================*/
#define CH32_OTG_USE_USB1                  TRUE
#define CH32_USB_OTG1_IRQ_PRIORITY         13

/*===========================================================================*/
/* WSPI (QSPI) driver system settings.                                       */
/*===========================================================================*/
#define CH32_WSPI_USE_WSPI1                FALSE
#define CH32_WSPI_USE_WSPI2                TRUE
#define CH32_WSPI_WSPI2_DMA_STREAM         CH32_DMA_STREAM_ID_ANY
#define CH32_WSPI_WSPI2_DMA_PRIORITY       2

/*===========================================================================*/
/* RTC driver system settings.                                               */
/*===========================================================================*/
#define CH32_RTC_USE_RTC1                  FALSE

/*===========================================================================*/
/* RNG driver system settings.                                               */
/*===========================================================================*/
#define HAL_USE_TRNG                       FALSE

/*===========================================================================*/
/* WDG driver system settings.                                               */
/*===========================================================================*/
#define CH32_WDG_USE_WDG1                  TRUE
