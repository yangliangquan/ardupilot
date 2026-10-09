# CCH-v6c V3F reset stub build (WCH QingKe V3F, core0)
#
# Builds the standalone V3F stub ELF/bin and, given a waf-built
# AP_Bootloader.bin, merges the two into a single CodeFlash image:
#
#   make                      build v3f_stub.bin
#   make TOOLCHAIN=... merge AP_BOOT_BIN=build/.../AP_Bootloader.bin
#

ifndef TOOLCHAIN_BIN_DIR
TOOLCHAIN_BIN_DIR := $(HOME)/tools/compiler/MRS_Toolchain_Linux_X64_V240/Toolchain/RISC-V-Embedded-GCC12/bin
endif

PREFIX := $(TOOLCHAIN_BIN_DIR)/
TRGT   := riscv-wch-elf-

CC := $(PREFIX)$(TRGT)gcc
LD := $(PREFIX)$(TRGT)gcc   # cross via gcc since we need nothing beyond libgcc
OBJCOPY := $(PREFIX)$(TRGT)objcopy

# V3F core: same bit-manipulation ISA bundle as the fork demos; ilp32
# (V3F has no FPU)
ARCH_FLAGS := -march=rv32imac_zba_zbb_zbc_zbs_xw -mabi=ilp32 -mcmodel=medlow

OUTDIR := build

all: $(OUTDIR)/v3f_stub.bin

$(OUTDIR)/v3f_stub.elf: v3f_stub.S v3f_stub.ld
	@mkdir -p $(OUTDIR)
	$(CC) -x assembler-with-cpp -c v3f_stub.S -o /tmp/v3f_stub.o $(ARCH_FLAGS) \
		-Wall -Wextra -Werror
	$(LD) -T v3f_stub.ld /tmp/v3f_stub.o -o $(OUTDIR)/v3f_stub.elf \
		-nostdlib -Wl,--gc-sections,-Map=$(OUTDIR)/v3f_stub.map

$(OUTDIR)/v3f_stub.bin: $(OUTDIR)/v3f_stub.elf
	$(OBJCOPY) -O binary $(OUTDIR)/v3f_stub.elf $(OUTDIR)/v3f_stub.bin

# AP_Bootloader.bin lays at the 256KB V5F window in the CodeFlash space;
# the merged image is exactly 960KB (DBMODE=1) and talks to the SW
# flash bootloader entry, so the stub occupies 0x0..0x40000.
CODEFLASH_KB := 960
V5F_OFFSET_KB := 256
BOOTSOURCE := $(AP_BOOT_BIN)

merge: $(OUTDIR)/v3f_stub.bin
	@mkdir -p $(OUTDIR)
	python3 merge_stub_apboot.py $(OUTDIR)/v3f_stub.bin $(BOOTSOURCE) \
		$(CODEFLASH_KB) $(V5F_OFFSET_KB) $(OUTDIR)/CCHv6c_codeflash.bin
	@/bin/echo -e "Done CCHv6c_codeflash.bin"

.PHONY: all merge clean
clean:
	rm -rf $(OUTDIR)
	rm -f /tmp/v3f_stub.o
