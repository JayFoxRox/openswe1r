#include <stdint.h>

void alloc_initialize(uint32_t size, uint32_t _block_size);
uint32_t alloc_allocate(uint32_t size);
void alloc_free(uint32_t offset);
