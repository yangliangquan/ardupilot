/*
 * CH32H417 (QingKe V5F) board initialisation for AP_HAL_ChibiOS.
 *
 * This is the RISC-V replacement for hwdef/common/board.c, which is written
 * against the STM32 GPIO register layout (MODER/OTYPER/OSPEEDR/PUPDR/ODR/
 * AFRx).  The CH32H417 has GPIOA..GPIOF with CFGLR/CFGHR and a separate
 * AFIO block holding GPIOx_AFLR/AFHR.
 *
 * The hwdef generator emits VAL_GPIOx_AFRL/AFRH using
 *   PIN_AFIO_AF(n, v) == (v) << ((n % 8) * 4)
 * which is exactly the CH32H417 AFIO nibble layout, so the AF mux tables
 * are written to AFIO verbatim.  The remaining STM32-style bitfields are
 * translated into iomode_t values and applied through the adapt-417 PAL
 * driver, which owns the CFGLR/CFGHR and AFIO programming details.
 */

#include "hal.h"
#include "board.h"
#include "hwdef.h"
#include "stm32_util.h"
#include "usbcfg.h"
#include "watchdog.h"

#if defined(CH32H417)

/*
 * Whether the clock tree may be re-programmed from __early_init().
 *
 * The flight image executes in place from the QSPI2 XIP window, so its
 * clock tree must be left exactly as the V3F stub / bootloader configured
 * it (HAL_XIP_ENABLED is TRUE for that image).  The bootloader itself runs
 * from internal CodeFlash, where ch32h417_clock_init() is both safe and
 * necessary: it builds HSE->PLL->SYSCLK/HCLK before hal_lld_init() checks
 * the frequencies.  HAL_FORCE_CLOCK_INIT is the escape hatch for an XIP
 * image whose V3F stub does not set up the tree.
 *
 * Note hwdef-bl.dat does not emit HAL_XIP_ENABLED at all, hence the
 * defined() first (short-circuited) term - a bare #if !HAL_XIP_ENABLED
 * would trip -Werror=undef in the bootloader build.
 */
#if defined(HAL_XIP_ENABLED) && HAL_XIP_ENABLED && !defined(HAL_FORCE_CLOCK_INIT)
#define CH32_SKIP_CLOCK_INIT 1
#else
#define CH32_SKIP_CLOCK_INIT 0
#endif

typedef struct {
    GPIO_TypeDef      *gpio;
    volatile uint32_t *aflr;
    volatile uint32_t *afhr;
    uint32_t           moder;
    uint32_t           otyper;
    uint32_t           pupdr;
    uint32_t           odr;
    uint32_t           afrl;
    uint32_t           afrh;
} ch32_gpio_port_t;

#define CH32_GPIO_PORT(x) {                                                  \
    GPIO##x,                                                                 \
    &AFIO->GPIO##x##_AFLR, &AFIO->GPIO##x##_AFHR,                            \
    VAL_GPIO##x##_MODER, VAL_GPIO##x##_OTYPER, VAL_GPIO##x##_PUPDR,          \
    VAL_GPIO##x##_ODR, VAL_GPIO##x##_AFRL, VAL_GPIO##x##_AFRH                 \
}

static const ch32_gpio_port_t ch32_gpio_ports[] = {
    CH32_GPIO_PORT(A),
    CH32_GPIO_PORT(B),
    CH32_GPIO_PORT(C),
    CH32_GPIO_PORT(D),
    CH32_GPIO_PORT(E),
    CH32_GPIO_PORT(F),
};

#define CH32_GPIO_PORT_COUNT (sizeof(ch32_gpio_ports) / sizeof(ch32_gpio_ports[0]))

static void ch32_gpio_clock_enable(void)
{
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO  |
                          RCC_HB2Periph_GPIOA |
                          RCC_HB2Periph_GPIOB |
                          RCC_HB2Periph_GPIOC |
                          RCC_HB2Periph_GPIOD |
                          RCC_HB2Periph_GPIOE |
                          RCC_HB2Periph_GPIOF,
                          ENABLE);
}

/*
 * In an XIP image the bootloader has already muxed the QSPI2 pads and the
 * application is executing out of that window.  palSetPadMode() is not a
 * no-op even when the new value equals the old one: set_pad_mode() runs
 * GPIO_PinAFConfig() followed by GPIO_Init(), which briefly drops the
 * alternate function and breaks instruction fetch.  Those pads must be
 * left completely untouched here.
 *
 * The line macros are emitted by the hwdef generator from the pin labels
 * in hwdef.dat, and only exist in the application (XIP) build; the
 * bootloader build is guarded out by HAL_XIP_ENABLED.
 */
#if defined(HAL_XIP_ENABLED) && HAL_XIP_ENABLED && defined(HAL_GPIO_PIN_QSPI2_SCK)
static const ioline_t ch32_xip_lines[] = {
    HAL_GPIO_PIN_QSPI2_SCK,
    HAL_GPIO_PIN_QSPI2_SCSN,
    HAL_GPIO_PIN_QSPI2_SCSXN,
    HAL_GPIO_PIN_QSPI2_SIO0,
    HAL_GPIO_PIN_QSPI2_SIO1,
    HAL_GPIO_PIN_QSPI2_SIO2,
    HAL_GPIO_PIN_QSPI2_SIO3,
    HAL_GPIO_PIN_QSPI2_SIOX0,
    HAL_GPIO_PIN_QSPI2_SIOX1,
    HAL_GPIO_PIN_QSPI2_SIOX2,
    HAL_GPIO_PIN_QSPI2_SIOX3,
};

#define CH32_XIP_LINES_COUNT (sizeof(ch32_xip_lines) / sizeof(ch32_xip_lines[0]))

static unsigned ch32_is_xip_pad(GPIO_TypeDef *gpio, unsigned pin)
{
    for (unsigned i = 0U; i < CH32_XIP_LINES_COUNT; i++) {
        if ((PAL_PORT(ch32_xip_lines[i]) == gpio) &&
            (PAL_PAD(ch32_xip_lines[i]) == pin)) {
            return 1U;
        }
    }
    return 0U;
}
#else
#define ch32_is_xip_pad(gpio, pin) 0U
#endif

/* Apply the hwdef pin table, except for the QSPI2 pads in an XIP image.
   The bootloader build has no XIP lines and applies the full table. */
static void ch32_gpio_init(void)
{
    unsigned port_i;

    ch32_gpio_clock_enable();

    for (port_i = 0U; port_i < CH32_GPIO_PORT_COUNT; port_i++) {
        const ch32_gpio_port_t *p = &ch32_gpio_ports[port_i];
        unsigned pin;

        /* Identical nibble layout: AFRL/AFRH -> GPIOx_AFLR/AFHR.
           The PAL driver rewrites the nibble of each alternate-function
           pin through GPIO_PinAFConfig(), but that helper is a read-
           modify-write on a single nibble, so the rest of the table
           survives.  The values are identical by construction. */
        uint32_t aflr = p->afrl;
        uint32_t afhr = p->afrh;
#if defined(HAL_XIP_ENABLED) && HAL_XIP_ENABLED && defined(HAL_GPIO_PIN_QSPI2_SCK)
        /* Keep the bootloader's mux nibble on the QSPI2 pads instead of
           overwriting the whole register while the CPU fetches through
           them.  The generated table should agree, but the XIP handover
           is what actually matters here. */
        for (pin = 0U; pin < 16U; pin++) {
            if (ch32_is_xip_pad(p->gpio, pin)) {
                if (pin < 8U) {
                    const uint32_t mask = 0x0FU << (pin * 4U);
                    aflr = (aflr & ~mask) | (*p->aflr & mask);
                } else {
                    const uint32_t mask = 0x0FU << ((pin - 8U) * 4U);
                    afhr = (afhr & ~mask) | (*p->afhr & mask);
                }
            }
        }
#endif
        *p->aflr = aflr;
        *p->afhr = afhr;

        for (pin = 0U; pin < 16U; pin++) {
            /* palSetPadMode() briefly drops the alternate function on
               these pads (GPIO_PinAFConfig + GPIO_Init), which breaks
               instruction fetch from the QSPI2 XIP window. */
            if (ch32_is_xip_pad(p->gpio, pin)) {
                continue;
            }

            const uint32_t m  = (p->moder  >> (pin * 2U)) & 3U;
            const uint32_t ot = (p->otyper >> pin) & 1U;
            const uint32_t pu = (p->pupdr  >> (pin * 2U)) & 3U;
            const uint32_t af = (pin < 8U)
                              ? ((p->afrl >> ((pin % 8U) * 4U)) & 0x0FU)
                              : ((p->afrh >> ((pin % 8U) * 4U)) & 0x0FU);
            iomode_t mode;

            switch (m) {
            case 0U: /* input */
                if (pu == 1U) {
                    mode = PAL_MODE_INPUT_PULLUP;
                } else if (pu == 2U) {
                    mode = PAL_MODE_INPUT_PULLDOWN;
                } else {
                    mode = PAL_MODE_INPUT;
                }
                break;
            case 1U: /* gpio output */
                mode = ot ? PAL_MODE_OUTPUT_OPENDRAIN : PAL_MODE_OUTPUT_PUSHPULL;
                break;
            case 2U: /* alternate function */
                mode = ot ? PAL_MODE_ALTERNATE_OPENDRAIN(af)
                          : PAL_MODE_ALTERNATE(af);
                break;
            default: /* analog */
                mode = PAL_MODE_INPUT_ANALOG;
                break;
            }

            palSetPadMode(p->gpio, pin, mode);

            /* GPIO_Init() drives BSHR/BCR from the mode for IPU/IPD pins,
               and ODR is meaningless for AF and analog pins.  Writing ODR
               for every pad would silently turn every pull-up input into a
               pull-down/floating one, so only drive plain GPIO outputs. */
            if (m == 1U) {
                if ((p->odr >> pin) & 1U) {
                    palSetPad(p->gpio, pin);
                } else {
                    palClearPad(p->gpio, pin);
                }
            }
        }
    }
}

/* Called from crt0 before .data/.bss are set up (see crt0.S:223).  Only
   register/rodata operations are allowed here.  The full GPIO table is
   applied from boardInit(), after static data is valid; ch32_gpio_ports
   lives in flash either way. */
void __early_init(void)
{
    ch32_gpio_clock_enable();
    ch32h417_clock_init();
}

void boardInit(void)
{
    ch32_gpio_init();
    HAL_BOARD_INIT_HOOK_CALL;
}

/*
 * Late initialization - the CH32 counterpart of board.c::__late_init().
 *
 * crt0.S calls this hook after DATA/BSS are in place and before the C++
 * constructors (crt0.S:280).  crt1.c supplies a WEAK *empty* default, so
 * leaving it undefined links perfectly cleanly but silently skips the
 * entire HAL and RTOS startup.  In particular boardInit() is reached from
 * halInit(), so without this hook the whole hwdef pin table is never
 * applied either.  This is a boot-time-only failure: no compiler or
 * linker diagnostic can catch it.
 */
void __late_init(void)
{
    /* hal_lld_init(), palInit() and every enabled xxxInit() (sd, spi, i2c,
       pwm, adc, usb, sdu, wspi, trng, wdg, st), then boardInit(). */
    halInit();

    /* Kernel, idle thread, current-thread context and the ST timebase. */
    chSysInit();

    /* No STM32 RNG peripheral here: HAL_USE_TRNG provides TRNGD1 and
       halInit() already called trngInit(), so the upstream RNG->CR block
       has no counterpart. */

    /* Latch the reset flags before anything clears them.  On this port
       WDG_RESET_STATUS is RCC->RSTSCKR, so this is real functionality,
       not a no-op. */
    stm32_watchdog_save_reason();
#ifndef HAL_BOOTLOADER_BUILD
    stm32_watchdog_clear_reason();
#endif

#if CH_CFG_USE_HEAP == TRUE
    /* Sets up the extra memory heaps and, importantly, the 6 KiB
       DMA-reserve heap used by malloc_dma(). */
    malloc_init();
#endif

#ifdef HAL_USB_PRODUCT_ID
    /* Fill in the manufacturer/product/serial USB descriptor strings
       (%BOARD%/%SERIAL% substitution). */
    setup_usb_strings();
#endif

    /* No stm32_flash_set_NRST_MODE() counterpart: this port has no
       option-byte flash interface (HAL_NO_FLASH_SUPPORT). */
}

#endif /* CH32H417 */
