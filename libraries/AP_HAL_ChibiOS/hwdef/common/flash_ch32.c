/*
 * CH32H417 flash backend for AP_HAL_ChibiOS.
 *
 * There is deliberately no flash region that the flight firmware may erase
 * or program at run time on this board:
 *
 *   - the internal CodeFlash is owned by the internal-flash bootloader;
 *   - the flight image executes in place from the external QSPI2 NOR at
 *     0x70000000, so erasing or programming that device would stall the
 *     very instruction stream the CPU is fetching from;
 *   - parameters are stored in the SPI FRAM (HAL_WITH_RAMTRON=1), and
 *     STORAGE_FLASH_PAGE is not defined;
 *   - AP_BOOTLOADER_FLASHING_ENABLED=0 and flash crash dumps are disabled.
 *
 * hwdef.h defines HAL_NO_FLASH_SUPPORT, which makes the HAL select
 * Empty::Flash.  This translation unit exists only to keep the flash.h ABI
 * linkable for the few call sites that are not guarded by that macro.
 * Every operation reports "not available" - it never claims a successful
 * erase or program.  If a writable internal-flash region is reserved in a
 * future board revision, replace this file with a real CH32 flash driver
 * and drop HAL_NO_FLASH_SUPPORT.
 *
 * NOTE: bootloader builds share this board makefile but need a real QSPI2
 * NOR writer in order to flash an application image.  That driver is a
 * separate porting item; this stub is for the application build only.
 */

#include "hal.h"
#include "flash.h"

uint32_t stm32_flash_getpageaddr(uint32_t page)
{
    (void)page;
    return 0U;
}

uint32_t stm32_flash_getpagesize(uint32_t page)
{
    (void)page;
    return 0U;
}

uint32_t stm32_flash_getnumpages(void)
{
    return 0U;
}

bool stm32_flash_erasepage(uint32_t page)
{
    (void)page;
    return false;
}

bool stm32_flash_write(uint32_t addr, const void *buf, uint32_t count)
{
    (void)addr;
    (void)buf;
    (void)count;
    return false;
}

void stm32_flash_keep_unlocked(bool set)
{
    (void)set;
}

bool stm32_flash_ispageerased(uint32_t page)
{
    (void)page;
    return false;
}

void stm32_flash_protect_flash(bool bootloader, bool protect)
{
    (void)bootloader;
    (void)protect;
}

void stm32_flash_unprotect_flash(void)
{
}

void stm32_flash_set_NRST_MODE(uint8_t nrst_mode)
{
    (void)nrst_mode;
}

#ifndef HAL_BOOTLOADER_BUILD
bool stm32_flash_recent_erase(void)
{
    return false;
}
#endif
