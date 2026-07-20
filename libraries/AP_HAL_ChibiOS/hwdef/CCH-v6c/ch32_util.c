/*
 * CH32H417 utility functions - stubs for functions declared in ch32_util.h
 * that are not yet implemented for the CH32H417 platform.
 */

#include "ch.h"
#include "ch32_util.h"
#include <string.h>

/*
  peripheral power enable - called late to prevent problems with CTS
  causing SiK radios to stay in the bootloader.
*/
void peripheral_power_enable(void)
{
    // wait for radio to be powered down
    chThdSleep(chTimeMS2I(100));

#ifdef HAL_GPIO_PIN_nVDD_5V_PERIPH_EN
    palClearLine(HAL_GPIO_PIN_nVDD_5V_PERIPH_EN);
#endif
#ifdef HAL_GPIO_PIN_nVDD_5V_HIPOWER_EN
    palClearLine(HAL_GPIO_PIN_nVDD_5V_HIPOWER_EN);
#endif
#ifdef HAL_GPIO_PIN_VDD_3V3_SENSORS_EN
    palSetLine(HAL_GPIO_PIN_VDD_3V3_SENSORS_EN);
#endif
#ifdef HAL_GPIO_PIN_VDD_3V3_SENSORS1_EN
    palSetLine(HAL_GPIO_PIN_VDD_3V3_SENSORS1_EN);
#endif
}

/*
  hook called by ChibiOS on system halt
*/
void system_halt_hook(void)
{
}

/*
  check how much stack is free given a stack base.
  Assumes the fill byte is 0x55
*/
#if CH_DBG_ENABLE_STACK_CHECK == TRUE
uint32_t stack_free(void *stack_base)
{
    const uint32_t *p = (uint32_t *)stack_base;
    const uint32_t canary_word = 0x55555555;
    while (*p == canary_word) {
        p++;
    }
    return ((uint32_t)p) - (uint32_t)stack_base;
}
#endif

/*
  stack overflow hook
*/
void stack_overflow(thread_t *tp)
{
    (void)tp;
}

/*
  check fast reboot
*/
enum rtc_boot_magic check_fast_reboot(void)
{
    return RTC_BOOT_OFF;
}

/*
  set fast reboot
*/
void set_fast_reboot(enum rtc_boot_magic v)
{
    (void)v;
}

/*
  UTC system clock stubs
*/
static uint64_t utc_usec;

void stm32_set_utc_usec(uint64_t time_utc_usec)
{
    utc_usec = time_utc_usec;
}

uint64_t stm32_get_utc_usec(void)
{
    return utc_usec;
}

/*
  __wrap_setjmp - the linker --wrap=setjmp flag redirects setjmp calls
  to __wrap_setjmp. Call the real setjmp via __real_setjmp.
*/
#include <setjmp.h>

extern int __real_setjmp(jmp_buf env);

int __wrap_setjmp(jmp_buf env)
{
    return __real_setjmp(env);
}

/*
  Cache operations - CH32H417 has no cache, these are no-ops.
*/
void stm32_cacheBufferFlush(const void *p, size_t size)
{
    (void)p;
    (void)size;
}

void stm32_cacheBufferInvalidate(const void *p, size_t size)
{
    (void)p;
    (void)size;
}

void palLineSetPushPull(ioline_t line, enum PalPushPull pp){
    (void)line;
    (void)pp;
}

void stm32_clock_init(){
    
}

/*
  RTC backup register stubs - CH32H417 does not have STM32-style RTC backup registers.
*/
void set_rtc_backup(uint8_t idx, const uint32_t *v, uint8_t n)
{
    (void)idx;
    (void)v;
    (void)n;
}

void get_rtc_backup(uint8_t idx, uint32_t *v, uint8_t n)
{
    (void)idx;
    (void)v;
    (void)n;
}