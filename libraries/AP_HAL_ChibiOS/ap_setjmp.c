#include <AP_HAL/ap_setjmp.h>

#if AP_SETJMP_CORTEX_ENABLE

int __attribute__((naked, noinline)) ap_setjmp(ap_jmp_buf env) {
    __asm__(
        "mov ip, sp\n\t" // ip must be free on a function call; can't store sp
        "stmia r0!, {r4-r10, fp, ip, lr}\n\t" // store 10 regular registers
        "vstm r0, {d8-d15}\n\t" // store 16 FP registers. use 8 byte store for
                                // speed as our buffer is 8 byte aligned
        "movs r0, #0\n\t" // return 0
        "bx lr\n\t" // do return
    );
}

void __attribute__((noreturn, naked, noinline)) ap_longjmp(ap_jmp_buf env, int val) {
    __asm__(
        "ldmia r0!, {r4-r10, fp, ip, lr}\n\t" // load 10 regular registers
        "mov sp, ip\n\t" // restore stack pointer from scratch register
        "vldm r0, {d8-d15}\n\t" // load 16 FP registers. use 8 byte load for
                                // speed as our buffer is 8 byte aligned
        "movs r0, r1\n\t" // prepare return of value
        "it eq\n\t" // but if value is zero
        "moveq r0, #1\n\t" // return one
        "bx lr\n\t" // do return
    );
}

#elif AP_SETJMP_RISCV_ENABLE

/*
  RISC-V (rv32imafc) setjmp/longjmp for the Lua scripting engine.

  Written as top level assembly rather than naked functions: the sequence has
  to be exactly the instructions below (no stack frame, no register
  allocation decisions by the compiler), and top level asm is the most
  predictable way to get that.

  Only callee-saved state is saved - ra, sp, s0..s11 and, because the port is
  built with the single precision F extension, fs0..fs11. Everything else is
  caller-saved by the RISC-V calling convention, so a caller of ap_setjmp()
  may not assume it survives the call anyway.

  Buffer layout (byte offsets, all 4 byte accesses):
      0  ra      4  sp      8  s0     12  s1
     16  s2     20  s3     24  s4     28  s5
     32  s6     36  s7     40  s8     44  s9
     48  s10    52  s11
     56  fs0    60  fs1    64  fs2    68  fs3
     72  fs4    76  fs5    80  fs6    84  fs7
     88  fs8    92  fs9    96  fs10  100  fs11

  The buffer is 104 bytes (13 int64_t, 8 byte aligned), the same size the
  Cortex-M version needs.
 */

#if !defined(__riscv_flen) || (__riscv_flen != 32)
#error "ap_setjmp for RISC-V assumes the single precision F extension (rv32imafc)"
#endif

__asm__(
    "    .text\n"
    "    .globl  ap_setjmp\n"
    "    .type   ap_setjmp, @function\n"
    "ap_setjmp:\n"
    "    sw      ra,    0(a0)\n"
    "    sw      sp,    4(a0)\n"
    "    sw      s0,    8(a0)\n"
    "    sw      s1,   12(a0)\n"
    "    sw      s2,   16(a0)\n"
    "    sw      s3,   20(a0)\n"
    "    sw      s4,   24(a0)\n"
    "    sw      s5,   28(a0)\n"
    "    sw      s6,   32(a0)\n"
    "    sw      s7,   36(a0)\n"
    "    sw      s8,   40(a0)\n"
    "    sw      s9,   44(a0)\n"
    "    sw      s10,  48(a0)\n"
    "    sw      s11,  52(a0)\n"
    "    fsw     fs0,  56(a0)\n"
    "    fsw     fs1,  60(a0)\n"
    "    fsw     fs2,  64(a0)\n"
    "    fsw     fs3,  68(a0)\n"
    "    fsw     fs4,  72(a0)\n"
    "    fsw     fs5,  76(a0)\n"
    "    fsw     fs6,  80(a0)\n"
    "    fsw     fs7,  84(a0)\n"
    "    fsw     fs8,  88(a0)\n"
    "    fsw     fs9,  92(a0)\n"
    "    fsw     fs10, 96(a0)\n"
    "    fsw     fs11, 100(a0)\n"
    "    li      a0, 0\n"          // ap_setjmp() returns 0
    "    ret\n"
    "    .size   ap_setjmp, .-ap_setjmp\n"
);

__asm__(
    "    .text\n"
    "    .globl  ap_longjmp\n"
    "    .type   ap_longjmp, @function\n"
    "ap_longjmp:\n"
    "    lw      ra,    0(a0)\n"
    "    lw      sp,    4(a0)\n"
    "    lw      s0,    8(a0)\n"
    "    lw      s1,   12(a0)\n"
    "    lw      s2,   16(a0)\n"
    "    lw      s3,   20(a0)\n"
    "    lw      s4,   24(a0)\n"
    "    lw      s5,   28(a0)\n"
    "    lw      s6,   32(a0)\n"
    "    lw      s7,   36(a0)\n"
    "    lw      s8,   40(a0)\n"
    "    lw      s9,   44(a0)\n"
    "    lw      s10,  48(a0)\n"
    "    lw      s11,  52(a0)\n"
    "    flw     fs0,  56(a0)\n"
    "    flw     fs1,  60(a0)\n"
    "    flw     fs2,  64(a0)\n"
    "    flw     fs3,  68(a0)\n"
    "    flw     fs4,  72(a0)\n"
    "    flw     fs5,  76(a0)\n"
    "    flw     fs6,  80(a0)\n"
    "    flw     fs7,  84(a0)\n"
    "    flw     fs8,  88(a0)\n"
    "    flw     fs9,  92(a0)\n"
    "    flw     fs10, 96(a0)\n"
    "    flw     fs11, 100(a0)\n"
    "    mv      a0, a1\n"          // return the value passed to ap_longjmp()
    "    bnez    a0, .Lap_longjmp_nonzero\n"
    "    li      a0, 1\n"           // ... but never 0, per setjmp semantics
    ".Lap_longjmp_nonzero:\n"
    "    ret\n"
    "    .size   ap_longjmp, .-ap_longjmp\n"
);

#endif
