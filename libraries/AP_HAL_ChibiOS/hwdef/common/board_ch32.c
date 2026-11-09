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

#if defined(CH32H417)

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

/* Apply the whole hwdef pin table.  Keeping the QSPI2 pins configured here
   matters: the flight image executes from the QSPI2 XIP window, so the
   bootloader's pad setup must be reproduced rather than dropped. */
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
        *p->aflr = p->afrl;
        *p->afhr = p->afrh;

        for (pin = 0U; pin < 16U; pin++) {
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

/* Called from crt0 before .data/.bss are set up.  Only enable the clocks
   here; the full table is applied from boardInit(), after static data is
   valid.  ch32_gpio_ports lives in flash either way. */
void __early_init(void)
{
    ch32_gpio_clock_enable();
}

void boardInit(void)
{
    ch32_gpio_init();
    HAL_BOARD_INIT_HOOK_CALL;
}

#endif /* CH32H417 */
