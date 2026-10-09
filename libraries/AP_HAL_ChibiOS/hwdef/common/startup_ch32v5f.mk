# Startup build file for CH32H417 V5F core, ArduPilot application variant.
#
# Differences from the WCH fork startup.mk (os/common/startup/RISCV-QINGKE/
# CH32H417/V5F/mk/startup.mk) used for the internal-flash bootloader:
# - crt0_ch32v5f.S: XIP aware crt0 that does not touch the system clock
# - vector.S, crt1.c and core_riscv.c come from the fork/startup tree

STARTUPASM = $(CHIBIOS)/os/common/startup/RISCV-QINGKE/CH32H417/V5F/vector.S
STARTUPASM += $(AP_HAL)/hwdef/common/crt0_ch32v5f.S

STARTUPSRC = $(CHIBIOS)/os/common/startup/RISCV-QINGKE/compilers/GCC/crt1.c
STARTUPSRC += $(CHIBIOS)/os/common/ext/RISCV/QINGKE/Core/V5F/core_riscv.c

STARTUPINC = $(CHIBIOS)/os/common/portability/GCC \
             $(CHIBIOS)/os/common/startup/RISCV-QINGKE/CH32H417/V5F \
             $(CHIBIOS)/os/common/ext/WCH/CH32H417 \
             $(CHIBIOS)/os/common/ext/RISCV/QINGKE/Core/V5F

STARTUPLD  = $(CHIBIOS)/os/common/startup/RISCV-QINGKE/CH32H417/V5F

ALLXASMSRC += $(STARTUPASM)
ALLCSRC    += $(STARTUPSRC)
ALLINC     += $(STARTUPINC)
