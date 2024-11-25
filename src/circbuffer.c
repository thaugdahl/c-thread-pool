#include "circbuffer.h"
#include <stdlib.h>
#include <stdio.h>

CircularBuffer circular_buffer_init(size_t capacity_) {
  CircularBuffer result;
  result.capacity = capacity_;

  result.head = 0;
  result.tail = 0;

  result.buffer = malloc(sizeof(int) * capacity_);

  if (!result.buffer) {
    fprintf(stderr, "Failed to allocate circular buffer with capacity %zu\n",
            capacity_);
  }

  return result;
}
bool circular_buffer_empty(CircularBuffer *buffer) {
  return buffer->head == buffer->tail;
}

size_t circular_buffer_next_index(CircularBuffer *buffer, size_t index) {
  if (index == buffer->capacity - 1) {
    return 0;
  }

  return index + 1;
}

bool circular_buffer_full(CircularBuffer *buffer) {
  size_t next = circular_buffer_next_index(buffer, buffer->head);

  if (next == buffer->tail)
    return true;
  return false;
}

bool circular_buffer_insert(CircularBuffer *buffer, int elem) {
  if (circular_buffer_full(buffer)) {
    return false;
  }

  size_t next = circular_buffer_next_index(buffer, buffer->head);

  buffer->buffer[next] = elem;
  buffer->head = next;

  return true;
}

bool circular_buffer_take(CircularBuffer *buffer, int *res) {
  if (circular_buffer_empty(buffer))
    return false;

  size_t next = circular_buffer_next_index(buffer, buffer->tail);
  *res = buffer->buffer[buffer->tail];

  buffer->tail = next;
  return true;
}

