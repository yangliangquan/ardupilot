/*
  MCU identification tables for the WCH CH32H417 (QingKe V5F, RISC-V).

  The STM32 tables key off DBGMCU->IDCODE.  The CH32H417 exposes its
  device identification through the WCH debug/DBGMCU registers instead,
  so the CH32 path in support.cpp reads a fixed device id and this table
  only supplies the human readable name and silicon revisions.
*/

#pragma once

#if defined(CH32H417)

#define CH32_UNKNOWN                0x000
#define CH32_H417                   0x417

mcu_des_t mcu_descriptions[] = {
    { CH32_UNKNOWN,    "CH32H417???"},
    { CH32_H417,       "CH32H417"},
};

const mcu_rev_t silicon_revs[] = {
    {0x1000, 'A'},
    {0x1001, 'Z'},
    {0x2001, 'X'},
};

#endif // CH32H417
