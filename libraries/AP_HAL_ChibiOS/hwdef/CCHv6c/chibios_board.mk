##############################################################################
# Board makefile for the CH32H417 (QingKe V5F, RISC-V) CCHv6c board.
#
# This file is picked up by Tools/ardupilotwaf/chibios.py in preference to
# hwdef/common/chibios_board.mk.  It only pulls in the ChibiOS sources from
# the adapt-417 submodule and the ArduPilot HAL common sources; the actual
# compile/link rules live in hwdef/common/chibios_common_riscv.mk.
#
# NOTE(CH32): the hwdef/common/*.c files below were written for STM32.  Their
# CH32-specific register handling is the remaining HAL porting work; the
# build is expected to be completed incrementally on the Linux build host.
#
##############################################################################
# Build global options
#

ifeq ($(USE_OPT),)
  USE_OPT = -fomit-frame-pointer -falign-functions=16 -msave-restore
endif

ifeq ($(ENABLE_DEBUG_SYMBOLS), yes)
  USE_OPT += -g3
endif

ifeq ($(USE_COPT),)
  USE_COPT = -Os
endif

ifeq ($(USE_CPPOPT),)
  USE_CPPOPT = -fno-rtti -std=gnu++11
endif

ifeq ($(USE_ASOPT),)
  USE_ASOPT =
endif

ifeq ($(USE_ASXOPT),)
  USE_ASXOPT =
endif

ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

ifeq ($(USE_LDOPT),)
  USE_LDOPT =
endif

ifeq ($(USE_LTO),)
  USE_LTO = no
endif

ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

ifeq ($(USE_SMART_BUILD),)
  USE_SMART_BUILD = no
endif

ifeq ($(USE_PROCESS_STACKSIZE),)
  USE_PROCESS_STACKSIZE = 0x6000
endif

ifeq ($(USE_EXCEPTIONS_STACKSIZE),)
  USE_EXCEPTIONS_STACKSIZE = 0x2000
endif

HWDEF = $(AP_HAL)/hwdef

ifeq ($(USE_FATFS),yes)
include $(CHIBIOS)/os/various/cpp_wrappers/chcpp.mk
include $(CHIBIOS)/os/various/fatfs_bindings/fatfs.mk
endif

#
# Build global options
##############################################################################

##############################################################################
# Project, sources and paths
#

PROJECT = ch

# Startup / HAL / RTOS, all from the adapt-417 ChibiOS submodule.
include $(CHIBIOS)/os/license/license.mk
include $(CHIBIOS)/$(CHIBIOS_STARTUP_MK)
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/$(CHIBIOS_PLATFORM_MK)
include $(CHIBIOS)/os/hal/osal/rt-nil/osal.mk
# NOTE: adapt-417's CUSTOM_CH32H417/board.mk is deliberately NOT included.
# It adds its own board.c (a duplicate board.o target) and puts a board.h in
# the include path that shadows AP_HAL's board.h (losing
# HAL_BOARD_INIT_HOOK_CALL).  This board uses hwdef/common/board_ch32.c.
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/RISCV-ECLIC/compilers/GCC/mk/port.mk
include $(CHIBIOS)/os/hal/lib/streams/streams.mk

# hwdef-generated linker script.  STARTUPLD is set by startup_ch32h417.mk.
LDSCRIPT ?= $(BUILDROOT)/ldscript.ld

CSRC = $(sort $(ALLCSRC))

CSRC += $(HWDEF)/common/stubs.c \
        $(HWDEF)/common/board_ch32.c \
        $(HWDEF)/common/usbcfg.c \
        $(HWDEF)/common/usbcfg_common.c \
        $(HWDEF)/common/flash.c \
        $(HWDEF)/common/malloc.c \
        $(HWDEF)/common/hrt.c \
        $(HWDEF)/common/stm32_util.c \
        $(HWDEF)/common/bouncebuffer.c \
        $(HWDEF)/common/watchdog.c \
        $(HWDEF)/common/sysperf.c

ifeq ($(USE_USB_MSD),yes)
CSRC += $(CHIBIOS)/os/various/scsi_bindings/lib_scsi.c \
        $(CHIBIOS)/os/hal/src/hal_usb_msd.c
endif

# fatfs.mk (included above when USE_FATFS=yes) already adds
# fatfs_syscall.c to ALLCSRC; adding it again creates a duplicate
# fatfs_syscall.o target.

CPPSRC = $(sort $(ALLCPPSRC))

# RISC-V has no ARM/THUMB mode split.
ACSRC =
ACPPSRC =
TCSRC =
TCPPSRC =

ASMSRC  = $(ALLASMSRC)
ASMXSRC = $(ALLXASMSRC)

# CrashCatcher_armv7m_asm.S is Cortex-M only; no equivalent on RISC-V yet.
CRASHCATCHER_ASMXSRC =

INCDIR = $(CHIBIOS)/os/license \
         $(ALLINC) $(HWDEF)/common $(BUILDROOT)

ifeq ($(ENABLE_CRASHDUMP),yes)
INCDIR += $(AP_HAL)
endif

ifeq ($(USE_USB_MSD),yes)
INCDIR += $(CHIBIOS)/os/various/scsi_bindings
endif

#
# Project, sources and paths
##############################################################################

##############################################################################
# Compiler settings
#

TRGT = riscv-wch-elf-
CC   = $(TRGT)gcc
CPPC = $(TRGT)g++
LD   = $(TRGT)gcc
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
AR   = $(TRGT)ar
OD   = $(TRGT)objdump
SZ   = $(TRGT)size
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary

AOPT =
TOPT =

CWARN   = -Wall -Wextra -Wundef -Wstrict-prototypes -Werror
CPPWARN = -Wall -Wextra -Wundef -Werror

#
# Compiler settings
##############################################################################

##############################################################################
# User defines
#

UDEFS = $(ENV_UDEFS) $(FATFS_FLAGS) -DHAL_BOARD_NAME=\"$(HAL_BOARD_NAME)\" \
        -DHAL_MAX_STACK_FRAME_SIZE=$(HAL_MAX_STACK_FRAME_SIZE) \
        -DCRT0_INIT_TEXT=0 -DCRT1_AREAS_NUMBER=3

# crt0.S is assembled from the ChibiOS library, so the "do not copy the
# whole .text into ITCM" switch must also reach the assembler.
UADEFS = -DCRT0_INIT_TEXT=0

ifeq ($(ENABLE_ASSERTS),yes)
 UDEFS += -DHAL_CHIBIOS_ENABLE_ASSERTS
 UADEFS += -DHAL_CHIBIOS_ENABLE_ASSERTS
endif

ifeq ($(ENABLE_MALLOC_GUARD),yes)
 UDEFS += -DHAL_CHIBIOS_ENABLE_MALLOC_GUARD
endif

ifeq ($(ENABLE_STATS),yes)
 UDEFS += -DHAL_ENABLE_THREAD_STATISTICS
endif

ifneq ($(AP_BOARD_START_TIME),)
 UDEFS += -DAP_BOARD_START_TIME=$(AP_BOARD_START_TIME)
endif

UINCDIR =
ULIBDIR =
ULIBS =

#
# End of user defines
##############################################################################
include $(HWDEF)/common/chibios_common_riscv.mk

ifeq ($(USE_USB_MSD),yes)
$(OBJDIR)/hal_usb_msd.o: CWARN += -Wno-unterminated-string-initialization
endif
