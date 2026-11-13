# Common makefile rules for RISC-V (QingKe V5F / CH32H417) ChibiOS builds.
#
# This is the RISC-V counterpart of chibios_common.mk.  It keeps the same
# make variables and targets that Tools/ardupilotwaf/chibios.py expects
# (lib / pass / include_dirs / chibios_flags.h dependency) but drops every
# Cortex-M specific option (THUMB/ARM mode split, -mcpu, FPU, interwork).
#
# MCU_ARCH / MCU_ABI / MCU_CMODEL are supplied by the MCU description module
# through the hwdef build flags.  STARTUPLD comes from adapt-417's
# startup_ch32h417.mk, which is included by the board makefile.

##############################################################################
# Processing options coming from the upper Makefile.
#
OPT    := $(USE_OPT)
COPT   := $(USE_COPT)
CPPOPT := $(USE_CPPOPT)
ASOPT  := $(USE_ASOPT)
ASXOPT := $(USE_ASXOPT)

# Garbage collection
ifeq ($(USE_LINK_GC),yes)
  OPT   += -ffunction-sections -fdata-sections -fno-common
  LDOPT := ,--gc-sections
else
  LDOPT :=
endif

# Linker extra options
ifneq ($(USE_LDOPT),)
  LDOPT := $(LDOPT),$(USE_LDOPT)
endif

# Link time optimizations
ifeq ($(USE_LTO),yes)
  OPT += -flto
endif

# Process stack size
ifeq ($(USE_PROCESS_STACKSIZE),)
  LDOPT := $(LDOPT),--defsym=__process_stack_size__=0x400
else
  LDOPT := $(LDOPT),--defsym=__process_stack_size__=$(USE_PROCESS_STACKSIZE)
endif

# Exceptions stack size
ifeq ($(USE_EXCEPTIONS_STACKSIZE),)
  LDOPT := $(LDOPT),--defsym=__main_stack_size__=0x800
else
  LDOPT := $(LDOPT),--defsym=__main_stack_size__=$(USE_EXCEPTIONS_STACKSIZE)
endif

# Output directory and files
ifeq ($(BUILDDIR),)
  BUILDDIR = build
endif
ifeq ($(BUILDDIR),.)
  BUILDDIR = build
endif
OUTFILES := $(BUILDDIR)/$(PROJECT).elf \
            $(BUILDDIR)/$(PROJECT).hex \
            $(BUILDDIR)/$(PROJECT).bin \
            $(BUILDDIR)/$(PROJECT).dmp \
            $(BUILDDIR)/$(PROJECT).list

ifdef SREC
  OUTFILES += $(BUILDDIR)/$(PROJECT).srec
endif

# Source files groups and paths.  RISC-V has no ARM/THUMB mode split, so
# every C/C++ source goes into the "A" group.
ACSRC     += $(CSRC)
ACPPSRC   += $(CPPSRC)
ASRC      := $(ACSRC) $(ACPPSRC)
TSRC      := $(TCSRC) $(TCPPSRC)
SRCPATHS  := $(sort $(dir $(ASMXSRC)) $(dir $(ASMSRC)) $(dir $(ASRC)) $(dir $(TSRC)))

# Various directories
OBJDIR    := $(BUILDDIR)/obj
LSTDIR    := $(BUILDDIR)/lst

# Object files groups
ACOBJS    := $(addprefix $(OBJDIR)/, $(notdir $(ACSRC:.c=.o)))
ACPPOBJS  := $(addprefix $(OBJDIR)/, $(notdir $(ACPPSRC:.cpp=.o)))
TCOBJS    := $(addprefix $(OBJDIR)/, $(notdir $(TCSRC:.c=.o)))
TCPPOBJS  := $(addprefix $(OBJDIR)/, $(notdir $(TCPPSRC:.cpp=.o)))
ASMOBJS   := $(addprefix $(OBJDIR)/, $(notdir $(ASMSRC:.s=.o)))
ASMXOBJS  := $(addprefix $(OBJDIR)/, $(notdir $(ASMXSRC:.S=.o)))
CRASHCATCHER_ASMXOBJS := $(addprefix $(OBJDIR)/, $(notdir $(CRASHCATCHER_ASMXSRC:.S=.o)))
# sort() also de-duplicates: two source files with the same basename in
# different directories would otherwise produce the same .o target and
# make warns "target given more than once in the same rule".
OBJS      := $(sort $(ASMXOBJS) $(ASMOBJS) $(ACOBJS) $(TCOBJS) $(ACPPOBJS) $(TCPPOBJS))

# Paths
IINCDIR   := $(patsubst %,-I%,$(INCDIR) $(DINCDIR) $(UINCDIR))
LLIBDIR   := $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))
LLIBDIR   += -L$(dir $(LDSCRIPT))

# Macros
DEFS      := $(DDEFS) $(UDEFS)
ADEFS     := $(DADEFS) $(UADEFS)

# Libs
LIBS      := $(DLIBS) $(ULIBS)

# Various settings
MCFLAGS   := -march=$(MCU_ARCH) -mabi=$(MCU_ABI) -mcmodel=$(MCU_CMODEL)
ODFLAGS    = -x --syms
ASFLAGS    = $(MCFLAGS) $(ADEFS) $(ASOPT)
ASXFLAGS   = $(MCFLAGS) $(ADEFS) $(ASXOPT)
CRASHCATCHER_ASXFLAGS = $(ASXFLAGS)
CFLAGS     = $(MCFLAGS) $(OPT) $(COPT) $(CWARN) $(DEFS)
CPPFLAGS   = $(MCFLAGS) $(OPT) $(CPPOPT) $(CPPWARN) $(DEFS)
LDFLAGS    = $(MCFLAGS) $(OPT) -nostartfiles $(LLIBDIR) \
             -Wl,-Map=$(BUILDDIR)/$(PROJECT).map,--cref,--no-warn-mismatch,--library-path=$(STARTUPLD),--script=$(LDSCRIPT)$(LDOPT)

# provide a marker for ArduPilot build options in ChibiOS
CFLAGS    += -D_ARDUPILOT_

# Generate dependency information
ASFLAGS  += -MD -MP -MF .dep/$(@F).d
ASXFLAGS += -MD -MP -MF .dep/$(@F).d
CFLAGS   += -MD -MP -MF .dep/$(@F).d
CPPFLAGS += -MD -MP -MF .dep/$(@F).d

# Paths where to search for sources
VPATH     = $(SRCPATHS)

ifndef ECHO
T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory \
      -nrRf $(firstword $(MAKEFILE_LIST)) \
      ECHO="COUNTTHIS" | grep -c "COUNTTHIS")

N := x
C = $(words $N)$(eval N := x $N)
ECHO = echo "[$C/$T] ChibiOS:"
endif
all: PRE_MAKE_ALL_RULE_HOOK $(OBJS) $(CRASHCATCHER_ASMXOBJS) $(OUTFILES) POST_MAKE_ALL_RULE_HOOK

PRE_MAKE_ALL_RULE_HOOK:

POST_MAKE_ALL_RULE_HOOK:

$(OBJS) $(CRASHCATCHER_ASMXOBJS): | $(BUILDDIR) $(OBJDIR) $(LSTDIR)

$(BUILDDIR):
ifneq ($(USE_VERBOSE_COMPILE),yes)
	@echo Compiler Options
	@echo $(CC) -c $(CFLAGS) -I. $(IINCDIR) main.c -o main.o
	@echo
endif
	@mkdir -p $(BUILDDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(LSTDIR):
	@mkdir -p $(LSTDIR)

$(ACPPOBJS) : $(OBJDIR)/%.o : %.cpp $(BUILDROOT)/chibios_flags.h
ifeq ($(USE_VERBOSE_COMPILE),yes)
	@echo
	$(CPPC) -c $(CPPFLAGS) $(AOPT) -I. $(IINCDIR) $< -o $@
else
	@$(ECHO) Compiling $(<F)
	@$(CPPC) -c $(CPPFLAGS) $(AOPT) -I. $(IINCDIR) $< -o $@
endif

$(TCPPOBJS) : $(OBJDIR)/%.o : %.cpp $(BUILDROOT)/chibios_flags.h
ifeq ($(USE_VERBOSE_COMPILE),yes)
	@echo
	$(CPPC) -c $(CPPFLAGS) $(TOPT) -I. $(IINCDIR) $< -o $@
else
	@$(ECHO) Compiling $(<F)
	@$(CPPC) -c $(CPPFLAGS) $(TOPT) -I. $(IINCDIR) $< -o $@
endif

$(ACOBJS) : $(OBJDIR)/%.o : %.c $(BUILDROOT)/chibios_flags.h
ifeq ($(USE_VERBOSE_COMPILE),yes)
	@echo
	$(CC) -c $(CFLAGS) $(AOPT) -I. $(IINCDIR) $< -o $@
else
	@$(ECHO) Compiling $(<F)
	@$(CC) -c $(CFLAGS) $(AOPT) -I. $(IINCDIR) $< -o $@
endif

$(TCOBJS) : $(OBJDIR)/%.o : %.c $(BUILDROOT)/chibios_flags.h
ifeq ($(USE_VERBOSE_COMPILE),yes)
	@echo
	$(CC) -c $(CFLAGS) $(TOPT) -I. $(IINCDIR) $< -o $@
else
	@$(ECHO) Compiling $(<F)
	@$(CC) -c $(CFLAGS) $(TOPT) -I. $(IINCDIR) $< -o $@
endif

$(ASMOBJS) : $(OBJDIR)/%.o : %.s $(BUILDROOT)/chibios_flags.h
ifeq ($(USE_VERBOSE_COMPILE),yes)
	@echo
	$(AS) -c $(ASFLAGS) -I. $(IINCDIR) $< -o $@
else
	@$(ECHO) Compiling $(<F)
	@$(AS) -c $(ASFLAGS) -I. $(IINCDIR) $< -o $@
endif

$(ASMXOBJS) : $(OBJDIR)/%.o : %.S $(BUILDROOT)/chibios_flags.h
ifeq ($(USE_VERBOSE_COMPILE),yes)
	@echo
	$(CC) -c $(ASXFLAGS) -I. $(IINCDIR) $< -o $@
else
	@$(ECHO) Compiling $(<F)
	@$(CC) -c $(ASXFLAGS) -I. $(IINCDIR) $< -o $@
endif

$(CRASHCATCHER_ASMXOBJS) : $(OBJDIR)/%.o : %.S $(BUILDROOT)/chibios_flags.h
ifeq ($(USE_VERBOSE_COMPILE),yes)
	@echo
	$(CC) -c $(CRASHCATCHER_ASXFLAGS) -I. $(IINCDIR) $< -o $@
else
	@$(ECHO) Compiling $(<F)
	@$(CC) -c $(CRASHCATCHER_ASXFLAGS) -I. $(IINCDIR) $< -o $@
endif

$(BUILDDIR)/$(PROJECT).elf: $(OBJS) $(LDSCRIPT)
ifeq ($(USE_VERBOSE_COMPILE),yes)
	@echo
	$(LD) $(OBJS) $(LDFLAGS) $(LIBS) -o $@
else
	@echo Linking $@
	@$(LD) $(OBJS) $(LDFLAGS) $(LIBS) -o $@
endif

%.hex: %.elf
	$(HEX) $< $@

%.bin: %.elf
	$(BIN) $< $@

%.srec: %.elf
ifdef SREC
	$(SREC) $< $@
endif

%.dmp: %.elf
	@$(OD) $(ODFLAGS) $< > $@
	@$(SZ) $<

%.list: %.elf
	@$(OD) -S $< > $@
	@echo Done

lib: $(OBJS) $(CRASHCATCHER_ASMXOBJS) $(BUILDDIR)/lib$(PROJECT).a pass

$(BUILDDIR)/lib$(PROJECT).a: $(OBJS)
	@$(AR) -r $@ $^
	@echo
	@echo ChibiOS: Done!

pass: $(BUILDDIR)
	@echo $(foreach f,$(IINCDIR),"$(f);") > $(BUILDDIR)/include_dirs

clean: CLEAN_RULE_HOOK
	@echo Cleaning
	-rm -fR .dep $(BUILDDIR)
	@echo
	@echo Done

CLEAN_RULE_HOOK:

#
# Include the dependency files, should be the last of the makefile
#
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***
