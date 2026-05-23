#ifndef EZOS_KALLOC
#define EZOS_KALLOC
#include "kernel.h"

typedef struct BlockHeader{
  uint32_t size;
  uint8_t is_free;
  struct BlockHeader* next;
} BlockHeader;
static BlockHeader* heap_start = (BlockHeader*)0x400000;
static uint8_t is_heap_init = 0;
void kinit(uint32_t init_size) {
  heap_start->size = init_size - sizeof(BlockHeader);
  heap_start->is_free = 1;
  heap_start->next = 0; //NULL
  is_heap_init = 1;
}
void* kalloc(uint32_t size) {
  if(!is_heap_init) {
    kinit(1024 * 1024 * 4);
  }
  if(size % 4 != 0) {
    size = (size + 4) - (size % 4);
  }
  BlockHeader* current = heap_start;
  while(current != 0) {
    if(current->is_free && current->size >= size) {
      if(current->size >= size + sizeof(BlockHeader) + 4) {
        BlockHeader* next_block = (BlockHeader*)((uint32_t)current+sizeof(BlockHeader)+size);
        next_block->size = current->size - size - sizeof(BlockHeader);
        next_block->is_free = 1;
        next_block->next = current->next;
        current->size = size;
        current->next = next_block;
      }
      current->is_free = 0;
      return (void*)((uint32_t)current + sizeof(BlockHeader));
    }
    current = current->next;
  }
  return 0;
}

void kfree(void* ptr) {
  if(ptr == 0) return;
  BlockHeader* header = (BlockHeader*)((uint32_t)ptr - sizeof(BlockHeader));
  header->is_free = 1;
  BlockHeader* current = heap_start;
  while(current != 0 && current->next != 0) {
    if(current->is_free && current->next->is_free) {
      current->size += sizeof(BlockHeader) + current->next->size;
      current->next = current->next->next;
    } else {
      current = current->next;
    }
  }
}

#endif
