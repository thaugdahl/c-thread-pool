#include "circbuffer.h"
#include <stdlib.h>
#include <stdio.h>

CircularBuffer circular_buffer_init(size_t capacity_) {
  CircularBuffer result;
  result.capacity = capacity_;

  result.head = 0;
  result.tail = 0;

  result.buffer = malloc(sizeof(void *) * capacity_);

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

bool circular_buffer_insert(CircularBuffer *buffer, void *elem) {
  if (circular_buffer_full(buffer)) {
    return false;
  }

  size_t next = circular_buffer_next_index(buffer, buffer->head);

  buffer->buffer[buffer->head] = elem;
  buffer->head = next;

  return true;
}

size_t circular_buffer_size(CircularBuffer *buffer)
{
    return (buffer->head + buffer->tail - 1) % buffer->capacity;
}

bool circular_buffer_take(CircularBuffer *buffer, void **res) {
  if (circular_buffer_empty(buffer))
    return false;

  size_t next = circular_buffer_next_index(buffer, buffer->tail);
  *res = buffer->buffer[buffer->tail];

    buffer->buffer[buffer->tail] = NULL;


  buffer->tail = next;
  return true;
}

void circular_buffer_free(CircularBuffer *buffer)
{
    free(buffer->buffer);
}

