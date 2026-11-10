#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <setjmp.h>

#include "AP_HAL_Boards.h"

// setjmp with platform-specific implementations.

// older ARM newlib (in particular that shipped with ARM GCC 10.3.1) does not
// save the callee-saved floating point registers. if we detect a situation
// where we may be using such a newlib and such registers are available, use
// our own implementation which actually saves them. these cannot be wrappers
// as we cannot safely use the stack and have to expand the jmp_buf.
#if (CONFIG_HAL_BOARD == HAL_BOARD_CHIBIOS) && (defined(_NEWLIB_VERSION) && (defined(__ARM_FP) || defined(__ARM_FEATURE_MVE)))
#define AP_SETJMP_CORTEX_ENABLE 1
#else
#define AP_SETJMP_CORTEX_ENABLE 0
#endif

// ArduPilot links with -Wl,--wrap,setjmp and treats a lingering "setjmp"
// symbol as a hard error (see Tools/ardupilotwaf/chibios.py SYMBOLS_BLACKLIST),
// so on the RISC-V port we also cannot fall back to the C library. The
// QingKe V5F part is built for rv32imafc, i.e. with the single precision F
// extension, whose callee-saved registers fs0..fs11 must be preserved across
// a longjmp just like the integer s0..s11.
#if (CONFIG_HAL_BOARD == HAL_BOARD_CHIBIOS) && defined(__riscv)
#define AP_SETJMP_RISCV_ENABLE 1
#else
#define AP_SETJMP_RISCV_ENABLE 0
#endif

#if AP_SETJMP_CORTEX_ENABLE

#include <inttypes.h>

// 20 8-byte words is defined by
// https://github.com/ARM-software/abi-aa/blob/c51addc3dc03e73a016a1e4edf25440bcac76431/clibabi32/clibabi32.rst#setjmp-h
// but we only need 10 regular registers and 16 FP registers meaning 13 8 byte
// words so we can save a bit of memory. switching back to the official setjmp
// implementation will re-consume it though. Lua keeps these on the C stack.

struct _ap_jmp_buf {
    int64_t data[13];
};

// hack to allow ap_jmp_buf to decay to a pointer
typedef struct _ap_jmp_buf ap_jmp_buf[1];

int __attribute__((naked, noinline)) ap_setjmp(ap_jmp_buf env);
void __attribute__((noreturn, naked, noinline)) ap_longjmp(ap_jmp_buf env, int val);

#elif AP_SETJMP_RISCV_ENABLE

#include <inttypes.h>

/*
  RISC-V (rv32imafc) buffer layout, 32-bit words:

     0 : ra
     4 : sp
     8 : s0/fp
    12 : s1
    16 : s2   ...  52 : s11
    56 : fs0  ... 100 : fs11

  13 int64_t keeps the buffer 8-byte aligned and matches the 104 bytes used
  (the same size the Cortex-M version needs).
 */

struct _ap_jmp_buf {
    int64_t data[13];
};

// hack to allow ap_jmp_buf to decay to a pointer
typedef struct _ap_jmp_buf ap_jmp_buf[1];

int ap_setjmp(ap_jmp_buf env);
void __attribute__((noreturn)) ap_longjmp(ap_jmp_buf env, int val);

#else

#define ap_setjmp(env) (setjmp(env))
#define ap_longjmp(env, val) (longjmp(env, val))

typedef jmp_buf ap_jmp_buf;

#endif

#ifdef __cplusplus
}
#endif
