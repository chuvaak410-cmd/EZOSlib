#ifndef EZOS_CONTAINERS
#define EZOS_CONTAINERS
#include "memory.h"
#include "kalloc.h"
#include "bool.h"
typedef struct {
  void** data;
  uint32_t capacity;
  uint32_t size;
} vector_t;

typedef struct {
  uint8_t* buffer;
  uint32_t size;
  uint32_t head;
  uint32_t tail;
} ring_buffer_t;

static inline vector_t* vector_create(uint32_t init_capacity) {
  vector_t* vec = (vector_t*)kalloc(sizeof(vector_t));
  if (!vec) return 0;
  vec->data = (void**)kalloc(init_capacity * sizeof(void*));
  if(!vec->data) {
    kfree(vec);
    return 0;
  }
  vec->capacity = init_capacity;
  vec->size = 0;
  return vec;
}

static inline void vector_free(vector_t* vec) {
  if(!vec) return;
  if(vec->data) kfree(vec->data);
  kfree(vec);
}

static inline bool vector_push(vector_t* vec, void* element) {
  if(!vec) return false;
  if(vec->size >= vec->capacity) {
    uint32_t new_capacity = vec->capacity * 2;
    void** new_data = (void**)kalloc(new_capacity * sizeof(void*));
    if(!new_data) return false;
    for(uint32_t i = 0; i < vec->size; i++) {
      new_data[i] = vec->data[i];
    }
    kfree(vec->data);
    vec->data = new_data;
    vec->capacity = new_capacity;
  }
  vec->data[vec->size] = element;
  vec->size++;
  return true;
}

static inline void* vector_get(vector_t* vec, uint32_t index) {
  if(!vec || index >= vec->size) return 0;
  return vec->data[index];
}

static inline ring_buffer_t* ring_create(uint32_t size) {
  ring_buffer_t* rb =(ring_buffer_t*)kalloc(sizeof(ring_buffer_t));
  if(!rb) return 0;
  rb->buffer = (uint8_t*)kalloc(size);
  if(!rb->buffer) {
    kfree(rb);
    return 0;
  }
  rb->size = size;
  rb->head = 0;
  rb->tail = 0;
  return rb;
}

static inline void ring_free(ring_buffer_t* rb) {
  if(!rb) return;
  if(rb->buffer) kfree(rb->buffer);
  kfree(rb);
}

static inline bool ring_push(ring_buffer_t* rb, uint8_t val) {
  if(!rb) return false;
  uint32_t next = (rb->head + 1) % rb->size;
  if(next == rb->tail) return false;
  rb->buffer[rb->head] = val;
  rb->head = next;
  return true;
}

static inline bool ring_pop(ring_buffer_t* rb, uint8_t* out_val) {
  if(!rb || rb->head == rb->tail) return false;
  *out_val = rb->buffer[rb->tail];
  rb->tail = (rb->tail + 1) % rb->size;
  return true;
}

#endif
