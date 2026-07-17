/*
 * CH32H417 compatibility header - replaces stm32_util.h for RISC-V builds.
 * Provides both STM32-compatible macro aliases and CH32-adapted
 * function declarations.
 */
#pragma once

#include "hal.h"
#include "ch32_compat.h"
#include <stdarg.h>


#ifdef __cplusplus
extern "C" {
#endif

// timer functions - implemented in ch32_util.c
void stm32_timer_set_input_filter(stm32_tim_t *tim, uint8_t channel, uint8_t filter_mode);
void stm32_timer_set_channel_input(stm32_tim_t *tim, uint8_t channel, uint8_t input_source);

// memory functions - implemented in malloc.c
size_t mem_available(void);
void *malloc_dma(size_t size);
void *malloc_axi_sram(size_t size);
void *malloc_fastmem(size_t size);
void *malloc_eth_safe(size_t size);
thread_t *thread_create_alloc(size_t size, const char *name, tprio_t prio, tfunc_t pf, void *arg);
bool mem_is_dma_safe(const void *addr, uint32_t size, bool filesystem_op);

struct memory_region {
    void *address;
    uint32_t size;
    uint32_t flags;
};
#if CH_CFG_USE_HEAP == TRUE
uint8_t malloc_get_heaps(memory_heap_t **_heaps, const struct memory_region **regions);
#endif

void memory_flush_all(void);

// UTC system clock handling
void stm32_set_utc_usec(uint64_t time_utc_usec);
uint64_t stm32_get_utc_usec(void);
uint32_t get_fattime(void);

bool check_limit_flash_1M(void);

// OTP area
#if !defined(OTP_BASE) && !defined(OTP_SIZE)
#define OTP_BASE 0
#define OTP_SIZE 0
#endif

enum rtc_boot_magic {
    RTC_BOOT_OFF  = 0,
    RTC_BOOT_HOLD = 0xb0070001,
    RTC_BOOT_FAST = 0xb0070002,
    RTC_BOOT_CANBL = 0xb0080000,
    RTC_BOOT_FWOK = 0xb0093a26
};

enum rtc_boot_magic check_fast_reboot(void);
void set_fast_reboot(enum rtc_boot_magic v);
void peripheral_power_enable(void);
void malloc_init(void);

// flash functions - implemented in flash.c
uint32_t stm32_flash_getpageaddr(uint32_t page);
uint32_t stm32_flash_getpagesize(uint32_t page);
uint32_t stm32_flash_getnumpages(void);
bool stm32_flash_erasepage(uint32_t page);
bool stm32_flash_write(uint32_t addr, const void *buf, uint32_t count);
void stm32_flash_keep_unlocked(bool set);
bool stm32_flash_ispageerased(uint32_t page);
void stm32_flash_protect_flash(bool bootloader, bool protect);
void stm32_flash_unprotect_flash(void);
void stm32_flash_set_NRST_MODE(uint8_t nrst_mode);
void stm32_flash_corrupt(uint32_t addr, bool double_bit);
bool stm32_flash_recent_erase(void);

// crash dump
uint32_t stm32_crash_dump_size(void);
uint32_t stm32_crash_dump_addr(void);
uint32_t stm32_crash_dump_max_size(void);

typedef enum {
    Reset = 1,
    NMI = 2,
    HardFault = 3,
    MemManage = 4,
    BusFault = 5,
    UsageFault = 6,
} FaultType;

#if AP_WATCHDOG_SAVE_FAULT_ENABLED
void save_fault_watchdog(uint16_t line, FaultType fault_type, uint32_t fault_addr, uint32_t lr);
#endif

// RNG
#if HAL_USE_HW_RNG && defined(RNG)
bool stm32_rand_generate_blocking(unsigned char* output, unsigned int sz, uint32_t timeout_us);
unsigned int stm32_rand_generate_nonblocking(unsigned char* output, unsigned int sz);
#endif
extern uint32_t chibios_rand_generate(void);

extern int (*vprintf_console_hook)(const char *fmt, va_list arg);

#if CH_DBG_ENABLE_STACK_CHECK == TRUE
void show_stack_usage(void);
#endif

// Pin mode reading
iomode_t palReadLineMode(ioline_t line);
enum PalPushPull {
    PAL_PUSHPULL_NOPULL=0,
    PAL_PUSHPULL_PULLUP=1,
    PAL_PUSHPULL_PULLDOWN=2
};
void palLineSetPushPull(ioline_t line, enum PalPushPull pp);

// RTC backup
void set_rtc_backup(uint8_t idx, const uint32_t *v, uint8_t n);
void get_rtc_backup(uint8_t idx, uint32_t *v, uint8_t n);

void stm32_cacheBufferInvalidate(const void *p, size_t size);
void stm32_cacheBufferFlush(const void *p, size_t size);

#ifdef HAL_GPIO_PIN_FAULT
void fault_printf(const char *fmt, ...);
#endif

void system_halt_hook(void);
void stack_overflow(thread_t *tp);
uint32_t stack_free(void *stack_base);
bool is_address_in_memory(void *addr);
void* get_addr_mem_region_start_addr(void *addr);
void* get_addr_mem_region_end_addr(void *addr);

// stack externs for stack checking
extern char __main_stack_base__[];
extern char __main_stack_end__[];
extern char __main_thread_stack_base__[];
extern char __main_thread_stack_end__[];

void stm32_disable_cm4_core(void);

/*
 * I2C software stop - sets STOP bit in CTLR1
 * Takes I2CDriver* (ChibiOS driver object), accesses hardware via driver
 */
static inline void i2cSoftStop(I2CDriver *i2cp) {
    i2cp->i2c->CTLR1 |= I2C_CTLR1_STOP;
}

#ifdef __cplusplus
}
#endif
