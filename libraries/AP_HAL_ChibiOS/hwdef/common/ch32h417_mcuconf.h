/*
 * mcuconf.h for the WCH CH32H417 (QingKe V5F) port.
 *
 * Switch names follow the adapt-417 ChibiOS port (os/hal/ports/WCH/LLD
 * driver headers).
 * The defaults here match the CCHv6c pin/peripheral layout:
 *   SERIAL_ORDER OTG1 USART7 USART5 USART1 USART4 USART2
 *   I2C1 / I2C3 / I2C4, SPI1 / SPI4, TIM1/3/4/5 PWM,
 *   CAN1 / CAN3, SDIO, ADC1, USBFS, QSPI2 (external XIP flash).
 *
 * NOTE: hwdef currently emits STM32-style peripheral enable macros, so
 * this file carries the CH32H417_* definitions for now.  Mapping the
 * generated hwdef.h values onto the CH32H417_* names is part of the
 * remaining incremental HAL porting work.
 */

#pragma once

/*
 * The CDC class request codes that WCH's ch32h417_usb.h collides with
 * ChibiOS' hal_usb_cdc.h are handled in ch32_stm32_compat.h.  Do not undef
 * them here: this file is reached from halconf.h, which may be processed
 * before chcore.h pulls in ch32h417_usb.h, and undef'ing in that order
 * defeats the vendor header's #ifndef guard and re-creates the clash.
 */

#ifndef CH32H417_MCUCONF
#define CH32H417_MCUCONF
#endif

/* System timer: the adapt-417 hal_st_lld only supports TIM2..TIM5.
   TIM2 is free on this board (PWM uses TIM1/3/4/5). */
#ifndef CH32H417_ST_USE_TIMER
#define CH32H417_ST_USE_TIMER              2
#endif

/* ---- serial / UART units ---- */
#define CH32H417_SERIAL_USE_USART1             TRUE
#define CH32H417_SERIAL_USE_USART2             TRUE
#define CH32H417_SERIAL_USE_USART3             TRUE
#define CH32H417_SERIAL_USE_USART4             TRUE
#define CH32H417_SERIAL_USE_USART5             TRUE
#define CH32H417_SERIAL_USE_USART6             FALSE
#define CH32H417_SERIAL_USE_USART7             TRUE
#define CH32H417_SERIAL_USE_USART8             FALSE

/* A unit can only belong to one of SERIAL / SIO / UART. */
#define CH32H417_SIO_USE_USART1                FALSE
#define CH32H417_SIO_USE_USART2                FALSE
#define CH32H417_SIO_USE_USART3                FALSE
#define CH32H417_SIO_USE_USART4                FALSE
#define CH32H417_SIO_USE_USART5                FALSE
#define CH32H417_SIO_USE_USART6                FALSE
#define CH32H417_SIO_USE_USART7                FALSE
#define CH32H417_SIO_USE_USART8                FALSE

#define CH32H417_UART_USE_USART1               FALSE
#define CH32H417_UART_USE_USART2               FALSE
#define CH32H417_UART_USE_USART3               FALSE
#define CH32H417_UART_USE_USART4               FALSE
#define CH32H417_UART_USE_USART5               FALSE
#define CH32H417_UART_USE_USART6               FALSE
#define CH32H417_UART_USE_USART7               FALSE
#define CH32H417_UART_USE_USART8               FALSE

/* ---- buses used by the board ---- */
#define CH32H417_SPI_USE_SPI1                  TRUE
#define CH32H417_SPI_USE_SPI2                  FALSE
#define CH32H417_SPI_USE_SPI3                  FALSE
#define CH32H417_SPI_USE_SPI4                  TRUE

#define CH32H417_I2C_USE_I2C1                  TRUE
#define CH32H417_I2C_USE_I2C2                  FALSE
#define CH32H417_I2C_USE_I2C3                  TRUE
#define CH32H417_I2C_USE_I2C4                  TRUE

#define CH32H417_PWM_USE_TIM1                  TRUE
#define CH32H417_PWM_USE_TIM2                  FALSE
#define CH32H417_PWM_USE_TIM3                  TRUE
#define CH32H417_PWM_USE_TIM4                  TRUE
#define CH32H417_PWM_USE_TIM5                  TRUE
#define CH32H417_PWM_USE_TIM8                  FALSE
#define CH32H417_PWM_USE_TIM9                  FALSE
#define CH32H417_PWM_USE_TIM10                 FALSE
#define CH32H417_PWM_USE_TIM11                 FALSE
#define CH32H417_PWM_USE_TIM12                 FALSE

#define CH32H417_GPT_USE_TIM7                  FALSE
#define CH32H417_GPT_USE_TIM8                  FALSE
#define CH32H417_ICU_USE_TIM3                  FALSE

#define CH32H417_CAN_USE_CAN1                  TRUE
#define CH32H417_CAN_USE_CAN2                  FALSE
#define CH32H417_CAN_USE_CAN3                  TRUE

/* ---- storage / analog / connectivity ---- */
#define CH32H417_SDC_USE_SDIO                  TRUE
#define CH32H417_SDC_USE_SDMMC                 FALSE
#define CH32H417_ADC_USE_ADC1                  TRUE
#define CH32H417_ADC_USE_ADC2                  FALSE
#define CH32H417_EFL_USE_EFL1                  TRUE
#define CH32H417_USB_USE_USBFS                 TRUE
#define CH32H417_RTC_USE_RTC1                  TRUE
#define CH32H417_TRNG_USE_RNG                  TRUE
#define CH32H417_WDG_USE_IWDG                  TRUE
#define CH32H417_WDG_USE_WWDG                  FALSE

/* QSPI2 carries the application image (XIP window 0x70000000).  The
   bootloader enables memory mapped mode on this unit before jumping. */
#define CH32H417_WSPI_USE_QSPI1                FALSE
#define CH32H417_WSPI_USE_QSPI2                TRUE

/* Unused on this board. */
#define CH32H417_I2S_USE_SPI2                  FALSE
#define CH32H417_I2S_USE_SPI3                  FALSE
#define CH32H417_DAC_USE_DAC1                  FALSE
#define CH32H417_CRY_USE_ECDC                  FALSE

#if HAL_XIP_ENABLED
#define CH32_QSPI2_NO_RESET TRUE
#define CH32H417_NO_INIT TRUE
#endif
