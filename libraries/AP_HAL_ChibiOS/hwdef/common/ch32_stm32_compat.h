/*
 * STM32-name compatibility layer for the CH32H417 RISC-V port.
 *
 * AP_HAL_ChibiOS is written against the STM32 ChibiOS port: it uses
 * STM32 type names, STM32 CMSIS register/bit names and STM32 DMA/TIM
 * API names, and it also puts those names inside #if expressions,
 * where -Werror=undef turns a missing name into a hard error.
 *
 * The CH32H417 (QingKe V5F) port exposes the same hardware through the
 * WCH vendor headers and the WCH LLDs, whose naming differs (CTLR1 vs
 * CR1, ch32DmaStream* vs dmaStream*, CH32_DMAMUX_* vs STM32_DMAMUX1_*).
 * This header bridges the two.
 *
 * Macros that reference CH32 symbols are expanded at the point of use,
 * i.e. after hal.h has pulled in the WCH vendor headers, so this file
 * only needs the WCH to be on the include path - not included itself.
 *
 * NOTE: this used to be emitted inline by hwdef/scripts/chibios_hwdef.py
 * into the generated hwdef.h.  It lives in a checked-in header now so
 * that editing the compatibility layer does not require re-running
 * ./waf configure (hwdef.h is only generated at configure time, which
 * made every bridge change silently use a stale hwdef.h).
 *
 * NOTE: STM32_SPI_USE_SPIx is NOT defined here.  hwdef.h emits TRUE for
 * the buses declared in hwdef.dat and FALSE fallbacks for the rest after
 * write_peripheral_enable(), and a same-file 0/TRUE pair is a -Werror
 * redefinition.
 */
#ifndef CH32_STM32_COMPAT_H
#define CH32_STM32_COMPAT_H

/* STM32-name compatibility for the CH32H417 RISC-V port. */
#define stm32_dma_stream_t ch32_dma_stream_t
#define stm32_dmaisr_t ch32_dmaisr_t
#define STM32_SDC_USE_SDMMC2 0
#define STM32_ADC_DUAL_MODE 0
/* CH32 routes every DMA request through its DMAMUX (RM 10.2.3, table 10-2)
   and the WCH LLD provides ch32DmaStreamSetRequest() to program it, which
   dmaSetRequestSource below aliases to. */
#define STM32_DMA_SUPPORTS_DMAMUX 1
/* CH32 DMAv1 has no FIFO, so dmaStreamSetFIFO() paths are compiled out. */
#define STM32_DMA_ADVANCED 0
#define STM32_PCLK1 CH32H417_HCLK
#define STM32_PCLK2 CH32H417_HCLK
#define STM32_HCLK CH32H417_HCLK
#define STM32_SYS_CK CH32H417_SYSCLK
#define STM32_FDCANCLK CH32H417_HCLK

/*
 * The QingKe V5F core has a single-precision FPU (rv32imafc) and the FPU is
 * enabled by crt0 (mstatus.FS) and by the port's context switch.  STM32's
 * CMSIS device headers define __FPU_PRESENT; the WCH headers do not, and
 * AP_HAL/utility/print_vprintf.cpp expands it inside a #if expression where
 * -Werror=undef turns a missing name into a hard error.
 *
 * This is a property of the *chip*, not of the board, so it belongs here
 * rather than in a hwdef.dat: putting it in CCHv6c/hwdef.dat only covered the
 * application image and left the bootloader build (hwdef-bl.dat) broken.
 */
#ifndef __FPU_PRESENT
#define __FPU_PRESENT 1
#endif

/*
 * USB CDC class request codes.
 *
 * WCH's ch32h417_usb.h (pulled in through ch32h417_conf.h from chcore.h)
 * defines CDC_SET_LINE_CODING / CDC_GET_LINE_CODING / CDC_SEND_BREAK as
 * plain literals (0x20, 0x21, 0x23), inside an #ifndef guard.  ChibiOS'
 * hal_usb_cdc.h defines the same names as 0x20U / 0x21U / 0x23U, so a TU
 * that ends up including both emits "macro redefined" warnings.  Undef'ing
 * them in ch32h417_mcuconf.h (the earlier attempt) was order dependent: when
 * halconf.h/mcuconf.h is processed before ch32h417_usb.h, the undef defeats
 * the vendor's #ifndef guard and the collision comes back.
 *
 * Defining them here - this header is reached from hwdef.h, which chconf.h
 * and mcuconf.h both include, i.e. before either of the two - makes the
 * vendor header skip its own copies (same #ifndef guard) and leaves
 * hal_usb_cdc.h redefining an *identical* token sequence, which the C
 * standard allows silently.
 */
#ifndef CDC_SET_LINE_CODING
#define CDC_SET_LINE_CODING 0x20U
#endif
#ifndef CDC_GET_LINE_CODING
#define CDC_GET_LINE_CODING 0x21U
#endif
#ifndef CDC_SEND_BREAK
#define CDC_SEND_BREAK 0x23U
#endif

/*
 * DMA API bridge.  AP_HAL_ChibiOS drives UART/RC-output DMA through the
 * STM32 DMA API names (dmaStreamAllocI & friends live in the STM32 port's
 * stm32_dma.h); the WCH DMAv1 LLD provides the identical operations under
 * the ch32Dma* prefix.  The ISR callback type has the same signature
 * (void (*)(void *p, uint32_t flags)) on both ports.
 */
#define dmaStreamAllocI ch32DmaStreamAllocI
#define dmaStreamFreeI ch32DmaStreamFreeI
#define dmaStreamSetPeripheral ch32DmaStreamSetPeripheral
#define dmaStreamSetMemory0 ch32DmaStreamSetMemory0
#define dmaStreamSetTransactionSize ch32DmaStreamSetTransactionSize
#define dmaStreamGetTransactionSize ch32DmaStreamGetTransactionSize
#define dmaStreamSetMode ch32DmaStreamSetMode
#define dmaStreamEnable ch32DmaStreamEnable
#define dmaStreamDisable ch32DmaStreamDisable
#define dmaSetRequestSource ch32DmaStreamSetRequest

/*
 * Stream-id encodings.  dma_resolver.py addresses channels as
 * STM32_DMA_STREAM_ID(controller, stream) with controller in 1..2 and
 * stream in 0..7; the CH32 DMAv1 LLD addresses its 16 DMAMUX-fed channels
 * linearly 0..15 (DMA1_Ch1..DMA1_Ch8 = 0..7, DMA2_Ch1..DMA2_Ch8 = 8..15).
 * The layouts correspond one-to-one.  SHARED_DMA_MAX_STREAM_ID is 16, so
 * the ANY id (16) still takes the allocator path instead of the lock table.
 */
#define STM32_DMA_STREAM_ID(d, c) (((d) - 1U) * 8U + (c))
#define STM32_DMA_STREAM_ID_ANY CH32_DMA_STREAM_ID_ANY

/*
 * DMAMUX request names.  dma_resolver.py emits STM32_DMAMUX1_<PERIPH> as
 * the request id (0-based, fed to dmaSetRequestSource); the WCH LLD names
 * them CH32_DMAMUX_<PERIPH> and ch32DmaStreamSetRequest() subtracts one
 * before programming the 1-based DMAMUX request field, so the values
 * match exactly.
 */
#define STM32_DMAMUX1_USART1_RX CH32_DMAMUX_USART1_RX
#define STM32_DMAMUX1_USART1_TX CH32_DMAMUX_USART1_TX
#define STM32_DMAMUX1_USART2_RX CH32_DMAMUX_USART2_RX
#define STM32_DMAMUX1_USART2_TX CH32_DMAMUX_USART2_TX
#define STM32_DMAMUX1_USART3_RX CH32_DMAMUX_USART3_RX
#define STM32_DMAMUX1_USART3_TX CH32_DMAMUX_USART3_TX
#define STM32_DMAMUX1_USART4_RX CH32_DMAMUX_USART4_RX
#define STM32_DMAMUX1_USART4_TX CH32_DMAMUX_USART4_TX
#define STM32_DMAMUX1_USART5_RX CH32_DMAMUX_USART5_RX
#define STM32_DMAMUX1_USART5_TX CH32_DMAMUX_USART5_TX
#define STM32_DMAMUX1_USART6_RX CH32_DMAMUX_USART6_RX
#define STM32_DMAMUX1_USART6_TX CH32_DMAMUX_USART6_TX
#define STM32_DMAMUX1_USART7_RX CH32_DMAMUX_USART7_RX
#define STM32_DMAMUX1_USART7_TX CH32_DMAMUX_USART7_TX
#define STM32_DMAMUX1_USART8_RX CH32_DMAMUX_USART8_RX
#define STM32_DMAMUX1_USART8_TX CH32_DMAMUX_USART8_TX
#define STM32_DMAMUX1_SPI1_RX CH32_DMAMUX_SPI1_RX
#define STM32_DMAMUX1_SPI1_TX CH32_DMAMUX_SPI1_TX
#define STM32_DMAMUX1_SPI2_RX CH32_DMAMUX_SPI2_RX
#define STM32_DMAMUX1_SPI2_TX CH32_DMAMUX_SPI2_TX
#define STM32_DMAMUX1_SPI3_RX CH32_DMAMUX_SPI3_RX
#define STM32_DMAMUX1_SPI3_TX CH32_DMAMUX_SPI3_TX
#define STM32_DMAMUX1_SPI4_RX CH32_DMAMUX_SPI4_RX
#define STM32_DMAMUX1_SPI4_TX CH32_DMAMUX_SPI4_TX
#define STM32_DMAMUX1_SPI5_RX CH32_DMAMUX_SPI5_RX
#define STM32_DMAMUX1_SPI5_TX CH32_DMAMUX_SPI5_TX
#define STM32_DMAMUX1_SPI6_RX CH32_DMAMUX_SPI6_RX
#define STM32_DMAMUX1_SPI6_TX CH32_DMAMUX_SPI6_TX
#define STM32_DMAMUX1_I2C1_RX CH32_DMAMUX_I2C1_RX
#define STM32_DMAMUX1_I2C1_TX CH32_DMAMUX_I2C1_TX
#define STM32_DMAMUX1_I2C2_RX CH32_DMAMUX_I2C2_RX
#define STM32_DMAMUX1_I2C2_TX CH32_DMAMUX_I2C2_TX
#define STM32_DMAMUX1_I2C3_RX CH32_DMAMUX_I2C3_RX
#define STM32_DMAMUX1_I2C3_TX CH32_DMAMUX_I2C3_TX
#define STM32_DMAMUX1_I2C4_RX CH32_DMAMUX_I2C4_RX
#define STM32_DMAMUX1_I2C4_TX CH32_DMAMUX_I2C4_TX
#define STM32_DMAMUX1_ADC1 CH32_DMAMUX_ADC1
#define STM32_DMAMUX1_ADC2 CH32_DMAMUX_ADC2

/*
 * Timer DMA requests.  The WCH DMAv1 LLD does not define these (it only
 * defines the ones its own peripheral LLDs use), so they are transcribed
 * from CH32H417RM.PDF v1.8 table 10-2 "DMA 复用器输入到资源的分配表"
 * (page 147-148).  Values are the 1-based "DMA 请求输入" numbers from the
 * table, matching the CH32_DMAMUX_* convention above
 * (ch32DmaStreamSetRequest() subtracts one before writing DMAMUX CFGR).
 * Cross-checked against the LLD: SPI1_TX=63, I2C1_TX=73, USART1_TX=85,
 * DAC1=103, SDIO=111, ADC1=120, QSPI1=71, QSPI2=72 all match.
 */
#define STM32_DMAMUX1_TIM1_CH1 1U
#define STM32_DMAMUX1_TIM1_CH2 2U
#define STM32_DMAMUX1_TIM1_CH3 3U
#define STM32_DMAMUX1_TIM1_CH4 4U
#define STM32_DMAMUX1_TIM1_UP 5U
#define STM32_DMAMUX1_TIM1_COM 6U
#define STM32_DMAMUX1_TIM1_TRIG 7U
#define STM32_DMAMUX1_TIM2_CH1 8U
#define STM32_DMAMUX1_TIM2_CH2 9U
#define STM32_DMAMUX1_TIM2_CH3 10U
#define STM32_DMAMUX1_TIM2_CH4 11U
#define STM32_DMAMUX1_TIM2_UP 12U
#define STM32_DMAMUX1_TIM2_TRIG 13U
#define STM32_DMAMUX1_TIM3_CH1 14U
#define STM32_DMAMUX1_TIM3_CH2 15U
#define STM32_DMAMUX1_TIM3_CH3 16U
#define STM32_DMAMUX1_TIM3_CH4 17U
#define STM32_DMAMUX1_TIM3_UP 18U
#define STM32_DMAMUX1_TIM3_TRIG 19U
#define STM32_DMAMUX1_TIM4_CH1 20U
#define STM32_DMAMUX1_TIM4_CH2 21U
#define STM32_DMAMUX1_TIM4_CH3 22U
#define STM32_DMAMUX1_TIM4_CH4 23U
#define STM32_DMAMUX1_TIM4_UP 24U
#define STM32_DMAMUX1_TIM4_TRIG 25U
#define STM32_DMAMUX1_TIM5_CH1 26U
#define STM32_DMAMUX1_TIM5_CH2 27U
#define STM32_DMAMUX1_TIM5_CH3 28U
#define STM32_DMAMUX1_TIM5_CH4 29U
#define STM32_DMAMUX1_TIM5_UP 30U
#define STM32_DMAMUX1_TIM5_TRIG 31U
#define STM32_DMAMUX1_TIM8_CH1 32U
#define STM32_DMAMUX1_TIM8_CH2 33U
#define STM32_DMAMUX1_TIM8_CH3 34U
#define STM32_DMAMUX1_TIM8_CH4 35U
#define STM32_DMAMUX1_TIM8_UP 36U
#define STM32_DMAMUX1_TIM8_COM 37U
#define STM32_DMAMUX1_TIM8_TRIG 38U
#define STM32_DMAMUX1_TIM9_CH1 39U
#define STM32_DMAMUX1_TIM9_CH2 40U
#define STM32_DMAMUX1_TIM9_CH3 41U
#define STM32_DMAMUX1_TIM9_CH4 42U
#define STM32_DMAMUX1_TIM9_UP 43U
#define STM32_DMAMUX1_TIM9_TRIG 44U
#define STM32_DMAMUX1_TIM10_CH1 45U
#define STM32_DMAMUX1_TIM10_CH2 46U
#define STM32_DMAMUX1_TIM10_CH3 47U
#define STM32_DMAMUX1_TIM10_CH4 48U
#define STM32_DMAMUX1_TIM10_UP 49U
#define STM32_DMAMUX1_TIM10_TRIG 50U
#define STM32_DMAMUX1_TIM11_CH1 51U
#define STM32_DMAMUX1_TIM11_CH2 52U
#define STM32_DMAMUX1_TIM11_CH3 53U
#define STM32_DMAMUX1_TIM11_CH4 54U
#define STM32_DMAMUX1_TIM11_UP 55U
#define STM32_DMAMUX1_TIM11_TRIG 56U
#define STM32_DMAMUX1_TIM12_CH1 57U
#define STM32_DMAMUX1_TIM12_CH2 58U
#define STM32_DMAMUX1_TIM12_CH3 59U
#define STM32_DMAMUX1_TIM12_CH4 60U
#define STM32_DMAMUX1_TIM12_UP 61U
#define STM32_DMAMUX1_TIM12_TRIG 62U
#define STM32_DMAMUX1_TIM6_UP 122U
#define STM32_DMAMUX1_TIM7_UP 123U

/*
 * Mode/status bits.  The WCH DMAv1 is F1-style: the CFGR bit layout
 * (EN/TCIE/HTIE/TEIE/DIR/CIRC/PINC/MINC/PSIZE/MSIZE/PL) is identical to
 * the STM32 DMA CR fields AP_HAL_ChibiOS uses, so the names map 1:1.
 * There is no CHSEL field - routing goes through the DMAMUX request
 * selector, which dmaSetRequestSource (above) programs separately.
 */
#define STM32_DMA_CR_EN CH32_DMA_CFGR_EN
#define STM32_DMA_CR_DIR_P2M CH32_DMA_CFGR_DIR_P2M
#define STM32_DMA_CR_DIR_M2P CH32_DMA_CFGR_DIR_M2P
#define STM32_DMA_CR_PSIZE_BYTE CH32_DMA_CFGR_PSIZE_BYTE
#define STM32_DMA_CR_PSIZE_HWORD CH32_DMA_CFGR_PSIZE_HWORD
#define STM32_DMA_CR_PSIZE_WORD CH32_DMA_CFGR_PSIZE_WORD
#define STM32_DMA_CR_MSIZE_BYTE CH32_DMA_CFGR_MSIZE_BYTE
#define STM32_DMA_CR_MSIZE_HWORD CH32_DMA_CFGR_MSIZE_HWORD
#define STM32_DMA_CR_MSIZE_WORD CH32_DMA_CFGR_MSIZE_WORD
#define STM32_DMA_CR_MINC CH32_DMA_CFGR_MINC
#define STM32_DMA_CR_PINC CH32_DMA_CFGR_PINC
#define STM32_DMA_CR_CIRC CH32_DMA_CFGR_CIRC
#define STM32_DMA_CR_TEIE CH32_DMA_CFGR_TEIE
#define STM32_DMA_CR_TCIE CH32_DMA_CFGR_TCIE
#define STM32_DMA_CR_HTIE CH32_DMA_CFGR_HTIE
#define STM32_DMA_CR_PL(n) CH32_DMA_CFGR_PL(n)
#define STM32_DMA_CR_CHSEL(n) 0U
/* ISR callback flags are pre-shifted to bit zero on both ports. */
#define STM32_DMA_ISR_TCIF CH32_DMA_ISR_TCIF
#define STM32_DMA_ISR_HTIF CH32_DMA_ISR_HTIF
#define STM32_DMA_ISR_TEIF CH32_DMA_ISR_TEIF
#define STM32_DMA_ISR_DMEIF 0U

/*
 * TIM bit bridges for RCOutput's DMAR / bdshot paths.  The CH32H417 timer
 * is F1-style: CTLR1/CTLR2/CHCTLR1/CHCTLR2/CCER/DMAINTENR/INTFR/SWEVGR/
 * ATRLR/DMACFGR/DMAADR play the roles of STM32's CR1/CR2/CCMR1/CCMR2/CCER/
 * DIER/SR/EGR/ARR/DCR/DMAR, with identical bit layouts (RCM section 12).
 * The bit macros below map the STM32 CMSIS names onto the WCH names; every
 * value was verified against CH32H417RM v1.8 (TIM_DMACFGR: DBA[4:0] at
 * bits 4:0, DBL[4:0] at bits 11:8 - the same positions STM32 uses).
 * Register member names (DMAR etc.) are aliased file-locally in
 * RCOutput.cpp / RCOutput_bdshot.cpp.
 */
#define STM32_TIM_CR1_CEN TIM_CEN
#define STM32_TIM_CR1_UDIS TIM_UDIS
#define STM32_TIM_CR1_URS TIM_URS
#define STM32_TIM_CR1_ARPE TIM_ARPE
#define STM32_TIM_EGR_UG TIM_UG
#define STM32_TIM_DIER_IRQ_MASK (TIM_UIE|TIM_CC1IE|TIM_CC2IE|TIM_CC3IE|TIM_CC4IE|TIM_COMIE|TIM_TIE|TIM_BIE)
#define STM32_TIM_DCR_DBA_MASK TIM_DBA
#define STM32_TIM_DCR_DBA(n) ((n) & TIM_DBA)
#define STM32_TIM_DCR_DBL_MASK TIM_DBL
#define STM32_TIM_DCR_DBL(n) (((n) << 8) & TIM_DBL)
/* CMSIS-style names used by RCOutput_bdshot.cpp */
#define TIM_CCMR1_CC1S TIM_CC1S
#define TIM_CCMR1_CC1S_0 TIM_CC1S_0
#define TIM_CCMR1_CC1S_1 TIM_CC1S_1
#define TIM_CCMR1_IC1F TIM_IC1F
#define TIM_CCMR1_IC1F_1 TIM_IC1F_1
#define TIM_CCMR1_IC1PSC TIM_IC1PSC
#define TIM_CCMR1_CC2S TIM_CC2S
#define TIM_CCMR1_CC2S_0 TIM_CC2S_0
#define TIM_CCMR1_CC2S_1 TIM_CC2S_1
#define TIM_CCMR1_IC2F TIM_IC2F
#define TIM_CCMR1_IC2F_1 TIM_IC2F_1
#define TIM_CCMR1_IC2PSC TIM_IC2PSC
#define TIM_CCMR2_CC3S TIM_CC3S
#define TIM_CCMR2_CC3S_0 TIM_CC3S_0
#define TIM_CCMR2_CC3S_1 TIM_CC3S_1
#define TIM_CCMR2_IC3F TIM_IC3F
#define TIM_CCMR2_IC3F_1 TIM_IC3F_1
#define TIM_CCMR2_IC3PSC TIM_IC3PSC
#define TIM_CCMR2_CC4S TIM_CC4S
#define TIM_CCMR2_CC4S_0 TIM_CC4S_0
#define TIM_CCMR2_CC4S_1 TIM_CC4S_1
#define TIM_CCMR2_IC4F TIM_IC4F
#define TIM_CCMR2_IC4F_1 TIM_IC4F_1
#define TIM_CCMR2_IC4PSC TIM_IC4PSC
#define TIM_CCER_CC1E TIM_CC1E
#define TIM_CCER_CC1P TIM_CC1P
#define TIM_CCER_CC1NP TIM_CC1NP
#define TIM_CCER_CC2E TIM_CC2E
#define TIM_CCER_CC2P TIM_CC2P
#define TIM_CCER_CC2NP TIM_CC2NP
#define TIM_CCER_CC3E TIM_CC3E
#define TIM_CCER_CC3P TIM_CC3P
#define TIM_CCER_CC3NP TIM_CC3NP
#define TIM_CCER_CC4E TIM_CC4E
#define TIM_CCER_CC4P TIM_CC4P
/* NOTE: there is no TIM_CCER_CC4NP mapping.  CH32H417's CCER has bits
   15:14 reserved (RM 14.4.9), i.e. channel 4 has no complementary-output
   polarity bit, so TIM_CCER_CC4NP (used by the bidir-dshot telemetry
   configuration) cannot map 1:1.  That code path only compiles when
   HAL_WITH_BIDIR_DSHOT is defined; enabling it on this port will need a
   channel-4 double-edge capture rework, not a macro. */
#define TIM_DIER_CC1DE TIM_CC1DE
#define TIM_DIER_CC2DE TIM_CC2DE
#define TIM_DIER_CC3DE TIM_CC3DE
#define TIM_DIER_CC4DE TIM_CC4DE
#define TIM_DIER_UDE TIM_UDE
#define TIM_CR1_ARPE TIM_ARPE
/* CMSIS register helpers used by RCOutput_bdshot.cpp */
#define READ_REG(REG) (*(volatile uint32_t *)(REG))
#define WRITE_REG(REG, VAL) (*(volatile uint32_t *)(REG) = (VAL))
#define MODIFY_REG(REG, CLEARMASK, SETMASK) WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

/*
 * USART bit-name bridge for UARTDriver.  The WCH vendor header names the
 * F1-model USART registers CTLR1/CTLR2/CTLR3; AP_HAL_ChibiOS drives
 * parity, stop-bit, half-duplex, flow-control and DMA options through the
 * CMSIS CRx bit names.  The bit positions are identical F1 layout
 * (CH32H417RM 19.11), so the names map 1:1.  The register *member* names
 * (usart->CR1 etc. -> CTLR1/CTLR2/CTLR3/STATR/DATAR) are aliased
 * file-locally in UARTDriver.cpp, like the TIM aliases in RCOutput.cpp.
 */
#define USART_CR1_UE USART_CTLR1_UE
#define USART_CR1_M USART_CTLR1_M
#define USART_CR1_PCE USART_CTLR1_PCE
#define USART_CR1_PS USART_CTLR1_PS
#define USART_CR1_RXNEIE USART_CTLR1_RXNEIE
#define USART_CR1_IDLEIE USART_CTLR1_IDLEIE
/* Stop-bit values follow the ChibiOS STM32 serial LLD convention
   (hal_serial_lld.h): STOP1_BITS selects 1 stop bit (field 0b00) and
   STOP2_BITS selects 2 stop bits (field 0b10 = 0x2000, WCH
   USART_CTLR2_STOP_1).  F1 encodes 1.5 stop bits as 0b11; ArduPilot never
   requests it. */
#define USART_CR2_STOP1_BITS 0U
#define USART_CR2_STOP2_BITS USART_CTLR2_STOP_1
#define USART_CR3_HDSEL USART_CTLR3_HDSEL
#define USART_CR3_DMAR USART_CTLR3_DMAR
#define USART_CR3_DMAT USART_CTLR3_DMAT
#define USART_CR3_CTSE USART_CTLR3_CTSE
#define USART_CR3_RTSE USART_CTLR3_RTSE
/* CH32H417 has no CR3 ONEBIT oversampling bit (F1 CR3 bit 11 is
   repurposed for LP wakeup, RM 19.11.4), so the HAL_HAVE_LOW_NOISE_UART
   one-bit-sampling option degrades to a no-op instead of enabling the
   unrelated LPWKUP_EN bit. */
#ifndef USART_CR3_ONEBIT
#define USART_CR3_ONEBIT 0U
#endif

/*
 * SPI bit-name bridge for SPIDevice.  The WCH SPI LLD is SPIv2-style with
 * F1 register names (CTLR1/CTLR2/STATR/DATAR); AP_HAL_ChibiOS builds the
 * SPIDEV_MODE* values and the baud-rate divider from the CMSIS CR1 names.
 * CPOL/CPHA/BR bit positions are identical F1 layout.
 */
#define SPI_CR1_CPHA SPI_CTLR1_CPHA
#define SPI_CR1_CPOL SPI_CTLR1_CPOL
#define SPI_CR1_BR_0 SPI_CTLR1_BR_0

/* CH32 DMAv1 has no direct-mode-error interrupt (its CFGR is the F1 subset
   of the STM32 bit layout), so the DMEIE term in UARTDriver's DMA mode
   compiles away to nothing instead of being an undefined name. */
#define STM32_DMA_CR_DMEIE 0U

/*
 * TIM complementary-output modes.  The WCH TIMv1 PWM LLD implements only
 * the generic PWM_OUTPUT_* modes; RCOutput still compares channel modes
 * against the complementary constants when applying SERVOn_POLARITY, so
 * they must exist.  Values are the STM32 TIM LLD ones (distinct high
 * nibble, never produced by this LLD because the board's hwdef.dat has no
 * complementary outputs).
 */
#define PWM_COMPLEMENTARY_OUTPUT_MASK 0xF0
#define PWM_COMPLEMENTARY_OUTPUT_DISABLED 0x00
#define PWM_COMPLEMENTARY_OUTPUT_ACTIVE_HIGH 0x10
#define PWM_COMPLEMENTARY_OUTPUT_ACTIVE_LOW 0x20

#endif /* CH32_STM32_COMPAT_H */
