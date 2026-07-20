/*
 * arm_const_structs.h - Compatibility header for RISC-V NMSIS DSP
 *
 * Maps ARM CMSIS const structs to RISC-V NMSIS equivalents.
 */

#ifndef _ARM_CONST_STRUCTS_H
#define _ARM_CONST_STRUCTS_H

#include "arm_math.h"
#include "arm_common_tables.h"

/* Include RISC-V NMSIS const structs */
#include "Include/riscv_const_structs.h"

/* Map ARM-named const structs to RISC-V equivalents */
/* The const structs have the same names in both libraries (riscv_cfft_sR_f32_lenXX) */
/* and arm_cfft_sR_f32_lenXX is already mapped via #define in arm_math.h */

#endif /* _ARM_CONST_STRUCTS_H */
