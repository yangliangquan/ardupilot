/*
 * ArduPilot / ChibiOS CH32H417 (QingKe V5F, RISC-V) linker rules.
 *
 * Shared by BOTH images; the generated ldscript.ld decides where
 * "default_flash" points through a REGION_ALIAS:
 *
 *   flight firmware : default_flash -> the app image window chosen by
 *                     write_ch32_ldscript() from hwdef.dat: external
 *                     QSPI2 NOR XIP at 0x70000000 (4M, ch32h417-port)
 *                     or internal CodeFlash at 0x0000F000 (896K,
 *                     ch32h417-port-crap)
 *   bootloader      : default_flash -> "flash", the internal CodeFlash
 *                     bootloader area at 0x00001000 (zero wait states)
 *
 * Memory layout (see CH32H417xx.py / ch32h417_memory.h for the numbers):
 *
 *   default_flash : .crt0, load images for ITCM/SRAM/DTCM, .text,
 *                   .rodata, initialisers (all XIP; both the internal
 *                   CodeFlash and any external XIP window only give
 *                   ~20MB/s of fetch bandwidth)
 *   instram       : ITCM, 0x200A0000, 128K
 *                   -> ECLIC vector table (.vectors), __FASTRAMFUNC__ /
 *                      section(".itcm") code, plus the whole inner-loop
 *                      objects listed in .itcm_init (rate loop, motor
 *                      mixing, IMU) - copied at boot
 *   flashram      : shared SRAM, 0x20100000, 512K
 *                   -> EKF3 / AHRS / position nav / math objects listed in
 *                      .sram_init (code + their .rodata), plus __SRAMFUNC__
 *                      / __RAMFUNC__ / .sram code (copied at boot), .ram2
 *                      USB buffers, and the default heap in whatever is
 *                      left above them (HEAP_RAM alias, RAM_MAP[0])
 *   ram0          : DTCM, 0x200C0200 (first 512B of DTCM are hardware owned)
 *                   -> .ram0_init hot code (copied at boot), .data/.sdata,
 *                      .bss/.sbss, all stacks; no heap
 *
 * The itcm_init/sram_init rules MUST stay above the .text rule: GNU ld
 * places an input section with the first matching pattern, and the listed
 * objects must be stolen out of the .text/.rodata catch-alls.
 *
 * The boot path is: internal-flash bootloader validates the app image and
 * jumps to APP_START_ADDRESS (0x70000000 on port, 0x0000F000 on crap) in
 * the flight image.  Both the internal CodeFlash and the QSPI XIP window
 * have the same (slow) fetch bandwidth, so the hot flight path is
 * relocated into ITCM/SRAM at boot instead of being XIP'd; everything
 * else stays XIP.
 *
 * The ChibiOS adapt-417 crt0.S/crt1.c do all runtime relocation:
 *   - crt0 copies .data and clears .bss, using __textdata_base__,
 *     __data_base__/__data_end__, __bss_base__/__bss_end__ below.
 *   - crt1 __init_ram_areas() copies/zeroes the three areas 0..2 using
 *     __ramN_init_text__, __ramN_init__, __ramN_clear__, __ramN_noinit__.
 *     Area 0 = DTCM, area 1 = ITCM, area 2 = shared SRAM.
 *
 * IMPORTANT: build with -DCRT0_INIT_TEXT=0.  adapt-417 defaults it to TRUE
 * so a standalone demo can copy its whole .text into ITCM; here the image
 * is XIP from CodeFlash and only the opted-in sections are relocated.
 *
 * This file is copied to build/common.ld and INCLUDEd by the hwdef-generated
 * ldscript.ld, which defines the MEMORY regions and, for the bootloader,
 * REGION_ALIAS("default_flash", flash) before the INCLUDE.
 *
 * Required linker --defsym (supplied by boards.py LINKFLAGS):
 *   __process_stack_size__, __main_stack_size__
 */

/* Every variable and both stacks live in DTCM.  The heap shares the
   shared-SRAM region with the copied .sram code: .heap is the last
   section placed there, so __heap_base__ lands above all static
   content and RAM_MAP[0] (= FLASH_RAM) describes the whole region,
   the classic STM32 ram0 pattern. */
REGION_ALIAS("MAIN_STACK_RAM", ram0);
REGION_ALIAS("PROCESS_STACK_RAM", ram0);
REGION_ALIAS("DATA_RAM", ram0);
REGION_ALIAS("BSS_RAM", ram0);
REGION_ALIAS("HEAP_RAM", flashram);

__ram0_start__  = ORIGIN(ram0);
__ram0_size__   = LENGTH(ram0);
__ram0_end__    = __ram0_start__ + __ram0_size__;
__itcm_start__  = ORIGIN(instram);
__itcm_size__   = LENGTH(instram);
__itcm_end__    = __itcm_start__ + __itcm_size__;
__sram_start__  = ORIGIN(flashram);
__sram_size__   = LENGTH(flashram);
__sram_end__    = __sram_start__ + __sram_size__;
__flash_start__ = ORIGIN(default_flash);
__flash_size__  = LENGTH(default_flash);
__flash_end__   = __flash_start__ + __flash_size__;

ENTRY(_start)

SECTIONS
{
    /* Reset entry code.  _start must be the very first byte of the
       external flash image: the bootloader jumps to ORIGIN(default_flash). */
    .crt0 : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(SORT_NONE(.crt0)))
    } > default_flash
    __firmware_crc_start__ = LOADADDR(.crt0);
    __firmware_crc_ext_start__ = 0;
    __firmware_crc_ext_end__ = 0;

    constructors : ALIGN(4) SUBALIGN(4)
    {
        __init_array_base__ = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end__ = .;
    } > default_flash

    destructors : ALIGN(4) SUBALIGN(4)
    {
        __fini_array_base__ = .;
        KEEP(*(.fini_array))
        KEEP(*(SORT(.fini_array.*)))
        __fini_array_end__ = .;
    } > default_flash

    /* ---------------- CRT1 area 1: ITCM hot code ----------------
       Must be declared BEFORE the .text rule below: GNU ld places an input
       section with the FIRST matching pattern, so these rules steal the
       hottest objects' .text out of the XIP catch-all.  (CodeFlash XIP
       fetch bandwidth is the bottleneck, so whole hot objects are relocated
       into ITCM instead of relying only on per-function __FASTRAMFUNC__
       tags.)  VMA = ITCM, load image AT > default_flash, copied by crt1.
       Members' read-only data (float constants, vtables) goes to SRAM so
       the relocated code never fetches constants through the flash bus. */
    .itcm_init : ALIGN(4) SUBALIGN(4)
    {
        __ram1_init_text__ = LOADADDR(.itcm_init);
        __ram1_init__ = .;
        /* ECLIC vector table: fetched on every interrupt entry, so it lives
           in zero-wait-state ITCM instead of the slow QSPI XIP window.
           vectors.S aligns it on a 1024 byte boundary (.balign 1024) as
           required by mtvec[9:2] being hardwired to 0 on QingKe V5F, and
           since this section starts at ORIGIN(instram) the alignment holds
           for both the VMA and the 1024 padded load image.  crt0 programs
           mtvec with the (ITCM) address of _vector_base before crt1 copies
           this area; interrupts are globally disabled (MIE=0) until after
           the copy, and the slot entries are absolute handler addresses
           which are unaffected by the move. */
        __vectors_base__ = .;
        KEEP(*(SORT_NONE(.vectors)))
        __vectors_end__ = .;
        KEEP(*(.itcm))
        KEEP(*(.itcm.*))
        KEEP(*(.fastramfunc))
        KEEP(*(.fastramfunc.*))
        /* --- bulk-relocated inner-loop objects (rate loop / motor mixing /
           IMU sampling).  Keep this list inside the ITCM 128K budget. --- */
        *libArduCopter_libs.a:AC_PID.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:AC_AttitudeControl.cpp.3.o(.text .text.*)
        *libArduCopter_libs.a:AC_AttitudeControl_Multi.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:AC_PosControl.cpp.3.o(.text .text.*)
        *libArduCopter_libs.a:AP_Motors.cpp.3.o(.text .text.*)
        *libArduCopter_libs.a:AP_MotorsMatrix.cpp.3.o(.text .text.*)
        *libArduCopter_libs.a:AP_MotorsMulticopter.cpp.3.o(.text .text.*)
        *libArduCopter_libs.a:AP_InertialSensor.cpp.3.o(.text .text.*)
        *libArduCopter_libs.a:AP_InertialSensor_Invensensev3.cpp.3.o(.text .text.*)
        *libArduCopter_libs.a:HarmonicNotchFilter.cpp.3.o(.text .text.*)
        *Attitude.cpp.101.o(.text .text.*)
        *motors.cpp.101.o(.text .text.*)
        *land_detector.cpp.101.o(.text .text.*)
        *rate_thread.cpp.101.o(.text .text.*)
        /* --- interrupt-path and bus objects: serial ISR + DMA, DShot/PWM
           output at the motor update rate, the 1kHz scheduler dispatch,
           INS bus transactions at the gyro ODR, ChibiOS kernel switches
           and the out-of-line sample filters.  Soft-float (rv32imac, no
           F extension) makes the libgcc/libm helpers on the next rate
           loop path worth the space.  Their .rodata goes to SRAM below. --- */
        *libArduCopter_libs.a:UARTDriver.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:RCOutput.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:AP_Scheduler.cpp.3.o(.text .text.*)
        *libArduCopter_libs.a:Scheduler.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:SPIDevice.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:I2CDevice.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:Device.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:NotchFilter.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:LowPassFilter.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:LowPassFilter2p.cpp.0.o(.text .text.*)
        *libch.a:chschd.o(.text .text.*)
        *libch.a:chmtx.o(.text .text.*)
        *libch.a:chsem.o(.text .text.*)
        *libch.a:chthreads.o(.text .text.*)
        *libch.a:chsys.o(.text .text.*)
        /* --- control math helpers: the emitted definitions live in
           control.cpp / AP_Math.cpp, matched with a wildcard so a changed
           object suffix cannot silently send them back to the XIP flash --- */
        *control.cpp.*.o(.text .text.*)
        *AP_Math.cpp.*.o(.text .text.*)
        /* --- C library memory and string primitives: called from every
           subsystem (buffers, MAVLink, logging, parameters), only a few
           hundred bytes but on the hottest paths of the main loop --- */
        *libc_nano.a:libc_a-memcpy-asm.o(.text .text.*)
        *libc_nano.a:libc_a-memset.o(.text .text.*)
        *libc_nano.a:libc_a-memmove.o(.text .text.*)
        *libc_nano.a:libc_a-memcmp.o(.text .text.*)
        *libc_nano.a:libc_a-memchr.o(.text .text.*)
        *libc_nano.a:libc_a-strlen.o(.text .text.*)
        *libc_nano.a:libc_a-strcmp.o(.text .text.*)
        *libc_nano.a:libc_a-strncmp.o(.text .text.*)
        *libc_nano.a:libc_a-strchr.o(.text .text.*)
        /* --- executed first on every iteration: sensor acquisition --- */
        *ArduCopter/sensors.cpp.*.o(.text .text.*)
        /* --- ITCM fill (2026-11-21): small per-loop/per-frame functions;
           their .rodata goes to .sram_init (rules below).  Heap starvation
           from the 80KB logger buffer broke the IMU in this layout, fixed
           by HAL_LOGGING_FILE_BUFSIZE 32 in hwdef.dat. --- */
        *libArduCopter_libs.a:AP_AHRS_NavEKF3.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:crc.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:print_vprintf.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:SRV_Channel_aux.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:AP_RCProtocol_CRSF.cpp.3.o(.text .text.*)
        *libArduCopter_libs.a:GPIO.cpp.3.o(.text .text.*)
        *libArduCopter_libs.a:GPS_Backend.cpp.0.o(.text .text.*)
        . = ALIGN(4);
    } > instram AT > default_flash

    /* ---------------- CRT1 area 2: shared SRAM hot code ----------------
       Larger but still performance sensitive code (EKF3, AHRS, math,
       position nav) relocated wholesale from the XIP image; functions can
       also be tagged with __SRAMFUNC__ / __RAMFUNC__ / section(".sram").
       The bulk-relocated members also drop their .rodata here so hot float
       constants and vtables are read from SRAM, not the flash bus.  NOTE:
       .srodata must stay in flash - it is addressed gp-relative. */
    .sram_init : ALIGN(4) SUBALIGN(4)
    {
        __ram2_init_text__ = LOADADDR(.sram_init);
        __ram2_init__ = .;
        KEEP(*(.sram))
        KEEP(*(.sram.*))
        KEEP(*(.ramfunc))
        KEEP(*(.ramfunc.*))
        /* --- EKF3 (runs at the IMU rate) --- */
        *libArduCopter_libs.a:AP_NavEKF3_core.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_NavEKF3.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_NavEKF3_MagFusion.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_NavEKF3_PosVelFusion.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_NavEKF3_Measurements.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_NavEKF3_Control.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_NavEKF3_Outputs.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_NavEKF3_VehicleStatus.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_NavEKF3_AirDataFusion.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:EKFGSF_yaw.cpp.0.o(.text .text.* .rodata .rodata.*)
        /* --- AHRS + position nav + math helpers --- */
        *libArduCopter_libs.a:AP_AHRS.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_AHRS_DCM.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AC_WPNav.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:matrix_alg.cpp.0.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:quaternion.cpp.0.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:vector3.cpp.0.o(.text .text.* .rodata .rodata.*)
        /* --- rodata of the ITCM-resident objects (constants + vtables) --- */
        *libArduCopter_libs.a:AC_PID.cpp.0.o(.rodata .rodata.*)
        *libArduCopter_libs.a:AC_AttitudeControl.cpp.3.o(.rodata .rodata.*)
        *libArduCopter_libs.a:AC_AttitudeControl_Multi.cpp.0.o(.rodata .rodata.*)
        *libArduCopter_libs.a:AC_PosControl.cpp.3.o(.rodata .rodata.*)
        *libArduCopter_libs.a:AP_Motors.cpp.3.o(.rodata .rodata.*)
        *libArduCopter_libs.a:AP_MotorsMatrix.cpp.3.o(.rodata .rodata.*)
        *libArduCopter_libs.a:AP_MotorsMulticopter.cpp.3.o(.rodata .rodata.*)
        *libArduCopter_libs.a:AP_InertialSensor.cpp.3.o(.rodata .rodata.*)
        *libArduCopter_libs.a:AP_InertialSensor_Invensensev3.cpp.3.o(.rodata .rodata.*)
        *libArduCopter_libs.a:HarmonicNotchFilter.cpp.3.o(.rodata .rodata.*)
        *libArduCopter_libs.a:UARTDriver.cpp.0.o(.rodata .rodata.*)
        *libArduCopter_libs.a:RCOutput.cpp.0.o(.rodata .rodata.*)
        *libArduCopter_libs.a:AP_Scheduler.cpp.3.o(.rodata .rodata.*)
        *libArduCopter_libs.a:Scheduler.cpp.0.o(.rodata .rodata.*)
        *libArduCopter_libs.a:SPIDevice.cpp.0.o(.rodata .rodata.*)
        *libArduCopter_libs.a:I2CDevice.cpp.0.o(.rodata .rodata.*)
        *libArduCopter_libs.a:Device.cpp.0.o(.rodata .rodata.*)
        *libArduCopter_libs.a:NotchFilter.cpp.0.o(.rodata .rodata.*)
        *libArduCopter_libs.a:LowPassFilter.cpp.0.o(.rodata .rodata.*)
        *libArduCopter_libs.a:LowPassFilter2p.cpp.0.o(.rodata .rodata.*)
        *libArduCopter_libs.a:GCS_MAVLink.cpp.0.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:GCS_Common.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Logger.cpp.3.o(.text .text.* .rodata .rodata.*)
        *Log.cpp.101.o(.text .text.* .rodata .rodata.*)
        *libm_nano.a:*.o(.text .text.* .rodata .rodata.*)
        *libgcc.a:*.o(.text .text.*)
        /* --- ArduCopter vehicle code: flight mode dispatch, fast loop,
           sensor loop, telemetry and EKF logging all run every iteration --- */
        *ArduCopter/*.101.o(.text .text.* .rodata .rodata.*)
        /* --- per-loop support libraries (wildcards so a changed object
           suffix can never silently send them back to the XIP flash) --- */
        *AP_Param.cpp.*.o(.text .text.* .rodata .rodata.*)
        *AP_Arming.cpp.*.o(.text .text.* .rodata .rodata.*)
        *AP_BattMonitor.cpp.*.o(.text .text.* .rodata .rodata.*)
        *AP_RCProtocol.cpp.*.o(.text .text.* .rodata .rodata.*)
        *AP_Notify.cpp.*.o(.text .text.* .rodata .rodata.*)
        *AP_Mission.cpp.*.o(.text .text.* .rodata .rodata.*)
        *AP_GPS.cpp.*.o(.text .text.* .rodata .rodata.*)
        /* --- bisect Test A (2026-11-21): batch3 subset, ~10.8K, keeps the
           heap at 156.8K (= the known-good 01:14 build).  If the IMU dies
           with this build the heap is exonerated and one of these five
           objects is the culprit. --- */
        *libArduCopter_libs.a:SCurve.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_AHRS_NavEKF3.cpp.0.o(.text .text.*)
        *libArduCopter_libs.a:AP_NavEKF3_Logging.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:crc.cpp.0.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:print_vprintf.cpp.0.o(.text .text.* .rodata .rodata.*)
        /* --- ITCM fill objects' .rodata (their .text goes to .itcm_init
           above; first-match: the .text is claimed there, .rodata falls
           through to here) --- */
        *libArduCopter_libs.a:SRV_Channel_aux.cpp.0.o(.rodata .rodata.*)
        *libArduCopter_libs.a:AP_RCProtocol_CRSF.cpp.3.o(.rodata .rodata.*)
        *libArduCopter_libs.a:GPIO.cpp.3.o(.rodata .rodata.*)
        *libArduCopter_libs.a:GPS_Backend.cpp.0.o(.rodata .rodata.*)
        /* --- batch3 remainder (2026-11-21): baro/ESC/GPS-RC frontends and
           the SD logging write path (FatFS + AP_Logger file backends +
           FATFS filesystem layer); viable since HAL_LOGGING_FILE_BUFSIZE 32
           freed the heap.  On the QSPI XIP layout (port) this block demotes
           to the cached flash window instead: the full firmware overflows
           the 512K SRAM by ~20K and none of it is on the per-loop path. --- */
        IF (ORIGIN(default_flash) < 0x40000000)
        *libArduCopter_libs.a:AP_Baro_MS5611.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_ESC_Telem.cpp.0.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_ESC_Telem_Backend.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libch.a:ff.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Logger_File.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:LogFile.cpp.0.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Logger_Backend.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Logger_MAVLink.cpp.0.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Filesystem.cpp.0.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Filesystem_FATFS.cpp.0.o(.text .text.* .rodata .rodata.*)
        ENDIF
        . = ALIGN(4);
    } > flashram AT > default_flash

    IF (ORIGIN(default_flash) >= 0x40000000)
    /* port-only: the demoted batch3 block above, XIP from the icache MEM0
       window.  Kept as its own section before .ram0_init/.text so the
       patterns win first-match placement against the catch-alls. */
    .sram_demote : ALIGN(4) SUBALIGN(4)
    {
        *libArduCopter_libs.a:AP_Baro_MS5611.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_ESC_Telem.cpp.0.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_ESC_Telem_Backend.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libch.a:ff.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Logger_File.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:LogFile.cpp.0.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Logger_Backend.cpp.3.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Logger_MAVLink.cpp.0.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Filesystem.cpp.0.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Filesystem_FATFS.cpp.0.o(.text .text.* .rodata .rodata.*)
        . = ALIGN(4);
    } > default_flash
    ENDIF

    /* ---------------- CRT1 area 0: DTCM hot code ----------------
       Third code layer.  ITCM (128K) and shared SRAM (512K minus heap)
       are already full of relocated hot code, so the next tier of
       flash-resident objects is relocated into DTCM here.  DTCM is a
       zero-wait-state memory in the 0x20000000 window (SRAM_AREA=0: the
       icache never caches it), so relocated code executes without the
       ~20MB/s CodeFlash fetch bottleneck.  Must stay BEFORE the .text
       rule: GNU ld places an input section with the first matching
       pattern.  crt0.S relocates this area inline in assembly (no
       flash-resident relocation table is read at boot). */
    .ram0_init : ALIGN(4) SUBALIGN(4)
    {
        __ram0_init_text__ = LOADADDR(.ram0_init);
        __ram0_init__ = .;
        /* --- DroneCAN/CAN service stack: CAN1+CAN3 carry DroneCAN at
           1Mbit/s; the service threads run continuously --- */
        *libArduCopter_libs.a:AP_DroneCAN.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_DroneCAN_DNA_Server.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_GPS_DroneCAN.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_BattMonitor_DroneCAN.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:canard.c.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Canard_iface.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:CanIface.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_SLCANIface.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_MAVLinkCAN.cpp.*.o(.text .text.* .rodata .rodata.*)
        /* --- GPS frontends: parse every received byte of the streams --- */
        *libArduCopter_libs.a:AP_GPS_UBLOX.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_GPS_NMEA.cpp.*.o(.text .text.* .rodata .rodata.*)
        /* --- compass and baro frontends: read on the main sensor loop --- */
        *libArduCopter_libs.a:AP_Compass.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Baro.cpp.*.o(.text .text.* .rodata .rodata.*)
        /* --- per-loop rc/vehicle/hal entry code --- */
        *libArduCopter_libs.a:RC_Channel.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:AP_Vehicle.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:HAL_ChibiOS_Class.cpp.*.o(.text .text.* .rodata .rodata.*)
        /* --- GCS request handlers and message routing --- */
        *libArduCopter_libs.a:GCS_FTP.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:GCS_Param.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:MAVLink_routing.cpp.*.o(.text .text.* .rodata .rodata.*)
        /* --- misc per-loop helpers --- */
        *libArduCopter_libs.a:Util.cpp.*.o(.text .text.* .rodata .rodata.*)
        *libArduCopter_libs.a:Location.cpp.*.o(.text .text.* .rodata .rodata.*)
        /* --- libc float/string conversion (param + MAVLink text paths) --- */
        *libc_nano.a:libc_a-strtod.o(.text .text.* .rodata .rodata.*)
        *libc_nano.a:libc_a-mprec.o(.text .text.* .rodata .rodata.*)
        /* --- ChibiOS SPI lld: every IMU/baro bus transaction --- */
        *libch.a:hal_spi_v2_lld.o(.text .text.* .rodata .rodata.*)
        . = ALIGN(4);
    } > ram0 AT > default_flash

    /* Main body of the firmware: XIP from the internal CodeFlash.
       app_descriptor stays near the start so the bootloader finds it
       before any embedded ROMFS can produce a false positive. */
    .text : ALIGN(4) SUBALIGN(4)
    {
        __text_base__ = .;
        KEEP(*(.app_descriptor))
        KEEP(*(.apsec_data))
        PROVIDE(__usb_debug_text_start = .);
        *USB_Debug.cpp*.o(.text .text.*)
        PROVIDE(__usb_debug_text_end = .);
        *(.text)
        *(.text.*)
        *(.gcc*)
        __text_end__ = .;
    } > default_flash
    __text_lma = LOADADDR(.text);

    .rodata : ALIGN(4)
    {
        __rodata_base__ = .;
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)
        . = ALIGN(4);
        __rodata_end__ = .;
    } > default_flash

    .eh_frame_hdr : { *(.eh_frame_hdr) } > default_flash
    .eh_frame : ONLY_IF_RO { *(.eh_frame) } > default_flash
    .gcc_except_table : { *(.gcc_except_table .gcc_except_table.*) } > default_flash
    .textalign : ONLY_IF_RO { . = ALIGN(8); } > default_flash

    PROVIDE(_etext = .);

    /* The .itcm_init / .sram_init rules live ABOVE the .text rule (GNU ld
       first-match placement); the NOLOAD bss/noinit areas follow here. */

    .ram1 (NOLOAD) : ALIGN(4)
    {
        __ram1_clear__ = .;
        *(.ram1_clear)
        *(.ram1_clear.*)
        . = ALIGN(4);
        __ram1_noinit__ = .;
        *(.ram1)
        *(.ram1.*)
        . = ALIGN(4);
        __ram1_free__ = .;
    } > instram

    .ram2 (NOLOAD) : ALIGN(4)
    {
        __ram2_clear__ = .;
        *(.ram2_clear)
        *(.ram2_clear.*)
        . = ALIGN(4);
        __ram2_noinit__ = .;
        *(.ram2)
        *(.ram2.*)
        . = ALIGN(4);
        __ram2_free__ = .;
    } > flashram

    /* The .ram0_init hot-code block lives above the .text rule (GNU ld
       first-match placement).  DTCM data follows as NOLOAD sections. */

    .ram0 (NOLOAD) : ALIGN(4)
    {
        __ram0_clear__ = .;
        *(.ram0_clear)
        *(.ram0_clear.*)
        . = ALIGN(4);
        __ram0_noinit__ = .;
        *(.ram0)
        *(.ram0.*)
        . = ALIGN(4);
        __ram0_free__ = .;
    } > ram0

    /* Independent interrupt stack. Interrupt frames of nested levels and the
       C call stack of all the handlers are allocated here instead of on the
       stack of the interrupted thread, see PORT_RISCV_USE_IRQ_STACK. */
    .istack (NOLOAD) :
    {
        . = ALIGN(16);
        __irq_stack_base__ = .;
        __main_stack_base__ = .;
        . += __main_stack_size__;
        . = ALIGN(16);
        __irq_stack_end__ = .;
        __main_stack_end__ = .;
    } > MAIN_STACK_RAM

    /* Process stack, also used by crt0 as the initial sp. */
    .pstack (NOLOAD) : ALIGN(16)
    {
        __process_stack_base__ = .;
        __main_thread_stack_base__ = .;
        . += __process_stack_size__;
        . = ALIGN(16);
        __process_stack_end__ = .;
        __main_thread_stack_end__ = .;
    } > PROCESS_STACK_RAM

    /* All mutable variables (data/sdata/bss/sbss) in DTCM. */
    .data : ALIGN(4)
    {
        PROVIDE(_textdata = LOADADDR(.data));
        PROVIDE(_data = .);
        __textdata_base__ = LOADADDR(.data);
        __data_base__ = .;
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
        . = ALIGN(8);
        PROVIDE(__global_pointer$ = . + 0x800);
        *(.sdata)
        *(.sdata.*)
        *(.ramtext)
        *(.gnu.linkonce.s.*)
        . = ALIGN(4);
        PROVIDE(_edata = .);
        __data_end__ = .;
    } > DATA_RAM AT > default_flash

    .bss (NOLOAD) : ALIGN(4)
    {
        __bss_base__ = .;
        *(.sbss)
        *(.sbss.*)
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        PROVIDE(end = .);
    } > BSS_RAM

    .heap (NOLOAD) : ALIGN(8)
    {
        __heap_base__ = .;
        . = ORIGIN(HEAP_RAM) + LENGTH(HEAP_RAM);
        __heap_end__ = .;
    } > HEAP_RAM

    /* Last load-image section placed in the flash window: .data is the
       final section with a flash load image (.bss/.heap are NOLOAD), so
       its LMA range covers every relocated area's load image (.crt0,
       .itcm_init, .sram_init, .ram0_init, .text, .rodata).  The
       CRC/self-check must cover the WHOLE image, not just the tail. */
    __firmware_crc_end__ = LOADADDR(.data) + SIZEOF(.data);
    __flash_image_end__ = LOADADDR(.data) + SIZEOF(.data);
    ASSERT(__flash_image_end__ <= __flash_end__, "ERROR: ArduPilot image overflows the flash window; disable more features in hwdef.dat (see the 'flash fit trimming' block)")

    /* Crash log occupies the unused tail of the flash window, mirroring
       common_extf_h750.ld: reserve 16K at the end.  If the image already
       leaves less than 16K (the internal-CodeFlash build) the reservation
       collapses to zero size rather than failing the link.  Symbols only
       - nothing is preloaded there and forcing the location counter into
       the window would trip "cannot move location counter backwards"
       when default_flash sits below the RAM regions. */
    PROVIDE(__crash_log_base__ = MAX(__flash_image_end__, __flash_end__ - 16K));
    PROVIDE(__crash_log_end__ = __flash_end__);
}
