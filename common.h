// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#ifndef __OPENSWE1R_COMMON_H__
#define __OPENSWE1R_COMMON_H__

#include <stdbool.h>
#include <stdint.h>

#if defined(XBOX)
#  include <xboxkrnl/xboxkrnl.h>
#elif defined(_WIN32)
#  include <malloc.h>
#else
#  include <sys/mman.h>
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
#ifdef XBOX
//  ptr = MmAllocateContiguousMemoryEx(size, 0x00000000, 0xFFFFFFFF, alignment, PAGE_READWRITE);
  ptr = NULL;
  DWORD allocated_size = size;
  NTSTATUS status = NtAllocateVirtualMemory(&ptr, 0, &allocated_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  assert(status == STATUS_SUCCESS);
#elif defined(_WIN32)
  ptr = _aligned_malloc(size, alignment);
#else
  posix_memalign(&ptr, alignment, size);
  mprotect(ptr, size, PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
  assert(ptr <= 0xFFFFFFFF);
  assert(ptr != NULL);
  return ptr;
}

static void aligned_free(void* ptr) {
#ifdef XBOX
//  MmFreeContiguousMemory(ptr);
  DWORD dwSize = 0;
  NTSTATUS status = NtFreeVirtualMemory(&ptr, &dwSize, MEM_RELEASE);
  assert(status == STATUS_SUCCESS);
#elif defined(_WIN32)
  _aligned_free(ptr);
#else
  free(ptr);
#endif
}

#endif
