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

  // Extensions, can be re-ordered / added / removed, but assembly needs update
  uint16_t fs;  // +32
  uint16_t pad; // +34
  uint32_t eip; // +36

  uint32_t fs_base; //+40

} Registers;

#endif
