/*
 * arm_math.h - Compatibility header for RISC-V NMSIS DSP
 *
 * Maps ARM CMSIS DSP types and function names to RISC-V NMSIS equivalents.
 * This allows ARM CMSIS DSP code (like AP_GyroFFT) to compile on RISC-V.
 */

#ifndef _ARM_MATH_H
#define _ARM_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Compiler specific diagnostic adjustment */
#if defined ( __GNUC__ )
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wsign-conversion"
  #pragma GCC diagnostic ignored "-Wconversion"
  #pragma GCC diagnostic ignored "-Wunused-parameter"
  #pragma GCC diagnostic ignored "-Wcast-align"
  #pragma GCC diagnostic ignored "-Wcomment"
#endif

/* Include cmsis_compiler.h for compiler intrinsics */
#include "cmsis_compiler.h"

#include "string.h"
#include "math.h"
#include "float.h"

/* Include RISC-V NMSIS DSP headers */
#include "Include/riscv_math.h"
#include "Include/riscv_const_structs.h"
#include "Include/riscv_common_tables.h"

/* ==================== Type Mappings ==================== */

/* RISC-V NMSIS uses the same basic types as ARM CMSIS */
/* float32_t, q31_t, q15_t, q7_t are defined in riscv_math_types.h */

/* ARM CMSIS status type */
typedef riscv_status cmsis_arm_status;

/* ARM CMSIS instance structures -> RISC-V NMSIS equivalents */
typedef riscv_cfft_instance_f32 arm_cfft_instance_f32;
typedef riscv_cfft_instance_f32 arm_cfft_radix2_instance_f32;
typedef riscv_cfft_instance_f32 arm_cfft_radix4_instance_f32;

typedef riscv_rfft_fast_instance_f32 arm_rfft_fast_instance_f32;
typedef riscv_rfft_instance_f32 arm_rfft_instance_f32;

/* Const structs for CFFT */
#define arm_cfft_sR_f32_len16   riscv_cfft_sR_f32_len16
#define arm_cfft_sR_f32_len32   riscv_cfft_sR_f32_len32
#define arm_cfft_sR_f32_len64   riscv_cfft_sR_f32_len64
#define arm_cfft_sR_f32_len128  riscv_cfft_sR_f32_len128
#define arm_cfft_sR_f32_len256  riscv_cfft_sR_f32_len256
#define arm_cfft_sR_f32_len512  riscv_cfft_sR_f32_len512
#define arm_cfft_sR_f32_len1024 riscv_cfft_sR_f32_len1024
#define arm_cfft_sR_f32_len2048 riscv_cfft_sR_f32_len2048
#define arm_cfft_sR_f32_len4096 riscv_cfft_sR_f32_len4096

/* ==================== Function Mappings ==================== */

/* CFFT functions */
#define arm_cfft_f32         riscv_cfft_f32
#define arm_cfft_init_f32    riscv_cfft_init_f32
#define arm_cfft_init_32_f32 riscv_cfft_init_32_f32
#define arm_cfft_init_64_f32 riscv_cfft_init_64_f32
#define arm_cfft_init_128_f32 riscv_cfft_init_128_f32
#define arm_cfft_init_256_f32 riscv_cfft_init_256_f32
#define arm_cfft_init_512_f32 riscv_cfft_init_512_f32
#define arm_cfft_init_1024_f32 riscv_cfft_init_1024_f32
#define arm_cfft_init_2048_f32 riscv_cfft_init_2048_f32
#define arm_cfft_init_4096_f32 riscv_cfft_init_4096_f32

/* RFFT Fast functions */
#define arm_rfft_fast_init_f32    riscv_rfft_fast_init_f32
#define arm_rfft_fast_f32         riscv_rfft_fast_f32
#define arm_rfft_32_fast_init_f32 riscv_rfft_fast_init_32_f32
#define arm_rfft_64_fast_init_f32 riscv_rfft_fast_init_64_f32
#define arm_rfft_128_fast_init_f32 riscv_rfft_fast_init_128_f32
#define arm_rfft_256_fast_init_f32 riscv_rfft_fast_init_256_f32
#define arm_rfft_512_fast_init_f32 riscv_rfft_fast_init_512_f32
#define arm_rfft_1024_fast_init_f32 riscv_rfft_fast_init_1024_f32
#define arm_rfft_2048_fast_init_f32 riscv_rfft_fast_init_2048_f32
#define arm_rfft_4096_fast_init_f32 riscv_rfft_fast_init_4096_f32

/* Internal FFT functions used by DSP.cpp */
#define arm_cfft_radix8by2_f32     riscv_cfft_radix8by2_f32
#define arm_cfft_radix8by4_f32     riscv_cfft_radix8by4_f32
#define arm_radix8_butterfly_f32   riscv_radix8_butterfly_f32
#define arm_bitreversal_32         riscv_bitreversal_32

/* stage_rfft_f32 is defined in riscv_rfft_fast_f32.c with the same name */

/* Complex math functions */
#define arm_cmplx_mag_squared_f32  riscv_cmplx_mag_squared_f32
#define arm_cmplx_mag_f32          riscv_cmplx_mag_f32
#define arm_cmplx_conj_f32         riscv_cmplx_conj_f32
#define arm_cmplx_mult_real_f32    riscv_cmplx_mult_real_f32

/* Basic math functions */
#define arm_mult_f32               riscv_mult_f32
#define arm_add_f32                riscv_add_f32
#define arm_sub_f32                riscv_sub_f32
#define arm_scale_f32              riscv_scale_f32
#define arm_negate_f32             riscv_negate_f32
#define arm_dot_prod_f32           riscv_dot_prod_f32

/* Statistics functions */
#define arm_mean_f32               riscv_mean_f32
#define arm_max_f32                riscv_max_f32
#define arm_min_f32                riscv_min_f32
#define arm_power_f32              riscv_power_f32
#define arm_var_f32                riscv_var_f32

/* Fast math functions */
#define arm_sqrt_f32               riscv_sqrt_f32
#define arm_cos_f32                riscv_cos_f32
#define arm_sin_f32                riscv_sin_f32

/* Twiddle tables - use RISC-V NMSIS tables */
#define twiddleCoef_f32            riscv_twiddleCoef_f32

/* FAST_MATH_TABLE_SIZE */
#ifndef FAST_MATH_TABLE_SIZE
  #define FAST_MATH_TABLE_SIZE 2048
#endif

/* Status codes */
#ifndef ARM_MATH_SUCCESS
  #define ARM_MATH_SUCCESS            0
#endif

#ifndef ARM_MATH_ARGUMENT_ERROR
  #define ARM_MATH_ARGUMENT_ERROR    -1
#endif

#ifndef ARM_MATH_LENGTH_ERROR
  #define ARM_MATH_LENGTH_ERROR      -2
#endif

#ifndef ARM_MATH_SIZE_MISMATCH
  #define ARM_MATH_SIZE_MISMATCH     -3
#endif

/* ARM math macros */
#ifndef PI
  #define PI 3.14159265358979f
#endif

#ifndef ONEBYPI
  #define ONEBYPI (1.0f / PI)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ARM_MATH_H */
