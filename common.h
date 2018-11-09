// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#ifndef __OPENSWE1R_COMMON_H__
#define __OPENSWE1R_COMMON_H__

#include <stdbool.h>
#include <stdint.h>

#if defined(_WIN32)
#if defined(XBOX)
#  include <xboxkrnl/xboxkrnl.h>
#else
#  include <malloc.h>
#endif
#else
#  include <stdlib.h>
#endif

static bool IsZero(const void* data, size_t size) {
  uint8_t* bytes = (uint8_t*)data;
  for(size_t i = 0; i < size; i++) {
    if (*bytes++ != 0x00) {
      return false;
    }
  }
  return true;
}

static uint32_t AlignDown(uint32_t address, uint32_t size) {
  return address - address % size;
}

static uint32_t AlignUp(uint32_t address, uint32_t size) {
  return AlignDown(address + size - 1, size);
}

static void* aligned_malloc(size_t alignment, size_t size) {
  void* ptr;
#if defined(_WIN32)
#ifdef XBOX
  ptr = MmAllocateContiguousMemoryEx(size, 0x00000000, 0xFFFFFFFF, alignment, PAGE_READWRITE);
#else
  ptr = _aligned_malloc(size, alignment);
#endif
#else
  posix_memalign(&ptr, alignment, size);
#endif
  assert(ptr != NULL);
  return ptr;
}

static void aligned_free(void* ptr) {
#if defined(_WIN32)
#ifdef XBOX
  MmFreeContiguousMemory(ptr);
#else
  _aligned_free(ptr);
#endif
#else
  free(ptr);
#endif
}

#endif
