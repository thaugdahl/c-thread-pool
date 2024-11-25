#ifndef CIRCBUFFER_H_WMOCT17Y
#define CIRCBUFFER_H_WMOCT17Y

#include <stddef.h>
#include <stdbool.h>

typedef struct CircularBuffer_
{
    size_t capacity;
    size_t head;
    size_t tail;
    int *buffer;
} CircularBuffer;

CircularBuffer circular_buffer_init(size_t capacity_);

bool circular_buffer_empty(CircularBuffer *buffer);

size_t circular_buffer_next_index(CircularBuffer *buffer, size_t index);

bool circular_buffer_full(CircularBuffer *buffer);

bool circular_buffer_insert(CircularBuffer *buffer, int elem);

bool circular_buffer_take(CircularBuffer *buffer, int *res);

#endif /* end of include guard: CIRCBUFFER_H_WMOCT17Y */
