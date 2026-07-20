/*
 * arm_common_tables.h - Compatibility header for RISC-V NMSIS DSP
 *
 * Maps ARM CMSIS common tables to RISC-V NMSIS equivalents.
 */

#ifndef _ARM_COMMON_TABLES_H
#define _ARM_COMMON_TABLES_H

#include "arm_math.h"

/* Include RISC-V NMSIS common tables */
#include "Include/riscv_common_tables.h"

/* Map ARM-named tables to RISC-V equivalents */
/* Note: twiddleCoef_XX tables have the same names in both libraries */

/* Bit reversal table - different naming between ARM and RISC-V */
#define armBitRevTable riscvBitRevTable

/* sin/cos tables */
#define sinTable_f32   riscv_sinTable_f32
#define sinTable_q31   riscv_sinTable_q31
#define sinTable_q15   riscv_sinTable_q15

#endif /* _ARM_COMMON_TABLES_H */
