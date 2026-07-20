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

/*
 * SPI_CFG2 aliases - CH32 does not have CFG2 register (F4/F7-style only).
 * Define as 0 so SPI mode selection compiles without error.
 */
#ifndef SPI_CFG2_CPHA
#define SPI_CFG2_CPHA                     SPI_CTLR1_CPHA
#endif
#ifndef SPI_CFG2_CPOL
#define SPI_CFG2_CPOL                     SPI_CTLR1_CPOL
#endif

/* SPI clock definitions - CH32H417 uses APB clocks for all SPI peripherals */
#ifndef STM32_SPI1CLK
#define STM32_SPI1CLK                     STM32_PCLK2
#endif
#ifndef STM32_SPI2CLK
#define STM32_SPI2CLK                     STM32_PCLK1
#endif
#ifndef STM32_SPI3CLK
#define STM32_SPI3CLK                     STM32_PCLK1
#endif
#ifndef STM32_SPI4CLK
#define STM32_SPI4CLK                     STM32_PCLK2
#endif
#ifndef STM32_SPI5CLK
#define STM32_SPI5CLK                     STM32_PCLK2
#endif
#ifndef STM32_SPI6CLK
#define STM32_SPI6CLK                     STM32_PCLK2
#endif

/* WSPI clock mode aliases */
#ifndef STM32_DCR_CK_MODE
#define STM32_DCR_CK_MODE                 0
#endif
#ifndef STM32_DCR1_CK_MODE
#define STM32_DCR1_CK_MODE                0
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

/* I2C_ISR_LIMIT - CH32 I2C driver does not have this feature, set to 0 to disable */
#ifndef I2C_ISR_LIMIT
#define I2C_ISR_LIMIT                     0
#endif

/* CH32H417 does not have SDMMC2 - only SDMMC1 */
#ifndef STM32_SDC_USE_SDMMC2
#define STM32_SDC_USE_SDMMC2              FALSE
#endif

/* HAL expected clock values for CH32H417 at 100MHz */
#ifndef HAL_EXPECTED_STM32_SYS_CK
#define HAL_EXPECTED_STM32_SYS_CK         100000000UL
#endif
#ifndef HAL_EXPECTED_STM32_HCLK
#define HAL_EXPECTED_STM32_HCLK           100000000UL
#endif
#ifndef STM32_SYS_CK
#define STM32_SYS_CK                      100000000UL
#endif
#ifndef STM32_SDMMC1CLK
#define STM32_SDMMC1CLK                   100000000UL
#endif
#ifndef STM32_SPI45CLK
#define STM32_SPI45CLK                    STM32_PCLK2
#endif
#ifndef STM32_FDCANCLK
#define STM32_FDCANCLK                    STM32_PCLK1
#endif

/* HAL_SPI_SCK_SAVE_RESTORE - CH32 supports SCK save/restore */
#ifndef HAL_SPI_SCK_SAVE_RESTORE
#define HAL_SPI_SCK_SAVE_RESTORE          1
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
#ifndef STM32_DMAMUX1_USART8_RX
#define STM32_DMAMUX1_USART8_RX           DMA_MUX_USART8_RX
#endif
#ifndef STM32_DMAMUX1_USART8_TX
#define STM32_DMAMUX1_USART8_TX           DMA_MUX_USART8_TX
#endif
#ifndef STM32_DMAMUX1_TIM1_CH1
#define STM32_DMAMUX1_TIM1_CH1            DMA_MUX_TIM1_CH1
#endif
#ifndef STM32_DMAMUX1_TIM1_CH2
#define STM32_DMAMUX1_TIM1_CH2            DMA_MUX_TIM1_CH2
#endif
#ifndef STM32_DMAMUX1_TIM1_CH3
#define STM32_DMAMUX1_TIM1_CH3            DMA_MUX_TIM1_CH3
#endif
#ifndef STM32_DMAMUX1_TIM1_CH4
#define STM32_DMAMUX1_TIM1_CH4            DMA_MUX_TIM1_CH4
#endif
#ifndef STM32_DMAMUX1_TIM2_CH1
#define STM32_DMAMUX1_TIM2_CH1            DMA_MUX_TIM2_CH1
#endif
#ifndef STM32_DMAMUX1_TIM2_UP
#define STM32_DMAMUX1_TIM2_UP             DMA_MUX_TIM2_UP
#endif
#ifndef STM32_DMAMUX1_TIM3_CH1
#define STM32_DMAMUX1_TIM3_CH1            DMA_MUX_TIM3_CH1
#endif
#ifndef STM32_DMAMUX1_TIM3_CH2
#define STM32_DMAMUX1_TIM3_CH2            DMA_MUX_TIM3_CH2
#endif
#ifndef STM32_DMAMUX1_TIM3_CH3
#define STM32_DMAMUX1_TIM3_CH3            DMA_MUX_TIM3_CH3
#endif
#ifndef STM32_DMAMUX1_TIM3_CH4
#define STM32_DMAMUX1_TIM3_CH4            DMA_MUX_TIM3_CH4
#endif
#ifndef STM32_DMAMUX1_TIM3_UP
#define STM32_DMAMUX1_TIM3_UP             DMA_MUX_TIM3_UP
#endif
#ifndef STM32_DMAMUX1_TIM4_CH1
#define STM32_DMAMUX1_TIM4_CH1            DMA_MUX_TIM4_CH1
#endif
#ifndef STM32_DMAMUX1_TIM5_CH1
#define STM32_DMAMUX1_TIM5_CH1            DMA_MUX_TIM5_CH1
#endif
#ifndef STM32_DMAMUX1_SPI2_RX
#define STM32_DMAMUX1_SPI2_RX             DMA_MUX_SPI2_RX
#endif
#ifndef STM32_DMAMUX1_SPI2_TX
#define STM32_DMAMUX1_SPI2_TX             DMA_MUX_SPI2_TX
#endif
#ifndef STM32_DMAMUX1_SPI3_RX
#define STM32_DMAMUX1_SPI3_RX             DMA_MUX_SPI3_RX
#endif
#ifndef STM32_DMAMUX1_SPI3_TX
#define STM32_DMAMUX1_SPI3_TX             DMA_MUX_SPI3_TX
#endif
#ifndef STM32_DMAMUX1_I2C2_RX
#define STM32_DMAMUX1_I2C2_RX             DMA_MUX_I2C2_RX
#endif
#ifndef STM32_DMAMUX1_I2C2_TX
#define STM32_DMAMUX1_I2C2_TX             DMA_MUX_I2C2_TX
#endif

/* CH32-prefixed DMAMUX aliases - hwdef.h generates these names */
#ifndef CH32_DMAMUX1_ADC1
#define CH32_DMAMUX1_ADC1                 DMA_MUX_ADC1
#endif
#ifndef CH32_DMAMUX1_TIM1_UP
#define CH32_DMAMUX1_TIM1_UP              DMA_MUX_TIM1_UP
#endif
#ifndef CH32_DMAMUX1_TIM1_CH1
#define CH32_DMAMUX1_TIM1_CH1             DMA_MUX_TIM1_CH1
#endif
#ifndef CH32_DMAMUX1_TIM1_CH2
#define CH32_DMAMUX1_TIM1_CH2             DMA_MUX_TIM1_CH2
#endif
#ifndef CH32_DMAMUX1_TIM1_CH3
#define CH32_DMAMUX1_TIM1_CH3             DMA_MUX_TIM1_CH3
#endif
#ifndef CH32_DMAMUX1_TIM1_CH4
#define CH32_DMAMUX1_TIM1_CH4             DMA_MUX_TIM1_CH4
#endif
#ifndef CH32_DMAMUX1_TIM2_CH1
#define CH32_DMAMUX1_TIM2_CH1             DMA_MUX_TIM2_CH1
#endif
#ifndef CH32_DMAMUX1_TIM2_UP
#define CH32_DMAMUX1_TIM2_UP              DMA_MUX_TIM2_UP
#endif
#ifndef CH32_DMAMUX1_TIM3_CH1
#define CH32_DMAMUX1_TIM3_CH1             DMA_MUX_TIM3_CH1
#endif
#ifndef CH32_DMAMUX1_TIM3_UP
#define CH32_DMAMUX1_TIM3_UP              DMA_MUX_TIM3_UP
#endif
#ifndef CH32_DMAMUX1_TIM4_UP
#define CH32_DMAMUX1_TIM4_UP              DMA_MUX_TIM4_UP
#endif
#ifndef CH32_DMAMUX1_TIM5_UP
#define CH32_DMAMUX1_TIM5_UP              DMA_MUX_TIM5_UP
#endif
#ifndef CH32_DMAMUX1_SPI1_RX
#define CH32_DMAMUX1_SPI1_RX              DMA_MUX_SPI1_RX
#endif
#ifndef CH32_DMAMUX1_SPI1_TX
#define CH32_DMAMUX1_SPI1_TX              DMA_MUX_SPI1_TX
#endif
#ifndef CH32_DMAMUX1_SPI4_RX
#define CH32_DMAMUX1_SPI4_RX              DMA_MUX_SPI4_RX
#endif
#ifndef CH32_DMAMUX1_SPI4_TX
#define CH32_DMAMUX1_SPI4_TX              DMA_MUX_SPI4_TX
#endif
#ifndef CH32_DMAMUX1_I2C1_RX
#define CH32_DMAMUX1_I2C1_RX              DMA_MUX_I2C1_RX
#endif
#ifndef CH32_DMAMUX1_I2C1_TX
#define CH32_DMAMUX1_I2C1_TX              DMA_MUX_I2C1_TX
#endif
#ifndef CH32_DMAMUX1_I2C3_RX
#define CH32_DMAMUX1_I2C3_RX              DMA_MUX_I2C3_RX
#endif
#ifndef CH32_DMAMUX1_I2C3_TX
#define CH32_DMAMUX1_I2C3_TX              DMA_MUX_I2C3_TX
#endif
#ifndef CH32_DMAMUX1_SDIO
#define CH32_DMAMUX1_SDIO                 DMA_MUX_SDIO
#endif
#ifndef CH32_DMAMUX1_USART1_RX
#define CH32_DMAMUX1_USART1_RX            DMA_MUX_USART1_RX
#endif
#ifndef CH32_DMAMUX1_USART1_TX
#define CH32_DMAMUX1_USART1_TX            DMA_MUX_USART1_TX
#endif
#ifndef CH32_DMAMUX1_USART2_RX
#define CH32_DMAMUX1_USART2_RX            DMA_MUX_USART2_RX
#endif
#ifndef CH32_DMAMUX1_USART2_TX
#define CH32_DMAMUX1_USART2_TX            DMA_MUX_USART2_TX
#endif
#ifndef CH32_DMAMUX1_USART3_RX
#define CH32_DMAMUX1_USART3_RX            DMA_MUX_USART3_RX
#endif
#ifndef CH32_DMAMUX1_USART3_TX
#define CH32_DMAMUX1_USART3_TX            DMA_MUX_USART3_TX
#endif
#ifndef CH32_DMAMUX1_USART4_RX
#define CH32_DMAMUX1_USART4_RX            DMA_MUX_USART4_RX
#endif
#ifndef CH32_DMAMUX1_USART4_TX
#define CH32_DMAMUX1_USART4_TX            DMA_MUX_USART4_TX
#endif
#ifndef CH32_DMAMUX1_USART5_RX
#define CH32_DMAMUX1_USART5_RX            DMA_MUX_USART5_RX
#endif
#ifndef CH32_DMAMUX1_USART5_TX
#define CH32_DMAMUX1_USART5_TX            DMA_MUX_USART5_TX
#endif
#ifndef CH32_DMAMUX1_USART6_RX
#define CH32_DMAMUX1_USART6_RX            DMA_MUX_USART6_RX
#endif
#ifndef CH32_DMAMUX1_USART6_TX
#define CH32_DMAMUX1_USART6_TX            DMA_MUX_USART6_TX
#endif
#ifndef CH32_DMAMUX1_USART7_RX
#define CH32_DMAMUX1_USART7_RX            DMA_MUX_USART7_RX
#endif
#ifndef CH32_DMAMUX1_USART7_TX
#define CH32_DMAMUX1_USART7_TX            DMA_MUX_USART7_TX
#endif
#ifndef CH32_DMAMUX1_USART8_RX
#define CH32_DMAMUX1_USART8_RX            DMA_MUX_USART8_RX
#endif
#ifndef CH32_DMAMUX1_USART8_TX
#define CH32_DMAMUX1_USART8_TX            DMA_MUX_USART8_TX
#endif

/* CH32-prefixed USART DMA config aliases */
#define CH32_USART1_RX_DMA_CONFIG         STM32_USART1_RX_DMA_CONFIG
#define CH32_USART1_TX_DMA_CONFIG         STM32_USART1_TX_DMA_CONFIG
#define CH32_USART2_RX_DMA_CONFIG         STM32_USART2_RX_DMA_CONFIG
#define CH32_USART2_TX_DMA_CONFIG         STM32_USART2_TX_DMA_CONFIG
#define CH32_USART3_RX_DMA_CONFIG         STM32_USART3_RX_DMA_CONFIG
#define CH32_USART3_TX_DMA_CONFIG         STM32_USART3_TX_DMA_CONFIG
#define CH32_USART4_RX_DMA_CONFIG         STM32_USART4_RX_DMA_CONFIG
#define CH32_USART4_TX_DMA_CONFIG         STM32_USART4_TX_DMA_CONFIG
#define CH32_USART5_RX_DMA_CONFIG         STM32_USART5_RX_DMA_CONFIG
#define CH32_USART5_TX_DMA_CONFIG         STM32_USART5_TX_DMA_CONFIG
#define CH32_USART6_RX_DMA_CONFIG         STM32_USART6_RX_DMA_CONFIG
#define CH32_USART6_TX_DMA_CONFIG         STM32_USART6_TX_DMA_CONFIG
#define CH32_USART7_RX_DMA_CONFIG         STM32_USART7_RX_DMA_CONFIG
#define CH32_USART7_TX_DMA_CONFIG         STM32_USART7_TX_DMA_CONFIG
#define CH32_USART8_RX_DMA_CONFIG         STM32_USART8_RX_DMA_CONFIG
#define CH32_USART8_TX_DMA_CONFIG         STM32_USART8_TX_DMA_CONFIG

/* CH32-prefixed I2C DMA config aliases */
#define CH32_I2C1_RX_DMA_CONFIG           STM32_I2C1_RX_DMA_CONFIG
#define CH32_I2C1_TX_DMA_CONFIG           STM32_I2C1_TX_DMA_CONFIG
#define CH32_I2C3_RX_DMA_CONFIG           STM32_I2C3_RX_DMA_CONFIG
#define CH32_I2C3_TX_DMA_CONFIG           STM32_I2C3_TX_DMA_CONFIG
#define CH32_I2C4_RX_DMA_CONFIG           STM32_I2C4_RX_DMA_CONFIG
#define CH32_I2C4_TX_DMA_CONFIG           STM32_I2C4_TX_DMA_CONFIG

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
#ifndef USART_SR_FE
#define USART_SR_FE                       USART_STATR_FE
#endif
#ifndef USART_SR_NE
#define USART_SR_NE                       USART_STATR_NE
#endif
#ifndef USART_SR_ORE
#define USART_SR_ORE                      USART_STATR_ORE
#endif
/* USART data register aliases - CH32 uses DATAR vs STM32's DR/RDR/TDR */
#ifndef USART_DR
#define USART_DR                          DATAR
#endif
#ifndef USART_RDR
#define USART_RDR                         DATAR
#endif
#ifndef USART_TDR
#define USART_TDR                         DATAR
#endif

/*
 * TIM register aliases - CH32 uses different naming for timer register bits.
 * CH32: TIM_CEN, TIM_URS, TIM_UDIS, TIM_ARPE, TIM_UG, TIM_CC1DE, etc.
 * STM32: TIM_CR1_CEN, TIM_CR1_URS, TIM_CR1_UDIS, TIM_CR1_ARPE, TIM_EGR_UG, TIM_DIER_CC1DE, etc.
 */
#ifndef TIM_CR1_CEN
#define TIM_CR1_CEN                       TIM_CEN
#endif
#ifndef TIM_CR1_UDIS
#define TIM_CR1_UDIS                      TIM_UDIS
#endif
#ifndef TIM_CR1_URS
#define TIM_CR1_URS                       TIM_URS
#endif
#ifndef TIM_CR1_OPM
#define TIM_CR1_OPM                       TIM_OPM
#endif
#ifndef TIM_CR1_DIR
#define TIM_CR1_DIR                       TIM_DIR
#endif
#ifndef TIM_CR1_ARPE
#define TIM_CR1_ARPE                      TIM_ARPE
#endif
#ifndef TIM_CR1_CKD
#define TIM_CR1_CKD                       TIM_CTLR1_CKD
#endif
#ifndef TIM_EGR_UG
#define TIM_EGR_UG                        TIM_UG
#endif
#ifndef STM32_TIM_EGR_UG
#define STM32_TIM_EGR_UG                  TIM_UG
#endif
#ifndef TIM_DIER_UDE
#define TIM_DIER_UDE                      TIM_UDE
#endif
#ifndef TIM_DIER_CC1DE
#define TIM_DIER_CC1DE                    TIM_CC1DE
#endif
#ifndef TIM_DIER_CC2DE
#define TIM_DIER_CC2DE                    TIM_CC2DE
#endif
#ifndef TIM_DIER_CC3DE
#define TIM_DIER_CC3DE                    TIM_CC3DE
#endif
#ifndef TIM_DIER_CC4DE
#define TIM_DIER_CC4DE                    TIM_CC4DE
#endif
/* TIM_DIER_IRQ_MASK - combine all interrupt enables */
#ifndef TIM_DIER_IRQ_MASK
#define TIM_DIER_IRQ_MASK                 (TIM_UIE | TIM_CC1IE | TIM_CC2IE | TIM_CC3IE | TIM_CC4IE | TIM_COMIE | TIM_TIE | TIM_BIE)
#endif
/* TIM CCER bits - CH32 uses same names (TIM_CC1E, etc.) */
#ifndef TIM_CCER_CC1E
#define TIM_CCER_CC1E                     TIM_CC1E
#endif
#ifndef TIM_CCER_CC1P
#define TIM_CCER_CC1P                     TIM_CC1P
#endif
#ifndef TIM_CCER_CC1NE
#define TIM_CCER_CC1NE                    TIM_CC1NE
#endif
#ifndef TIM_CCER_CC1NP
#define TIM_CCER_CC1NP                    TIM_CC1NP
#endif
#ifndef TIM_CCER_CC2E
#define TIM_CCER_CC2E                     TIM_CC2E
#endif
#ifndef TIM_CCER_CC2P
#define TIM_CCER_CC2P                     TIM_CC2P
#endif
#ifndef TIM_CCER_CC2NE
#define TIM_CCER_CC2NE                    TIM_CC2NE
#endif
#ifndef TIM_CCER_CC2NP
#define TIM_CCER_CC2NP                    TIM_CC2NP
#endif
#ifndef TIM_CCER_CC3E
#define TIM_CCER_CC3E                     TIM_CC3E
#endif
#ifndef TIM_CCER_CC3P
#define TIM_CCER_CC3P                     TIM_CC3P
#endif
#ifndef TIM_CCER_CC3NE
#define TIM_CCER_CC3NE                    TIM_CC3NE
#endif
#ifndef TIM_CCER_CC3NP
#define TIM_CCER_CC3NP                    TIM_CC3NP
#endif
#ifndef TIM_CCER_CC4E
#define TIM_CCER_CC4E                     TIM_CC4E
#endif
#ifndef TIM_CCER_CC4P
#define TIM_CCER_CC4P                     TIM_CC4P
#endif
/* TIM CCMR1 bits - CH32 uses CHCTLR1 naming */
#ifndef TIM_CCMR1_CC1S
#define TIM_CCMR1_CC1S                    TIM_CC1S
#endif
#ifndef TIM_CCMR1_OC1FE
#define TIM_CCMR1_OC1FE                   TIM_OC1FE
#endif
#ifndef TIM_CCMR1_OC1PE
#define TIM_CCMR1_OC1PE                   TIM_OC1PE
#endif
#ifndef TIM_CCMR1_OC1M
#define TIM_CCMR1_OC1M                    TIM_OC1M
#endif
#ifndef TIM_CCMR1_OC1M_0
#define TIM_CCMR1_OC1M_0                  TIM_OC1M_0
#endif
#ifndef TIM_CCMR1_OC1M_1
#define TIM_CCMR1_OC1M_1                  TIM_OC1M_1
#endif
#ifndef TIM_CCMR1_OC1M_2
#define TIM_CCMR1_OC1M_2                  TIM_OC1M_2
#endif
#ifndef TIM_CCMR1_IC1PSC
#define TIM_CCMR1_IC1PSC                  TIM_IC1PSC
#endif
#ifndef TIM_CCMR1_IC1F
#define TIM_CCMR1_IC1F                    TIM_IC1F
#endif
#ifndef TIM_CCMR1_IC1F_0
#define TIM_CCMR1_IC1F_0                  TIM_IC1F_0
#endif
#ifndef TIM_CCMR1_IC1F_1
#define TIM_CCMR1_IC1F_1                  TIM_IC1F_1
#endif
#ifndef TIM_CCMR1_IC1F_2
#define TIM_CCMR1_IC1F_2                  TIM_IC1F_2
#endif
#ifndef TIM_CCMR1_IC1F_3
#define TIM_CCMR1_IC1F_3                  TIM_IC1F_3
#endif
#ifndef TIM_CCMR1_CC2S
#define TIM_CCMR1_CC2S                    TIM_CC2S
#endif
#ifndef TIM_CCMR1_OC2FE
#define TIM_CCMR1_OC2FE                   TIM_OC2FE
#endif
#ifndef TIM_CCMR1_OC2PE
#define TIM_CCMR1_OC2PE                   TIM_OC2PE
#endif
#ifndef TIM_CCMR1_OC2M
#define TIM_CCMR1_OC2M                    TIM_OC2M
#endif
#ifndef TIM_CCMR1_IC2F
#define TIM_CCMR1_IC2F                    TIM_IC2F
#endif
/* TIM CCMR2 bits */
#ifndef TIM_CCMR2_CC3S
#define TIM_CCMR2_CC3S                    TIM_CC3S
#endif
#ifndef TIM_CCMR2_OC3FE
#define TIM_CCMR2_OC3FE                   TIM_OC3FE
#endif
#ifndef TIM_CCMR2_OC3PE
#define TIM_CCMR2_OC3PE                   TIM_OC3PE
#endif
#ifndef TIM_CCMR2_OC3M
#define TIM_CCMR2_OC3M                    TIM_OC3M
#endif
#ifndef TIM_CCMR2_OC3M_0
#define TIM_CCMR2_OC3M_0                  TIM_OC3M_0
#endif
#ifndef TIM_CCMR2_OC3M_1
#define TIM_CCMR2_OC3M_1                  TIM_OC3M_1
#endif
#ifndef TIM_CCMR2_OC3M_2
#define TIM_CCMR2_OC3M_2                  TIM_OC3M_2
#endif
#ifndef TIM_CCMR2_CC4S
#define TIM_CCMR2_CC4S                    TIM_CC4S
#endif
#ifndef TIM_CCMR2_OC4PE
#define TIM_CCMR2_OC4PE                   TIM_OC4PE
#endif
#ifndef TIM_CCMR2_OC4M
#define TIM_CCMR2_OC4M                    TIM_OC4M
#endif
/* TIM BDTR bits */
#ifndef TIM_BDTR_MOE
#define TIM_BDTR_MOE                      TIM_MOE
#endif
#ifndef TIM_BDTR_AOE
#define TIM_BDTR_AOE                      TIM_AOE
#endif
#ifndef TIM_BDTR_BKE
#define TIM_BDTR_BKE                      TIM_BKE
#endif
#ifndef TIM_BDTR_BKP
#define TIM_BDTR_BKP                      TIM_BKP
#endif
/* TIM SR (INTFR) bits */
#ifndef TIM_SR_CC1OF
#define TIM_SR_CC1OF                      TIM_CC1OF
#endif
#ifndef TIM_SR_CC2OF
#define TIM_SR_CC2OF                      TIM_CC2OF
#endif
#ifndef TIM_SR_CC3OF
#define TIM_SR_CC3OF                      TIM_CC3OF
#endif
#ifndef TIM_SR_CC4OF
#define TIM_SR_CC4OF                      TIM_CC4OF
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
#ifndef PAL_MODE_STM32_ALTERNATE_PUSHPULL
#define PAL_MODE_STM32_ALTERNATE_PUSHPULL PAL_CH32_ALTERNATE_PUSHPULL(0)
#endif

/* DMA_SxCR_TRBUFF - CH32 DMA does not have transfer buffer support */
#ifndef DMA_SxCR_TRBUFF
#define DMA_SxCR_TRBUFF                   0
#endif

/* STM32_DMA_ISR_DMEIF - CH32 DMA does not have direct mode error */
#ifndef STM32_DMA_ISR_DMEIF
#define STM32_DMA_ISR_DMEIF               0
#endif

/* STM32_ADC_DUAL_MODE - CH32H417 does not support ADC dual mode */
#ifndef STM32_ADC_DUAL_MODE
#define STM32_ADC_DUAL_MODE               0
#endif

/* IWDG base address for CH32H417 - different from STM32 */
#ifndef IWDG_BASE
#define IWDG_BASE                         ((uint32_t)0x40003000)
#endif

/* CORTEX_MAX_KERNEL_PRIORITY - RISC-V does not have this, define as safe default */
#ifndef CORTEX_MAX_KERNEL_PRIORITY
#define CORTEX_MAX_KERNEL_PRIORITY        15
#endif

/* PWMDriver clock member - CH32 does not have this, use APB2 clock */
#ifdef WCH
#define pwm_driver_clock(drv)             STM32_PCLK2
#else
#define pwm_driver_clock(drv)             ((drv)->clock)
#endif

/* System clock definitions for AP_HAL */
#ifndef STM32_HCLK
#define STM32_HCLK                        100000000UL
#endif

/* GPIO type alias for CH32H417 */
#ifndef stm32_gpio_t
#define stm32_gpio_t GPIO_TypeDef
#endif

/* stkalign_t - CH32 ChibiOS port uses stkline_t instead */
#ifndef stkalign_t
#define stkalign_t stkline_t
#endif