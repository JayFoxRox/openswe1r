// Copyright 2019 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

// This is a temporary placeholder for a memory allocator.

#include "alloc.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static unsigned int block_count = 0;
static uint32_t block_size = 0;
static uint8_t* block_usage = NULL;

static void mark_used(unsigned int block, bool used) {
  uint8_t mask = 1 << (block % 8);
  if (used) {
    block_usage[block / 8] |= mask;
  } else {
    block_usage[block / 8] &= ~mask;
  }
}

static bool is_used(unsigned int block) {
  uint8_t mask = 1 << (block % 8);
  return block_usage[block / 8] & mask;
}

static unsigned int get_block_count(uint32_t size) {
  return (size + block_size - 1) / block_size;
}

static unsigned int count_blocks(unsigned int block, bool used) {
  unsigned int count = 0;
  while(block < block_count) {
    if (is_used(block) != used) {
      break;
    }
    block++;
    count++;
  }
  return count;
}

static unsigned int find_free_blocks(unsigned int needed_blocks) {

  unsigned int best_count = 0;
  unsigned int best_block = 0;

  unsigned int block = 0;
  while(block < block_count) {

    bool block_used = is_used(block);
    unsigned int available_blocks = count_blocks(block, block_used);

    // Only look at unused blocks that are large enough
    if (!block_used && (available_blocks >= needed_blocks)) {

      // Check if we don't have a result yet, or if this is a better fit
      if ((best_count == 0) || (available_blocks < best_count)) {

        // Update result
        best_block = block;
        best_count = available_blocks;

        // If we have an exact fit, use that
        if (best_count == needed_blocks) {
          break;
        }

      }
    }

    block += available_blocks;
  }

  // Assert that we have a result
  assert(best_count != 0);

  // Returns best block address
  return best_block;
}

void alloc_initialize(uint32_t size, uint32_t _block_size) {
  
  // Sanity check arguments, we expect proper alignment
  assert((size % _block_size) == 0);

  // Store settings
  block_size = _block_size;
  block_count = get_block_count(size);

  // Allocate flags which tell us wether a block is used
  size_t used_flag_size = (block_count + 7) / 8;
  block_usage = malloc(used_flag_size);
  memset(block_usage, 0x00, used_flag_size);

}

uint32_t alloc_allocate(uint32_t size) {

  // Don't allow zero-size allocations (breaks algorithm)
  assert(size > 0);

  unsigned int needed_blocks = get_block_count(size);

  //FIXME: `+ 2` is a hack, so we can recognize block splits
  unsigned int block = find_free_blocks(needed_blocks + 2) + 1;

  printf("Allocated %u blocks (0x%X / 0x%X bytes)\n", needed_blocks, needed_blocks * block_size, size);

  for(unsigned int i = 0; i < needed_blocks; i++) {
    mark_used(block + i, true);
  }

  return block * block_size;
}

void alloc_free(uint32_t offset) {

  // Only free addresses that are block-aligned
  assert(offset % block_size == 0);
  unsigned int block = offset / block_size;

  unsigned int used_blocks = count_blocks(block, true);

  // Avoid double-free
  assert(used_blocks > 0);

  for(unsigned int i = 0; i < used_blocks; i++) {
    mark_used(block + i, false);
  }

}
