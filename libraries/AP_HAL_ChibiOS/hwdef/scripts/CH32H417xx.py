#!/usr/bin/env python3

# flake8: noqa
'''
these tables are generated from the WCH datasheets for the CH32H417
'''

# additional build information for ChibiOS
build = {
    "CHIBIOS_STARTUP_MK"  : "os/common/startup/RISCV-QINGKE/CH32H417/V3F/mk/startup.mk",
    "CHIBIOS_PLATFORM_MK" : "os/hal/ports/CH32/CH32H417/platform.mk"
    }

# MCU parameters
mcu = {
    # CH32H417 SRAM regions:
    #   ITCM       = 0x200A0000, 128KB (fast code execution, tightly coupled) -- INSTRUCTION_RAM
    #   DTCM       = 0x200C0000, 256KB (fast data, tightly coupled)          -- DATA_RAM
    #   SRAM2/XIP  = 0x20100000, 512KB (DMA-capable, flash operation RAM)    -- FLASH_RAM
    # ram map, as list of (address, size-kb, flags)
    # flags of 1 means DMA-capable
    # flags of 2 means faster memory for CPU intensive work
    # flags of 4 means memory can be used for SDMMC DMA
    'RAM_MAP' : [
        (0x20100000, 512, 1),  # FLASH_RAM (SRAM2) - DMA-capable, flash operation RAM
        (0x200C0000, 256, 2),  # DATA_RAM (DTCM) - fast data access (vtables, etc.)
        (0x200A0000, 128, 2),  # INSTRUCTION_RAM (ITCM) - fast code execution
    ],

    # same RAM layout when running from external flash
    # First region must be DMA-capable for XIP boot (FLASH_RAM)
    'RAM_MAP_EXTERNAL_FLASH' : [
        (0x20100000, 512, 1),  # FLASH_RAM (SRAM2) - DMA-capable, flash operation RAM
        (0x200C0000, 256, 2),  # DATA_RAM (DTCM) - fast data access (vtables, etc.)
        (0x200A0000, 128, 2),  # INSTRUCTION_RAM (ITCM) - fast code execution
    ],

    # RAM layout for bootloader (DTCM first to avoid DCache issues, matching STM32H7 pattern)
    'RAM_MAP_BOOTLOADER' : [
        (0x200C0000, 256, 2),  # DATA_RAM (DTCM) - fast data access (vtables, etc.)
        (0x20100000, 512, 1),  # FLASH_RAM (SRAM2) - DMA-capable, flash operation RAM
        (0x200A0000, 128, 2),  # INSTRUCTION_RAM (ITCM) - fast code execution
    ],

    # memory regions for external flash XIP linker script
    'INSTRUCTION_RAM' : (0x200A0000, 128),   # ITCM for fast code execution
    'FLASH_RAM' :       (0x20100000, 512),   # SRAM2 for RAM functions and process stack, DMA-capable
    'DATA_RAM' :        (0x200C0000, 256),   # DTCM for fast data (vtables, etc.)

    'EXPECTED_CLOCK' : 100000000,

    # this MCU has RISC-V single-precision FPU (rv32imafc)
    'CORTEX'    : 'riscv',
    'CPU_FLAGS' : '-march=rv32imafc_zba_zbb_zbc_zbs_xw -mabi=ilp32f -mcmodel=medlow -u_printf_float',

    'DEFINES' : {
        'HAL_HAVE_HARDWARE_DOUBLE' : '0',
        'HAL_WITH_MCU_MONITORING' : '0',
        '__FPU_PRESENT' : '1',
        'CH32H417' : '1',
    },

    # CPU serial number (12 bytes) address
    'UDID_START' : 0x1ffff7e8,
}

pincount = {
    'A': 16,
    'B': 16,
    'C': 16,
    'D': 16,
    'E': 16,
    'F': 16,
}

# CH32H417 uses DMAMUX for dynamic DMA channel allocation
DMA_Map = None

AltFunction_map = {
    # format is PIN:FUNCTION : AFNUM
    # extracted from CH32H417 datasheet

    # ********** TIM1 **********
    "PA8:TIM1_CH1"          :   1,
    "PE9:TIM1_CH1"          :   1,
    "PA9:TIM1_CH2"          :   1,
    "PE11:TIM1_CH2"         :   1,
    "PA10:TIM1_CH3"         :   1,
    "PE13:TIM1_CH3"         :   1,
    "PA11:TIM1_CH4"         :   1,
    "PE14:TIM1_CH4"         :   1,
    "PA7:TIM1_CH1N"         :   1,
    "PE8:TIM1_CH1N"         :   1,
    "PB13:TIM1_CH1N"        :   1,
    "PB0:TIM1_CH2N"         :   1,
    "PE10:TIM1_CH2N"        :   1,
    "PB14:TIM1_CH2N"        :   1,
    "PB1:TIM1_CH3N"         :   1,
    "PE12:TIM1_CH3N"        :   1,
    "PB15:TIM1_CH3N"        :   1,
    "PA6:TIM1_BKIN"         :   1,
    "PE15:TIM1_BKIN"        :   1,
    "PB12:TIM1_BKIN"        :   1,
    "PA5:TIM1_BKIN2"        :   2,
    "PE6:TIM1_BKIN2"        :   1,
    "PA12:TIM1_BKIN2"       :  12,

    # ********** TIM2 **********
    "PA0:TIM2_CH1"          :   1,
    "PA0:TIM2_ETR"          :   1,
    "PA5:TIM2_CH1"          :   1,
    "PA5:TIM2_ETR"          :   1,
    "PA15:TIM2_CH1"         :   1,
    "PA15:TIM2_ETR"         :   1,
    "PA1:TIM2_CH2"          :   1,
    "PB3:TIM2_CH2"          :   1,
    "PA2:TIM2_CH3"          :   1,
    "PB10:TIM2_CH3"         :   1,
    "PA3:TIM2_CH4"          :   1,
    "PB11:TIM2_CH4"         :   1,

    # ********** TIM3 **********
    "PA6:TIM3_CH1"          :   2,
    "PB4:TIM3_CH1"          :   2,
    "PC6:TIM3_CH1"          :   2,
    "PD3:TIM3_CH1"          :   9,
    "PA7:TIM3_CH2"          :   2,
    "PB5:TIM3_CH2"          :   2,
    "PC7:TIM3_CH2"          :   2,
    "PD4:TIM3_CH2"          :   9,
    "PB0:TIM3_CH3"          :   2,
    "PC8:TIM3_CH3"          :   2,
    "PD5:TIM3_CH3"          :   9,
    "PB1:TIM3_CH4"          :   2,
    "PC9:TIM3_CH4"          :   2,
    "PD6:TIM3_CH4"          :   9,
    "PD2:TIM3_ETR"          :   2,

    # ********** TIM4 **********
    "PB6:TIM4_CH1"          :   2,
    "PD12:TIM4_CH1"         :   2,
    "PE3:TIM4_CH1"          :   2,
    "PB7:TIM4_CH2"          :   2,
    "PD13:TIM4_CH2"         :   2,
    "PE4:TIM4_CH2"          :   2,
    "PB8:TIM4_CH3"          :   2,
    "PD14:TIM4_CH3"         :   2,
    "PE5:TIM4_CH3"          :   2,
    "PB9:TIM4_CH4"          :   2,
    "PD15:TIM4_CH4"         :   2,
    "PE6:TIM4_CH4"          :   2,
    "PB4:TIM4_ETR"          :  10,

    # ********** TIM5 **********
    "PA0:TIM5_CH1"          :   2,
    "PD12:TIM5_CH1"         :   6,
    "PC1:TIM5_CH1"          :   2,
    "PA1:TIM5_CH2"          :   2,
    "PD13:TIM5_CH2"         :   6,
    "PC2:TIM5_CH2"          :   2,
    "PA2:TIM5_CH3"          :   2,
    "PD14:TIM5_CH3"         :   6,
    "PC3:TIM5_CH3"          :   2,
    "PA3:TIM5_CH4"          :   2,
    "PD15:TIM5_CH4"         :   6,
    "PB0:TIM5_CH4"          :   4,
    "PA4:TIM5_ETR"          :   2,
    "PD11:TIM5_ETR"         :   6,

    # ********** TIM8 **********
    "PA0:TIM8_ETR"          :   3,
    "PF2:TIM8_ETR"          :   3,
    "PC6:TIM8_CH1"          :   3,
    "PE3:TIM8_CH1"          :   0,
    "PC7:TIM8_CH2"          :   3,
    "PE4:TIM8_CH2"          :   0,
    "PC8:TIM8_CH3"          :   3,
    "PE5:TIM8_CH3"          :   0,
    "PC9:TIM8_CH4"          :   3,
    "PE6:TIM8_CH4"          :   0,
    "PA5:TIM8_CH1N"         :   3,
    "PA7:TIM8_CH1N"         :   3,
    "PC1:TIM8_CH1N"         :   0,
    "PB0:TIM8_CH2N"         :   3,
    "PB14:TIM8_CH2N"        :   3,
    "PC2:TIM8_CH2N"         :   0,
    "PB1:TIM8_CH3N"         :   3,
    "PB15:TIM8_CH3N"        :   3,
    "PC3:TIM8_CH3N"         :   0,
    "PA6:TIM8_BKIN"         :   3,
    "PB12:TIM8_BKIN"        :   2,
    "PC0:TIM8_BKIN"         :   0,
    "PA8:TIM8_BKIN2"        :   3,
    "PB13:TIM8_BKIN2"       :   2,

    # ********** TIM9 **********
    "PB13:TIM9_ETR"         :   4,
    "PA4:TIM9_ETR"          :   4,
    "PC8:TIM9_ETR"          :   6,
    "PB14:TIM9_CH1"         :   2,
    "PA0:TIM9_CH1"          :   6,
    "PC9:TIM9_CH1"          :   6,
    "PB15:TIM9_CH2"         :   2,
    "PA1:TIM9_CH2"          :   6,
    "PC10:TIM9_CH2"         :   2,
    "PB10:TIM9_CH2"         :   2,
    "PE5:TIM9_CH3"          :   4,
    "PA2:TIM9_CH3"          :   4,
    "PC12:TIM9_CH3"         :   2,
    "PB12:TIM9_CH3"         :   8,
    "PE6:TIM9_CH4"          :   4,
    "PA3:TIM9_CH4"          :   4,
    "PC11:TIM9_CH4"         :   2,
    "PB11:TIM9_CH4"         :   9,

    # ********** TIM10 **********
    "PF10:TIM10_ETR"        :   8,
    "PA5:TIM10_ETR"         :   9,
    "PB5:TIM10_ETR"         :   0,
    "PF8:TIM10_CH1"         :   9,
    "PA6:TIM10_CH1"         :   9,
    "PB6:TIM10_CH1"         :   0,
    "PF9:TIM10_CH2"         :   9,
    "PA7:TIM10_CH2"         :   9,
    "PB7:TIM10_CH2"         :   0,
    "PF6:TIM10_CH3"         :   9,
    "PA3:TIM10_CH3"         :   8,
    "PB8:TIM10_CH3"         :   1,
    "PF7:TIM10_CH4"         :   9,
    "PA4:TIM10_CH4"         :   9,
    "PB9:TIM10_CH4"         :   1,

    # ********** TIM11 **********
    "PB2:TIM11_ETR"         :  13,
    "PD5:TIM11_ETR"         :  13,
    "PD3:TIM11_CH1"         :   2,
    "PF6:TIM11_CH1"         :  13,
    "PE0:TIM11_CH1"         :  13,
    "PD4:TIM11_CH2"         :   2,
    "PF7:TIM11_CH2"         :  13,
    "PE1:TIM11_CH2"         :  13,
    "PD5:TIM11_CH3"         :   2,
    "PF8:TIM11_CH3"         :  13,
    "PD7:TIM11_CH3"         :  13,
    "PD6:TIM11_CH4"         :   2,
    "PF9:TIM11_CH4"         :  13,

    # ********** TIM12 **********
    "PB0:TIM12_ETR"         :  13,
    "PB3:TIM12_ETR"         :  14,
    "PB1:TIM12_CH1"         :   5,
    "PE3:TIM12_CH1"         :   3,
    "PB2:TIM12_CH2"         :   5,
    "PE4:TIM12_CH2"         :   3,
    "PE13:TIM12_CH2"        :   2,
    "PF12:TIM12_CH3"        :  13,
    "PE5:TIM12_CH3"         :   3,
    "PE14:TIM12_CH3"        :   2,
    "PF13:TIM12_CH4"        :  13,
    "PE6:TIM12_CH4"         :   3,
    "PE15:TIM12_CH4"        :   2,

    # ********** SPI1 **********
    "PA4:SPI1_NSS"          :   5,
    "PA15:SPI1_NSS"         :   5,
    "PF4:SPI1_NSS"          :   5,
    "PF6:SPI1_NSS"          :   3,
    "PA5:SPI1_SCK"          :   5,
    "PB3:SPI1_SCK"          :   5,
    "PF5:SPI1_SCK"          :   5,
    "PF7:SPI1_SCK"          :   3,
    "PA7:SPI1_MOSI"         :   5,
    "PB5:SPI1_MOSI"         :   5,
    "PD7:SPI1_MOSI"         :   5,
    "PF8:SPI1_MOSI"         :   3,
    "PA6:SPI1_MISO"         :   5,
    "PB4:SPI1_MISO"         :   5,
    "PF3:SPI1_MISO"         :   5,
    "PF9:SPI1_MISO"         :   3,

    # ********** SPI2 **********
    "PB12:SPI2_NSS"         :   5,
    "PB9:SPI2_NSS"          :   5,
    "PA11:SPI2_NSS"         :   5,
    "PB4:SPI2_NSS"          :   7,
    "PB13:SPI2_SCK"         :   5,
    "PB10:SPI2_SCK"         :   5,
    "PA9:SPI2_SCK"          :   5,
    "PA12:SPI2_SCK"         :   5,
    "PD3:SPI2_SCK"          :   5,
    "PB15:SPI2_MOSI"        :   5,
    "PC1:SPI2_MOSI"         :   5,
    "PC3:SPI2_MOSI"         :   5,
    "PB14:SPI2_MISO"        :   5,
    "PC2:SPI2_MISO"         :   5,

    # ********** SPI3 **********
    "PA4:SPI3_NSS"          :   6,
    "PA15:SPI3_NSS"         :   6,
    "PB3:SPI3_SCK"          :   6,
    "PC10:SPI3_SCK"         :   6,
    "PA14:SPI3_SCK"         :   1,
    "PB2:SPI3_MOSI"         :   7,
    "PB5:SPI3_MOSI"         :   7,
    "PC12:SPI3_MOSI"        :   6,
    "PD6:SPI3_MOSI"         :   5,
    "PA13:SPI3_MOSI"        :   1,
    "PB4:SPI3_MISO"         :   6,
    "PC11:SPI3_MISO"        :   6,
    "PC9:SPI3_MISO"         :   5,

    # ********** SPI4 **********
    "PE4:SPI4_NSS"          :   5,
    "PE11:SPI4_NSS"         :   5,
    "PE2:SPI4_SCK"          :   5,
    "PE12:SPI4_SCK"         :   5,
    "PE6:SPI4_MOSI"         :   5,
    "PE14:SPI4_MOSI"        :   5,
    "PE5:SPI4_MISO"         :   5,
    "PE13:SPI4_MISO"        :   5,

    # ********** I2C1 **********
    "PB6:I2C1_SCL"          :   4,
    "PB8:I2C1_SCL"          :   4,
    "PB7:I2C1_SDA"          :   4,
    "PB9:I2C1_SDA"          :   4,
    "PB5:I2C1_SMBA"         :   4,

    # ********** I2C2 **********
    "PC0:I2C2_SCL"          :   9,
    "PB10:I2C2_SCL"         :   4,
    "PC1:I2C2_SDA"          :   9,
    "PB11:I2C2_SDA"         :   4,
    "PC2:I2C2_SMBA"         :   9,
    "PB12:I2C2_SMBA"        :   4,

    # ********** I2C3 **********
    "PA8:I2C3_SCL"          :   4,
    "PA14:I2C3_SCL"         :   7,
    "PC9:I2C3_SDA"          :   4,
    "PA13:I2C3_SDA"         :   7,
    "PA9:I2C3_SMBA"         :   4,
    "PA15:I2C3_SMBA"        :   7,

    # ********** I2C4 **********
    "PD12:I2C4_SCL"         :   4,
    "PF12:I2C4_SCL"         :   2,
    "PB6:I2C4_SCL"          :   6,
    "PB8:I2C4_SCL"          :   6,
    "PD13:I2C4_SDA"         :   4,
    "PF13:I2C4_SDA"         :   2,
    "PB7:I2C4_SDA"          :   6,
    "PB9:I2C4_SDA"          :   6,
    "PD11:I2C4_SMBA"        :   4,
    "PF11:I2C4_SMBA"        :   2,
    "PB5:I2C4_SMBA"         :   6,
    "PB9:I2C4_SMBA"         :  11,

    # ********** I3C **********
    "PE14:I3C_SCL"          :   3,
    "PC4:I3C_SCL"           :   7,
    "PD9:I3C_SCL"           :   5,
    "PF6:I3C_SCL"           :   5,
    "PE15:I3C_SDA"          :   3,
    "PC5:I3C_SDA"           :   7,
    "PD10:I3C_SDA"          :   5,
    "PF7:I3C_SDA"           :   5,

    # ********** USART1 **********
    "PA8:USART1_CK"         :   7,
    "PD11:USART1_CK"        :  14,
    "PA10:USART1_RX"        :   7,
    "PB7:USART1_RX"         :   7,
    "PB15:USART1_RX"        :   4,
    "PD12:USART1_RX"        :  14,
    "PA9:USART1_TX"         :   7,
    "PB6:USART1_TX"         :   7,
    "PB14:USART1_TX"        :   4,
    "PD13:USART1_TX"        :  14,
    "PA12:USART1_RTS"       :   7,
    "PD14:USART1_RTS"       :  14,
    "PA11:USART1_CTS"       :   7,
    "PD15:USART1_CTS"       :  14,

    # ********** USART2 **********
    "PA4:USART2_CK"         :   7,
    "PD7:USART2_CK"         :   7,
    "PA3:USART2_RX"         :   7,
    "PD6:USART2_RX"         :   7,
    "PA2:USART2_TX"         :   7,
    "PD5:USART2_TX"         :   7,
    "PA1:USART2_RTS"        :   7,
    "PD4:USART2_RTS"        :   7,
    "PA0:USART2_CTS"        :   7,
    "PD3:USART2_CTS"        :   7,

    # ********** USART3 **********
    "PB12:USART3_CK"        :   7,
    "PC12:USART3_CK"        :   7,
    "PD10:USART3_CK"        :   7,
    "PA11:USART3_CK"        :   4,
    "PB11:USART3_RX"        :   7,
    "PC11:USART3_RX"        :   7,
    "PD9:USART3_RX"         :   7,
    "PA14:USART3_RX"        :   4,
    "PB10:USART3_TX"        :   7,
    "PC10:USART3_TX"        :   7,
    "PD8:USART3_TX"         :   7,
    "PA13:USART3_TX"        :   4,
    "PB14:USART3_RTS"       :   7,
    "PD12:USART3_RTS"       :   7,
    "PA12:USART3_RTS"       :   4,
    "PB13:USART3_CTS"       :   7,
    "PD11:USART3_CTS"       :   7,
    "PA15:USART3_CTS"       :   4,

    # ********** USART4 **********
    "PF1:USART4_CK"         :   7,
    "PC8:USART4_CK"         :   7,
    "PF3:USART4_RX"         :   7,
    "PC7:USART4_RX"         :   7,
    "PF4:USART4_TX"         :   7,
    "PC6:USART4_TX"         :   7,
    "PF2:USART4_RTS"        :   7,
    "PE0:USART4_RTS"        :   7,
    "PF0:USART4_CTS"        :   7,
    "PE1:USART4_CTS"        :   7,

    # ********** USART5 **********
    "PE15:USART5_CK"        :  11,
    "PD6:USART5_CK"         :  11,
    "PE2:USART5_RX"         :   4,
    "PF5:USART5_RX"         :   4,
    "PE3:USART5_TX"         :  11,
    "PE0:USART5_TX"         :   4,
    "PD7:USART5_RTS"        :   4,
    "PE1:USART5_CTS"        :   4,

    # ********** USART6 **********
    # USART6 AF3 on CH32H417
    "PC11:USART6_RX"        :   3,
    "PC10:USART6_TX"        :   3,

    # ********** USART7 **********
    # USART7 is on AF3 for CH32H417 (inferred from pin allocation)
    "PC12:USART7_TX"        :   3,
    "PD2:USART7_RX"         :   3,
    "PC8:USART7_RTS"        :   3,
    "PC9:USART7_CTS"        :   3,

    # ********** USART8 **********
    # USART8 AF7 on CH32H417 (shared with USART4)
    "PC6:USART8_TX"         :   7,
    "PF3:USART8_RX"         :   7,

    # ********** CAN1 **********
    "PD0:CAN1_RX"           :   9,
    "PD1:CAN1_TX"           :   9,
    # CAN1 also available on PB8/PB9 etc with different AF
    "PB8:CAN1_RX"           :   9,
    "PB9:CAN1_TX"           :   9,
    "PA11:CAN1_RX"          :   9,
    "PA12:CAN1_TX"          :   9,

    # ********** CAN2 **********
    "PB5:CAN2_RX"           :   9,
    "PB13:CAN2_TX"          :   9,
    "PA8:CAN2_RX"           :   9,
    "PA15:CAN2_TX"          :   9,

    # ********** CAN3 **********
    "PF4:CAN3_RX"           :   9,
    "PF7:CAN3_TX"           :   9,
    "PA9:CAN3_RX"           :   9,
    "PA10:CAN3_TX"          :   9,

    # ********** SDIO **********
    # SDIO on CH32H417 uses specific AF
    "PB11:SDIO_CK"          :  10,
    "PB10:SDIO_CMD"         :  10,
    "PE8:SDIO_D0"           :  10,
    "PE9:SDIO_D1"           :  10,
    "PB3:SDIO_D2"           :  10,
    "PB4:SDIO_D3"           :  10,
    "PD6:SDIO_CK"           :  10,
    "PD7:SDIO_CMD"          :  10,
    "PB14:SDIO_D0"          :  10,
    "PB15:SDIO_D1"          :  10,

    # ********** USB OTG (USBFS) **********
    "PA11:OTG_FS_DM"        :   0,
    "PA12:OTG_FS_DP"        :   0,
    "PA9:VBUS"              :   0,

    # ********** LPTIM1 **********
    "PF4:LPTIM1_ETR"        :   1,
    "PD11:LPTIM1_ETR"       :   1,
    "PE0:LPTIM1_CH1"        :   1,
    "PD12:LPTIM1_CH1"       :   1,
    "PF5:LPTIM1_CH2"        :   1,
    "PD14:LPTIM1_CH2"       :   1,
    "PE1:LPTIM1_OC"         :   1,
    "PD13:LPTIM1_OC"        :   1,

    # ********** LPTIM2 **********
    "PB11:LPTIM2_ETR"       :   3,
    "PD10:LPTIM2_ETR"       :   4,
    "PB10:LPTIM2_CH1"       :   3,
    "PD12:LPTIM2_CH1"       :   3,
    "PD11:LPTIM2_CH2"       :   3,
    "PB12:LPTIM2_CH2"       :  13,
    "PB13:LPTIM2_OC"        :   3,

    # ********** QSPI1 **********
    "PF6:QUADSPI1_SCK"      :   1,
    "PF10:QUADSPI1_SCK"     :   0,
    "PB2:QUADSPI1_SCK"      :  11,
    "PF9:QUADSPI1_IO0"      :   1,
    "PF8:QUADSPI1_IO1"      :   1,
    "PF7:QUADSPI1_IO2"      :   1,
    "PF6:QUADSPI1_IO3"      :   1,
    "PA2:QUADSPI1_IO0"      :   9,
    "PA3:QUADSPI1_IO1"      :   9,
    "PF10:QUADSPI1_IO0"     :   1,
    "PF9:QUADSPI1_IO1"      :   6,
    "PE7:QUADSPI1_IO2"      :   1,
    "PE8:QUADSPI1_IO3"      :   1,
    "PC4:QUADSPI1_IO0"      :  13,

    # ********** QSPI2 **********
    "PF6:QUADSPI2_SCK"      :   2,
    "PF1:QUADSPI2_CS"       :   2,
    "PF2:QUADSPI2_IO0"      :   2,
    "PF9:QUADSPI2_IO1"      :   2,
    "PF10:QUADSPI2_IO2"     :   2,
    "PE15:QUADSPI2_IO3"     :   2,
    "PC1:QUADSPI2_IO0"      :   3,
    "PC2:QUADSPI2_IO1"      :   3,
    "PC3:QUADSPI2_IO2"      :   3,
    "PB13:QUADSPI2_IO3"     :   3,
    "PB14:QUADSPI2_IO0"     :   3,

    # ********** EVENT-OUT (all pins) **********
    "PA0:EVENT-OUT"         :  15,
    "PA1:EVENT-OUT"         :  15,
    "PA2:EVENT-OUT"         :  15,
    "PA3:EVENT-OUT"         :  15,
    "PA4:EVENT-OUT"         :  15,
    "PA5:EVENT-OUT"         :  15,
    "PA6:EVENT-OUT"         :  15,
    "PA7:EVENT-OUT"         :  15,
    "PA8:EVENT-OUT"         :  15,
    "PA9:EVENT-OUT"         :  15,
    "PA10:EVENT-OUT"        :  15,
    "PA11:EVENT-OUT"        :  15,
    "PA12:EVENT-OUT"        :  15,
    "PA13:EVENT-OUT"        :  15,
    "PA14:EVENT-OUT"        :  15,
    "PA15:EVENT-OUT"        :  15,
    "PB0:EVENT-OUT"         :  15,
    "PB1:EVENT-OUT"         :  15,
    "PB2:EVENT-OUT"         :  15,
    "PB3:EVENT-OUT"         :  15,
    "PB4:EVENT-OUT"         :  15,
    "PB5:EVENT-OUT"         :  15,
    "PB6:EVENT-OUT"         :  15,
    "PB7:EVENT-OUT"         :  15,
    "PB8:EVENT-OUT"         :  15,
    "PB9:EVENT-OUT"         :  15,
    "PB10:EVENT-OUT"        :  15,
    "PB11:EVENT-OUT"        :  15,
    "PB12:EVENT-OUT"        :  15,
    "PB13:EVENT-OUT"        :  15,
    "PB14:EVENT-OUT"        :  15,
    "PB15:EVENT-OUT"        :  15,
    "PC0:EVENT-OUT"         :  15,
    "PC1:EVENT-OUT"         :  15,
    "PC2:EVENT-OUT"         :  15,
    "PC3:EVENT-OUT"         :  15,
    "PC4:EVENT-OUT"         :  15,
    "PC5:EVENT-OUT"         :  15,
    "PC6:EVENT-OUT"         :  15,
    "PC7:EVENT-OUT"         :  15,
    "PC8:EVENT-OUT"         :  15,
    "PC9:EVENT-OUT"         :  15,
    "PC10:EVENT-OUT"        :  15,
    "PC11:EVENT-OUT"        :  15,
    "PC12:EVENT-OUT"        :  15,
    "PC13:EVENT-OUT"        :  15,
    "PC14:EVENT-OUT"        :  15,
    "PC15:EVENT-OUT"        :  15,
    "PD0:EVENT-OUT"         :  15,
    "PD1:EVENT-OUT"         :  15,
    "PD2:EVENT-OUT"         :  15,
    "PD3:EVENT-OUT"         :  15,
    "PD4:EVENT-OUT"         :  15,
    "PD5:EVENT-OUT"         :  15,
    "PD6:EVENT-OUT"         :  15,
    "PD7:EVENT-OUT"         :  15,
    "PD8:EVENT-OUT"         :  15,
    "PD9:EVENT-OUT"         :  15,
    "PD10:EVENT-OUT"        :  15,
    "PD11:EVENT-OUT"        :  15,
    "PD12:EVENT-OUT"        :  15,
    "PD13:EVENT-OUT"        :  15,
    "PD14:EVENT-OUT"        :  15,
    "PD15:EVENT-OUT"        :  15,
    "PE0:EVENT-OUT"         :  15,
    "PE1:EVENT-OUT"         :  15,
    "PE2:EVENT-OUT"         :  15,
    "PE3:EVENT-OUT"         :  15,
    "PE4:EVENT-OUT"         :  15,
    "PE5:EVENT-OUT"         :  15,
    "PE6:EVENT-OUT"         :  15,
    "PE7:EVENT-OUT"         :  15,
    "PE8:EVENT-OUT"         :  15,
    "PE9:EVENT-OUT"         :  15,
    "PE10:EVENT-OUT"        :  15,
    "PE11:EVENT-OUT"        :  15,
    "PE12:EVENT-OUT"        :  15,
    "PE13:EVENT-OUT"        :  15,
    "PE14:EVENT-OUT"        :  15,
    "PE15:EVENT-OUT"        :  15,
    "PF0:EVENT-OUT"         :  15,
    "PF1:EVENT-OUT"         :  15,
    "PF2:EVENT-OUT"         :  15,
    "PF3:EVENT-OUT"         :  15,
    "PF4:EVENT-OUT"         :  15,
    "PF5:EVENT-OUT"         :  15,
    "PF6:EVENT-OUT"         :  15,
    "PF7:EVENT-OUT"         :  15,
    "PF8:EVENT-OUT"         :  15,
    "PF9:EVENT-OUT"         :  15,
    "PF10:EVENT-OUT"        :  15,
    "PF11:EVENT-OUT"        :  15,
    "PF12:EVENT-OUT"        :  15,
    "PF13:EVENT-OUT"        :  15,
    "PF14:EVENT-OUT"        :  15,
    "PF15:EVENT-OUT"        :  15,
}

ADC1_map = {
    # format is PIN : ADC1_CHAN
    # CH32H417 ADC1 channels
    "PA0"   :   0,
    "PA1"   :   1,
    "PA2"   :   2,
    "PA3"   :   3,
    "PA4"   :   4,
    "PA5"   :   5,
    "PA6"   :   6,
    "PA7"   :   7,
    "PB0"   :   8,
    "PB1"   :   9,
    "PC0"   :  10,
    "PC1"   :  11,
    "PC2"   :  12,
    "PC3"   :  13,
    "PC4"   :  14,
    "PC5"   :  15,
}

# CH32H417 has only one ADC with 16 channels
# ADC2/ADC3 are not present on CH32H417, but we need empty maps to avoid errors
ADC2_map = {
}

ADC3_map = {
}
