// Copyright 2018 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#ifndef __OPENSWE1R_UC_NATIVE_H__
#define __OPENSWE1R_UC_NATIVE_H__


typedef struct {

  // These are accessed by PUSHA / POPA, so they must come first.
  // They must also be kept in this order.
  uint32_t edi; // +0
  uint32_t esi; // +4
  uint32_t ebp; // +8
  uint32_t esp; // +12
  uint32_t ebx; // +16
  uint32_t edx; // +20
  uint32_t ecx; // +24
  uint32_t eax; // +28

  // Hack because we always do pusha and pushf in sequence
  uint32_t eflags; // +32

  // Extensions, can be re-ordered / added / removed, but assembly needs update
  uint16_t fs;  // +36
  uint16_t pad; // +40
  uint32_t eip; // +44

  uint32_t fs_base; //+48

} Registers;

extern uint32_t guest_registers_esp asm("guest_registers_esp");
extern Registers* guest_registers asm("guest_registers");
extern uint8_t guest_registers_fpu[512] asm("guest_registers_fpu");
extern uint8_t host_registers_fpu[512] asm("host_registers_fpu");
extern uint32_t host_esp asm("host_esp");
extern uint32_t host_fs asm("host_fs");
extern uint32_t host_eip asm("host_eip");

#endif
