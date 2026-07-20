/*
 * bouncebuffer stub for CH32H417
 * CH32H417 has no cache coherency issues - all memory is DMA-safe.
 * All functions are no-ops that pass through the original buffer pointers.
 */

#include <stdint.h>
#include <stdbool.h>
#include "../../common/bouncebuffer.h"
#include <stdlib.h>

void bouncebuffer_init(struct bouncebuffer_t **bouncebuffer, uint32_t prealloc_bytes, bool axi_sram)
{
    (void)prealloc_bytes;
    (void)axi_sram;
    *bouncebuffer = calloc(1, sizeof(struct bouncebuffer_t));
}

bool bouncebuffer_setup_read(struct bouncebuffer_t *bouncebuffer, uint8_t **buf, uint32_t size)
{
    (void)bouncebuffer;
    (void)buf;
    (void)size;
    return true;
}

void bouncebuffer_finish_read(struct bouncebuffer_t *bouncebuffer, const uint8_t *buf, uint32_t size)
{
    (void)bouncebuffer;
    (void)buf;
    (void)size;
}

bool bouncebuffer_setup_write(struct bouncebuffer_t *bouncebuffer, const uint8_t **buf, uint32_t size)
{
    (void)bouncebuffer;
    (void)buf;
    (void)size;
    return true;
}

void bouncebuffer_finish_write(struct bouncebuffer_t *bouncebuffer, const uint8_t *buf)
{
    (void)bouncebuffer;
    (void)buf;
}

void bouncebuffer_abort(struct bouncebuffer_t *bouncebuffer)
{
    (void)bouncebuffer;
}
