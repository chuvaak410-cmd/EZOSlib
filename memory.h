#ifndef EZOS_MEMORY
#define EZOS_MEMORY
#include "kernel.h";

static uint32_t free_memory_addr = 0x400000;
void* OSalloc(uint32_t size) {
  if(size % 4 != 0) {
    size = (size + 4) - (size % 4);
  }
  uint32_t ret = free_memory_addr;
  free_memory_addr += size;
  return (void*)ret;
}
 
#endif
