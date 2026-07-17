/*
 * CH32H417 STM32 compatibility macros for RISC-V builds.
 * Safe to include from mcuconf.h (no dependency on HAL types).
 */
#pragma once

/*
 * STM32 compatibility defines for CH32 RISC-V builds.
 * (Simple macros PCLK, SPI_CR1_*, AP_* are in ch32_compat.h)
 */
#ifndef STM32_DMA_STREAM_ID
#define STM32_DMA_STREAM_ID(dma, stream)  CH32_DMA_STREAM_ID(dma, stream)
#endif
#ifndef STM32_DMA_STREAM_ID_ANY
#define STM32_DMA_STREAM_ID_ANY           CH32_DMA_STREAM_ID_ANY
#endif
#if defined(CH32_DMA_STREAM_ID_ANY_DMA1) && !defined(STM32_DMA_STREAM_ID_ANY_DMA1)
#define STM32_DMA_STREAM_ID_ANY_DMA1      CH32_DMA_STREAM_ID_ANY_DMA1
#endif
#if defined(CH32_DMA_STREAM_ID_ANY_DMA2) && !defined(STM32_DMA_STREAM_ID_ANY_DMA2)
#define STM32_DMA_STREAM_ID_ANY_DMA2      CH32_DMA_STREAM_ID_ANY_DMA2
#endif

#ifndef STM32_DMA_SUPPORTS_DMAMUX
#define STM32_DMA_SUPPORTS_DMAMUX         CH32_DMA_SUPPORTS_DMAMUX
#endif

#ifndef STM32_DMA_ADVANCED
#define STM32_DMA_ADVANCED                CH32_DMA_ADVANCED
#endif

#ifndef stm32_dma_stream_t
#define stm32_dma_stream_t                ch32_dma_stream_t
#endif

/* Timer type alias - CH32 uses TIM_TypeDef instead of stm32_tim_t */
#ifndef stm32_tim_t
#define stm32_tim_t                       TIM_TypeDef
#endif

/* DMA ISR callback type alias */
#ifndef stm32_dmaisr_t
#define stm32_dmaisr_t                    ch32_dmaisr_t
#endif

/*
 * Peripheral clock frequencies.
 * CH32H417 at 100MHz SYSCLK:
 *   APB1 (PCLK1) = SYSCLK / 2 = 50MHz
 *   APB2 (PCLK2) = SYSCLK     = 100MHz
 */
#ifndef STM32_PCLK1
#define STM32_PCLK1                       50000000
#endif
#ifndef STM32_PCLK2
#define STM32_PCLK2                       100000000
#endif

/*
 * SPI register compatibility - WCH uses CTLR1 naming vs STM32's CR1
 * SPI_CTLR1_* macros are defined in CH32 SPI LLD header (included via hal.h)
 * but these aliases are safe to define unconditionally.
 */
#ifndef SPI_CR1_CPOL
#define SPI_CR1_CPOL                      SPI_CTLR1_CPOL
#endif
#ifndef SPI_CR1_CPHA
#define SPI_CR1_CPHA                      SPI_CTLR1_CPHA
#endif
#ifndef SPI_CR1_MSTR
#define SPI_CR1_MSTR                      SPI_CTLR1_MSTR
#endif
#ifndef SPI_CR1_SSM
#define SPI_CR1_SSM                       SPI_CTLR1_SSM
#endif
#ifndef SPI_CR1_SSI
#define SPI_CR1_SSI                       SPI_CTLR1_SSI
#endif
#ifndef SPI_CR1_SPE
#define SPI_CR1_SPE                       SPI_CTLR1_SPE
#endif
#ifndef SPI_CR1_DFF
#define SPI_CR1_DFF                       SPI_CTLR1_DFF
#endif
#ifndef SPI_CR1_BIDIMODE
#define SPI_CR1_BIDIMODE                  SPI_CTLR1_BIDIMODE
#endif
#ifndef SPI_CR1_BR_0
#define SPI_CR1_BR_0                      SPI_CTLR1_BR_0
#endif
#ifndef SPI_CR1_BR_1
#define SPI_CR1_BR_1                      SPI_CTLR1_BR_1
#endif
#ifndef SPI_CR1_BR_2
#define SPI_CR1_BR_2                      SPI_CTLR1_BR_2
#endif
#ifndef SPI_CR1_BR_3
#define SPI_CR1_BR_3                      SPI_CTLR1_BR_3
#endif
#ifndef SPI_CR1_BR_4
#define SPI_CR1_BR_4                      SPI_CTLR1_BR_4
#endif
#ifndef SPI_CR1_BR_5
#define SPI_CR1_BR_5                      SPI_CTLR1_BR_5
#endif
#ifndef SPI_CR1_BR_6
#define SPI_CR1_BR_6                      SPI_CTLR1_BR_6
#endif
#ifndef SPI_CR1_BR_7
#define SPI_CR1_BR_7                      SPI_CTLR1_BR_7
#endif

/* Common AP defines (no HAL dependency) */
#ifndef AP_WATCHDOG_SAVE_FAULT_ENABLED
#if defined(__riscv)
#define AP_WATCHDOG_SAVE_FAULT_ENABLED 0
#else
#define AP_WATCHDOG_SAVE_FAULT_ENABLED 1
#endif
#endif

#ifndef AP_FASTBOOT_ENABLED
#define AP_FASTBOOT_ENABLED 1
#endif

#ifndef CC_NO_INLINE
#define CC_NO_INLINE __attribute__((always_inline))
#endif

/* I2C_ISR_LIMIT - CH32 I2C driver does not have this feature, set to 0 to disable */
#ifndef I2C_ISR_LIMIT
#define I2C_ISR_LIMIT                     0
#endif

/* DMAMUX alias - CH32 uses DMA_MUX_* enum instead of STM32_DMAMUX1_* */
#ifndef STM32_DMAMUX1_ADC1
#define STM32_DMAMUX1_ADC1                DMA_MUX_ADC1
#endif
#ifndef STM32_DMAMUX1_I2C1_RX
#define STM32_DMAMUX1_I2C1_RX             DMA_MUX_I2C1_RX
#endif
#ifndef STM32_DMAMUX1_I2C1_TX
#define STM32_DMAMUX1_I2C1_TX             DMA_MUX_I2C1_TX
#endif
#ifndef STM32_DMAMUX1_I2C3_RX
#define STM32_DMAMUX1_I2C3_RX             DMA_MUX_I2C3_RX
#endif
#ifndef STM32_DMAMUX1_I2C3_TX
#define STM32_DMAMUX1_I2C3_TX             DMA_MUX_I2C3_TX
#endif
#ifndef STM32_DMAMUX1_I2C4_RX
#define STM32_DMAMUX1_I2C4_RX             DMA_MUX_I2C4_RX
#endif
#ifndef STM32_DMAMUX1_I2C4_TX
#define STM32_DMAMUX1_I2C4_TX             DMA_MUX_I2C4_TX
#endif
#ifndef STM32_DMAMUX1_SDIO
#define STM32_DMAMUX1_SDIO                DMA_MUX_SDIO
#endif
#ifndef STM32_DMAMUX1_SPI1_RX
#define STM32_DMAMUX1_SPI1_RX             DMA_MUX_SPI1_RX
#endif
#ifndef STM32_DMAMUX1_SPI1_TX
#define STM32_DMAMUX1_SPI1_TX             DMA_MUX_SPI1_TX
#endif
#ifndef STM32_DMAMUX1_SPI4_RX
#define STM32_DMAMUX1_SPI4_RX             DMA_MUX_SPI4_RX
#endif
#ifndef STM32_DMAMUX1_SPI4_TX
#define STM32_DMAMUX1_SPI4_TX             DMA_MUX_SPI4_TX
#endif
#ifndef STM32_DMAMUX1_TIM1_UP
#define STM32_DMAMUX1_TIM1_UP             DMA_MUX_TIM1_UP
#endif
#ifndef STM32_DMAMUX1_TIM4_UP
#define STM32_DMAMUX1_TIM4_UP             DMA_MUX_TIM4_UP
#endif
#ifndef STM32_DMAMUX1_TIM5_UP
#define STM32_DMAMUX1_TIM5_UP             DMA_MUX_TIM5_UP
#endif
#ifndef STM32_DMAMUX1_USART1_RX
#define STM32_DMAMUX1_USART1_RX           DMA_MUX_USART1_RX
#endif
#ifndef STM32_DMAMUX1_USART1_TX
#define STM32_DMAMUX1_USART1_TX           DMA_MUX_USART1_TX
#endif
#ifndef STM32_DMAMUX1_USART2_RX
#define STM32_DMAMUX1_USART2_RX           DMA_MUX_USART2_RX
#endif
#ifndef STM32_DMAMUX1_USART2_TX
#define STM32_DMAMUX1_USART2_TX           DMA_MUX_USART2_TX
#endif
#ifndef STM32_DMAMUX1_USART3_RX
#define STM32_DMAMUX1_USART3_RX           DMA_MUX_USART3_RX
#endif
#ifndef STM32_DMAMUX1_USART3_TX
#define STM32_DMAMUX1_USART3_TX           DMA_MUX_USART3_TX
#endif
#ifndef STM32_DMAMUX1_USART4_RX
#define STM32_DMAMUX1_USART4_RX           DMA_MUX_USART4_RX
#endif
#ifndef STM32_DMAMUX1_USART4_TX
#define STM32_DMAMUX1_USART4_TX           DMA_MUX_USART4_TX
#endif
#ifndef STM32_DMAMUX1_USART5_RX
#define STM32_DMAMUX1_USART5_RX           DMA_MUX_USART5_RX
#endif
#ifndef STM32_DMAMUX1_USART5_TX
#define STM32_DMAMUX1_USART5_TX           DMA_MUX_USART5_TX
#endif
#ifndef STM32_DMAMUX1_USART6_RX
#define STM32_DMAMUX1_USART6_RX           DMA_MUX_USART6_RX
#endif
#ifndef STM32_DMAMUX1_USART6_TX
#define STM32_DMAMUX1_USART6_TX           DMA_MUX_USART6_TX
#endif
#ifndef STM32_DMAMUX1_USART7_RX
#define STM32_DMAMUX1_USART7_RX           DMA_MUX_USART7_RX
#endif
#ifndef STM32_DMAMUX1_USART7_TX
#define STM32_DMAMUX1_USART7_TX           DMA_MUX_USART7_TX
#endif

/* USART register aliases - CH32 uses STATR/DATAR vs STM32's SR/DR */
/* USART CR bits */
#ifndef USART_CR1_UE
#define USART_CR1_UE                      USART_CTLR1_UE
#endif
#ifndef USART_CR1_RE
#define USART_CR1_RE                      USART_CTLR1_RE
#endif
#ifndef USART_CR1_TE
#define USART_CR1_TE                      USART_CTLR1_TE
#endif
#ifndef USART_CR1_IDLEIE
#define USART_CR1_IDLEIE                  USART_CTLR1_IDLEIE
#endif
#ifndef USART_CR1_RXNEIE
#define USART_CR1_RXNEIE                  USART_CTLR1_RXNEIE
#endif
#ifndef USART_CR1_TCIE
#define USART_CR1_TCIE                    USART_CTLR1_TCIE
#endif
#ifndef USART_CR1_TXEIE
#define USART_CR1_TXEIE                   USART_CTLR1_TXEIE
#endif
#ifndef USART_CR1_PS
#define USART_CR1_PS                      USART_CTLR1_PS
#endif
#ifndef USART_CR1_PCE
#define USART_CR1_PCE                     USART_CTLR1_PCE
#endif
#ifndef USART_CR1_M
#define USART_CR1_M                       USART_CTLR1_M
#endif
#ifndef USART_CR1_M0
#define USART_CR1_M0                      0
#endif
#ifndef USART_CR1_OVER8
#define USART_CR1_OVER8                   0
#endif
#ifndef USART_CR1_FIFOEN
#define USART_CR1_FIFOEN                  0
#endif
#ifndef USART_CR1_RXNEIE_RXFNEIE
#define USART_CR1_RXNEIE_RXFNEIE          USART_CTLR1_RXNEIE
#endif
/* USART CR2 bits */
#ifndef USART_CR2_STOP1_BITS
#define USART_CR2_STOP1_BITS              USART_CTLR2_STOP_0
#endif
#ifndef USART_CR2_STOP2_BITS
#define USART_CR2_STOP2_BITS              USART_CTLR2_STOP_1
#endif
#ifndef USART_CR2_LINEN
#define USART_CR2_LINEN                   USART_CTLR2_LINEN
#endif
#ifndef USART_CR2_SWAP
#define USART_CR2_SWAP                    0
#endif
#ifndef USART_CR2_RXINV
#define USART_CR2_RXINV                   0
#endif
#ifndef USART_CR2_TXINV
#define USART_CR2_TXINV                   0
#endif
/* USART CR3 bits */
#ifndef USART_CR3_DMAR
#define USART_CR3_DMAR                    USART_CTLR3_DMAR
#endif
#ifndef USART_CR3_DMAT
#define USART_CR3_DMAT                    USART_CTLR3_DMAT
#endif
#ifndef USART_CR3_CTSE
#define USART_CR3_CTSE                    USART_CTLR3_CTSE
#endif
#ifndef USART_CR3_RTSE
#define USART_CR3_RTSE                    USART_CTLR3_RTSE
#endif
#ifndef USART_CR3_HDSEL
#define USART_CR3_HDSEL                   USART_CTLR3_HDSEL
#endif
#ifndef USART_CR3_ONEBIT
#define USART_CR3_ONEBIT                  0
#endif
#ifndef USART_CR3_DEM
#define USART_CR3_DEM                     0
#endif
/* USART SR bits */
#ifndef USART_SR_IDLE
#define USART_SR_IDLE                     USART_STATR_IDLE
#endif
#ifndef USART_SR_TC
#define USART_SR_TC                       USART_STATR_TC
#endif
#ifndef USART_SR_RXNE
#define USART_SR_RXNE                     USART_STATR_RXNE
#endif
#ifndef USART_SR_TXE
#define USART_SR_TXE                      USART_STATR_TXE
#endif
#ifndef USART_SR_PE
#define USART_SR_PE                       USART_STATR_PE
#endif

/* TIM register aliases */
#ifndef TIM_DIER_UDE
#define TIM_DIER_UDE                      TIM_UDE
#endif

/* TIM_DMAR - CH32 uses DMAADR instead of DMAR for the same register */
#ifndef DMAR
#define DMAR                              DMAADR
#endif

/* CH32 TIM_TypeDef uses CHxCVR instead of CCRx */
#ifndef CCR
#define CCR                               CH1CVR
#endif
#ifndef CCR1
#define CCR1                              CH1CVR
#endif
#ifndef CCR2
#define CCR2                              CH2CVR
#endif
#ifndef CCR3
#define CCR3                              CH3CVR
#endif
#ifndef CCR4
#define CCR4                              CH4CVR
#endif

/* CH32 TIM_TypeDef uses DMACFGR instead of DCR */
#ifndef DCR
#define DCR                               DMACFGR
#endif

/* TIM DCR register bit macros - same format as STM32 */
#ifndef STM32_TIM_DCR_DBA
#define STM32_TIM_DCR_DBA(n)              ((n) << 0)
#endif
#ifndef STM32_TIM_DCR_DBL
#define STM32_TIM_DCR_DBL(n)              ((n) << 8)
#endif

/*
 * DMA CR register flags - CH32 uses completely different register layout.
 * These STM32 flags are not applicable; dmaStreamSetMode handles CH32 internally.
 */
#ifndef STM32_DMA_CR_CHSEL
#define STM32_DMA_CR_CHSEL(n)            0
#endif
#ifndef STM32_DMA_CR_DIR_M2P
#define STM32_DMA_CR_DIR_M2P             0
#endif
#ifndef STM32_DMA_CR_PSIZE_HWORD
#define STM32_DMA_CR_PSIZE_HWORD         0
#endif
#ifndef STM32_DMA_CR_MSIZE_HWORD
#define STM32_DMA_CR_MSIZE_HWORD         0
#endif
#ifndef STM32_DMA_CR_PSIZE_WORD
#define STM32_DMA_CR_PSIZE_WORD          0
#endif
#ifndef STM32_DMA_CR_MSIZE_BYTE
#define STM32_DMA_CR_MSIZE_BYTE          0
#endif
#ifndef STM32_DMA_CR_MSIZE_WORD
#define STM32_DMA_CR_MSIZE_WORD          0
#endif
#ifndef STM32_DMA_CR_MINC
#define STM32_DMA_CR_MINC                0
#endif
#ifndef STM32_DMA_CR_PL
#define STM32_DMA_CR_PL(n)               0
#endif
#ifndef STM32_DMA_CR_TEIE
#define STM32_DMA_CR_TEIE                0
#endif
#ifndef STM32_DMA_CR_DIR_P2M
#define STM32_DMA_CR_DIR_P2M             0
#endif
#ifndef STM32_DMA_CR_EN
#define STM32_DMA_CR_EN                  0
#endif
#ifndef STM32_DMA_ISR_TEIF
#define STM32_DMA_ISR_TEIF               0
#endif
#ifndef STM32_DMA_CR_TCIE
#define STM32_DMA_CR_TCIE                0
#endif
#ifndef STM32_DMA_CR_DMEIE
#define STM32_DMA_CR_DMEIE               0
#endif
#ifndef STM32_DMA_FCR_DMDIS
#define STM32_DMA_FCR_DMDIS              0
#endif
#ifndef STM32_DMA_FCR_FTH_FULL
#define STM32_DMA_FCR_FTH_FULL           0
#endif

/* PAL mode aliases - CH32 uses PAL_MODE_INPUT etc. directly */
#ifndef PAL_STM32_MODE_INPUT
#define PAL_STM32_MODE_INPUT              PAL_MODE_INPUT
#endif
#ifndef PAL_STM32_OTYPE_PUSHPULL
#define PAL_STM32_OTYPE_PUSHPULL           0
#endif
#ifndef PAL_STM32_PUPDR_PULLUP
#define PAL_STM32_PUPDR_PULLUP             0
#endif
#ifndef PAL_STM32_OSPEED_LOW
#define PAL_STM32_OSPEED_LOW               0
#endif
#ifndef PAL_STM32_OSPEED_LOWEST
#define PAL_STM32_OSPEED_LOWEST            0
#endif
#ifndef PAL_STM32_OSPEED_MEDIUM
#define PAL_STM32_OSPEED_MEDIUM            0
#endif
#ifndef PAL_STM32_OSPEED_MID1
#define PAL_STM32_OSPEED_MID1              0
#endif
#ifndef PAL_STM32_HIGH
#define PAL_STM32_HIGH                     PAL_HIGH
#endif

/* PAL_MODE_ALTERNATE for CH32 - maps to CH32 alternate push-pull mode */
#ifndef PAL_MODE_ALTERNATE
#define PAL_MODE_ALTERNATE(n)             PAL_CH32_ALTERNATE_PUSHPULL(n)
#endif

/* System clock definitions for AP_HAL */
#ifndef STM32_HCLK
#define STM32_HCLK                        100000000UL
#endif

/* USART register aliases - CRn to CTLRn */
#ifndef USART_CR1_UE
#define USART_CR1_UE                      USART_CTLR1_UE
#endif
#ifndef USART_CR1_IDLEIE
#define USART_CR1_IDLEIE                  USART_CTLR1_IDLEIE
#endif
#ifndef USART_CR1_RXNEIE
#define USART_CR1_RXNEIE                  USART_CTLR1_RXNEIE
#endif
#ifndef USART_CR1_PS
#define USART_CR1_PS                      USART_CTLR1_PS
#endif
#ifndef USART_CR1_PCE
#define USART_CR1_PCE                     USART_CTLR1_PCE
#endif
#ifndef USART_CR1_M
#define USART_CR1_M                       USART_CTLR1_M
#endif
#ifndef USART_CR1_OVER8
#define USART_CR1_OVER8                   0
#endif
#ifndef USART_CR1_FIFOEN
#define USART_CR1_FIFOEN                  0
#endif
#ifndef USART_CR1_M0
#define USART_CR1_M0                      USART_CTLR1_M
#endif
#ifndef USART_CR1_RXNEIE_RXFNEIE
#define USART_CR1_RXNEIE_RXFNEIE          USART_CTLR1_RXNEIE
#endif
#ifndef USART_CR2_STOP1_BITS
#define USART_CR2_STOP1_BITS              USART_CTLR2_STOP_0
#endif
#ifndef USART_CR2_STOP2_BITS
#define USART_CR2_STOP2_BITS              USART_CTLR2_STOP_1
#endif
#ifndef USART_CR2_RXINV
#define USART_CR2_RXINV                   0
#endif
#ifndef USART_CR2_TXINV
#define USART_CR2_TXINV                   0
#endif
#ifndef USART_CR2_SWAP
#define USART_CR2_SWAP                    0
#endif
#ifndef USART_CR3_HDSEL
#define USART_CR3_HDSEL                   USART_CTLR3_HDSEL
#endif
#ifndef USART_CR3_DMAR
#define USART_CR3_DMAR                    USART_CTLR3_DMAR
#endif
#ifndef USART_CR3_DMAT
#define USART_CR3_DMAT                    USART_CTLR3_DMAT
#endif
#ifndef USART_CR3_RTSE
#define USART_CR3_RTSE                    USART_CTLR3_RTSE
#endif
#ifndef USART_CR3_CTSE
#define USART_CR3_CTSE                    USART_CTLR3_CTSE
#endif
#ifndef USART_CR3_ONEBIT
#define USART_CR3_ONEBIT                  0
#endif
#ifndef USART_CR3_DEM
#define USART_CR3_DEM                     0
#endif