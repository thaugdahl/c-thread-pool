//==------------------------------------------------------------==/
//= Thread Pool
//=
//= Author: Tor Andre Haugdahl <thaugdahl@gmail.com>
//==------------------------------------------------------------==/

#ifndef THREADPOOL_H_4MNCWQAX
#define THREADPOOL_H_4MNCWQAX

#include <stddef.h>
#include <stdlib.h>
#include "thread.h"

typedef struct {
    size_t num_threads;

    void **data;
    size_t data_member_size;
    Thread **threads;

} ThreadPool;

//< Initialize the Thread Pool
ThreadPool thread_pool_init(size_t num_threads, size_t data_size);

//< Perform a routine on every thread in the pool
bool thread_pool_foreach(ThreadPool *pool, void (*routine)(Thread *));

//< Get a thread by index
Thread *thread_pool_get(ThreadPool *pool, size_t index);

//< Terminate all threads
bool thread_pool_terminate(ThreadPool *pool);

//< Free and finalize all threads
bool thread_pool_finalize(ThreadPool *pool);

#endif /* end of include guard: THREADPOOL_H_4MNCWQAX */
