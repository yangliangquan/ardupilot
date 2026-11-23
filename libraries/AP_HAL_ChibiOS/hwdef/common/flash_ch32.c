/*
 * CH32H417 internal CodeFlash backend for the flash.h API used by
 * AP_HAL_ChibiOS and the ChibiOS bootloader protocol.
 *
 * The CH32H417 flash controller is not STM32-FMC compatible.  Per the WCH
 * CH32H417 reference manual (FLASH chapter) and the vendor library
 * os/common/ext/WCH/CH32H417/ch32h417_flash.c:
 *
 *   - CodeFlash is dual-aliased: the XIP execution window is 0x00000000
 *     (zero wait) and the offset-mapped programming window is 0x08000000.
 *     Erase/program addresses (FLASH->ADDR and the halfword write window)
 *     must be given in the 0x08000000 window; the bootloader protocol's
 *     flash_base is already alias-based (Tools/AP_Bootloader/support.cpp).
 *   - FLASH_CFGR0 bit 28 (DBMODE, read-only) selects the geometry:
 *     1 = 960K / 8K erase sector / 64K block, 0 = 480K / 4K / 32K.
 *     The erase granularity is detected at run time, mirroring the
 *     ChibiOS EFL lld (os/hal/ports/WCH/CH32H417/hal_efl_lld.c).
 *   - Programming granularity is 2 bytes (standard mode) or 256 bytes
 *     (fast page mode).  The protocol always hands us 32-byte aligned
 *     word data, so we program halfwords through the official vendor
 *     library, exactly as the EFL lld does.
 *   - Erased CodeFlash reads back as 0xE339E339, not 0xFFFFFFFF.
 *     bl_protocol.cpp already relies on that pattern for its post-erase
 *     verification and for the reserved-lead-word check.
 *   - The V3F boot stub owns 0x0000-0x0FFF and the V5F bootloader owns
 *     0x1000-0xFFFF; nothing below the application window may ever be
 *     erased or programmed at run time, so every destructive operation
 *     is range-checked against the app window before touching the
 *     controller.
 *
 * Bootloader builds implement the full API on top of the vendor flash
 * library.  Application (flight) builds refuse every destructive
 * operation: this board has no flash region that flight firmware may
 * erase or program (parameters live in the SPI FRAM, STORAGE_FLASH_PAGE
 * is not defined, crash dumps and in-app reflashing are disabled), and
 * the geometry queries stay functional so the HAL Flash class links and
 * reports sane values.
 */

#include "hal.h"
#include "flash.h"
#include "ch32h417_flash.h"

/* Managed CodeFlash partition, in the programming alias window.  These
 * come from hwdef via hal_efl_lld.h; the fallbacks keep this file
 * buildable if the EFL partition is ever redefined. */
#ifndef CH32H417_EFL_BASE_ADDR
#define CH32H417_EFL_BASE_ADDR 0x08000000U
#endif
#ifndef CH32H417_EFL_SIZE
#define CH32H417_EFL_SIZE (960U * 1024U)
#endif

/* FLASH_CFGR0.DBMODE (bit 28): dual-bank mode uses 8K erase sectors. */
#define CH32_FLASH_DBMODE_MASK (1UL << 28)

/* Word read back from erased CodeFlash (WCH RM FLASH chapter note). */
#define CH32_FLASH_ERASED_WORD 0xe339e339U

static uint32_t ch32_flash_sector_size(void)
{
    return ((*(volatile uint32_t *)FLASH_CFGR0_BASE) & CH32_FLASH_DBMODE_MASK)
               ? 8192U
               : 4096U;
}

uint32_t stm32_flash_getnumpages(void)
{
    return CH32H417_EFL_SIZE / ch32_flash_sector_size();
}

uint32_t stm32_flash_getpagesize(uint32_t page)
{
    if (page >= stm32_flash_getnumpages()) {
        return 0U;
    }
    return ch32_flash_sector_size();
}

uint32_t stm32_flash_getpageaddr(uint32_t page)
{
    if (page >= stm32_flash_getnumpages()) {
        return 0U;
    }
    return CH32H417_EFL_BASE_ADDR + page * ch32_flash_sector_size();
}

bool stm32_flash_ispageerased(uint32_t page)
{
    const uint32_t addr = stm32_flash_getpageaddr(page);

    if (addr == 0U) {
        return false;
    }

    const volatile uint32_t *p = (const volatile uint32_t *)addr;
    const uint32_t n = ch32_flash_sector_size() / 4U;

    for (uint32_t i = 0; i < n; i++) {
        if (p[i] != CH32_FLASH_ERASED_WORD) {
            return false;
        }
    }
    return true;
}

#if defined(HAL_BOOTLOADER_BUILD)

/*
 * Lowest alias address the flight image may occupy.  This matches
 * flash_base in Tools/AP_Bootloader/support.cpp and keeps the V3F boot
 * stub and the V5F bootloader itself out of reach of the protocol.
 */
#define FLASH_APP_MIN_ADDR (CH32H417_EFL_BASE_ADDR + FLASH_LOAD_ADDRESS + \
        (FLASH_BOOTLOADER_LOAD_KB + APP_START_OFFSET_KB) * 1024U)
#define FLASH_APP_MAX_ADDR (CH32H417_EFL_BASE_ADDR + CH32H417_EFL_SIZE)

static bool flash_keep_unlocked;

static bool ch32_flash_addr_ok(uint32_t addr)
{
    return (addr >= FLASH_APP_MIN_ADDR) && (addr < FLASH_APP_MAX_ADDR);
}

/*
 * The QingKe V5F caches instruction fetches; lines covering the region we
 * just erased or programmed (and the speculative prefetches the erase
 * verification loop triggers) must not survive into the next phase or into
 * the application image.
 */
static void ch32_flash_cache_invalidate(void)
{
    ch32h417_icache_invalidate();
}

bool stm32_flash_erasepage(uint32_t page)
{
    const uint32_t addr = stm32_flash_getpageaddr(page);

    if (addr == 0U || !ch32_flash_addr_ok(addr)) {
        return false;
    }

    if (!flash_keep_unlocked) {
        FLASH_Unlock();
    }

    /* FLASH_ErasePage truncates the address to the DBMODE sector, runs a
     * PER and waits for BSY itself (synchronous, like the EFL lld). */
    const bool ok = (FLASH_ErasePage(addr) == FLASH_COMPLETE);

    if (!flash_keep_unlocked) {
        FLASH_Lock();
    }

    ch32_flash_cache_invalidate();
    return ok;
}

bool stm32_flash_write(uint32_t addr, const void *buf, uint32_t count)
{
    if (count == 0U) {
        return true;
    }
    if ((count & 1U) != 0U || (addr & 1U) != 0U) {
        /* CH32H417 only programs 2-byte (or 256-byte) units */
        return false;
    }
    if (!ch32_flash_addr_ok(addr) || (addr + count > FLASH_APP_MAX_ADDR)) {
        return false;
    }

    const uint16_t *p = (const uint16_t *)buf;

    if (!flash_keep_unlocked) {
        FLASH_Unlock();
    }

    bool ok = true;
    for (uint32_t i = 0; i < count; i += 2U) {
        if (FLASH_ProgramHalfWord(addr + i, p[i / 2U]) != FLASH_COMPLETE) {
            ok = false;
            break;
        }
    }

    if (!flash_keep_unlocked) {
        FLASH_Lock();
    }

    ch32_flash_cache_invalidate();
    return ok;
}

void stm32_flash_keep_unlocked(bool set)
{
    if (set) {
        /* The RM requires Enhanced Read Mode to be exited before any
         * erase/program operation, otherwise the operations fail.  The
         * port never enables it, but clear it defensively; this is a
         * no-op when ENHANCE_STATUS is already 0. */
        FLASH_Enhance_Mode(DISABLE);
        FLASH_Unlock();
    } else {
        FLASH_Lock();
    }
    flash_keep_unlocked = set;
}

#else /* !HAL_BOOTLOADER_BUILD */

/*
 * Flight firmware must never touch internal CodeFlash on this board:
 * the bootloader and the V3F stub live there and the image itself runs
 * XIP from it.  Refuse every destructive operation.
 */

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

bool stm32_flash_recent_erase(void)
{
    return false;
}

#endif /* HAL_BOOTLOADER_BUILD */

/*
 * CH32H417 has no STM32-style option bytes for NRST mapping or per-sector
 * write protection through OB->USER; the bootloader protocol only calls
 * these on STM32 targets, and the vendor library handles write protection
 * through FLASH_EnableWriteProtection which we deliberately never invoke
 * (the first 8K is already auto write-protected in DBMODE=1).
 */
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
