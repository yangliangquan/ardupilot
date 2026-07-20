/*
 * cmsis_compiler.h - Compatibility header for RISC-V NMSIS DSP
 *
 * Maps ARM CMSIS compiler intrinsics to RISC-V NMSIS equivalents.
 * This allows ARM CMSIS DSP code to compile on RISC-V platforms.
 */

#ifndef __CMSIS_COMPILER_H
#define __CMSIS_COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Use NMSIS GCC compiler header for RISC-V */
#include "Core/Include/nmsis_gcc.h"

/* Provide ARM CMSIS compatible macros */

/* ARM CMSIS uses __STATIC_FORCEINLINE, NMSIS provides the same */
#ifndef __STATIC_FORCEINLINE
  #define __STATIC_FORCEINLINE __attribute__((always_inline)) static inline
#endif

#ifndef __STATIC_INLINE
  #define __STATIC_INLINE static inline
#endif

#ifndef __ALIGNED
  #define __ALIGNED(x) __attribute__((aligned(x)))
#endif

/* LOW_OPTIMIZATION_ENTER/EXIT - not needed on RISC-V */
#ifndef LOW_OPTIMIZATION_ENTER
  #define LOW_OPTIMIZATION_ENTER
#endif

#ifndef LOW_OPTIMIZATION_EXIT
  #define LOW_OPTIMIZATION_EXIT
#endif

#ifndef IAR_ONLY_LOW_OPTIMIZATION_ENTER
  #define IAR_ONLY_LOW_OPTIMIZATION_ENTER
#endif

#ifndef IAR_ONLY_LOW_OPTIMIZATION_EXIT
  #define IAR_ONLY_LOW_OPTIMIZATION_EXIT
#endif

/* ARM status type compatibility */
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

#ifndef ARM_MATH_NANINF
  #define ARM_MATH_NANINF            -4
#endif

#ifndef ARM_MATH_SINGULAR
  #define ARM_MATH_SINGULAR          -5
#endif

#ifndef ARM_MATH_TEST_FAILURE
  #define ARM_MATH_TEST_FAILURE      -6
#endif

#ifndef ARM_MATH_LENGTH_MISMATCH
  #define ARM_MATH_LENGTH_MISMATCH   -7
#endif

#ifndef ARM_MATH_NOT_FOUND
  #define ARM_MATH_NOT_FOUND         -8
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CMSIS_COMPILER_H */
