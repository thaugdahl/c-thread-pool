#include "threadpool.h"

bool thread_pool_finalize(ThreadPool *pool) {
  for (size_t i = 0; i < pool->num_threads; i++) {
    thread_free(pool->threads[i]);
    pool->threads[i] = NULL;
  }

    free(pool->threads);
    free(pool->data);

  return true;
}

bool thread_pool_terminate(ThreadPool *pool) {
  bool unterminated = true;
  while (unterminated) {
    unterminated = false;
    for (size_t i = 0; i < pool->num_threads; i++) {
      if (thread_get_state(thread_pool_get(pool, i)) != TERMINATED) {
        unterminated = false || unterminated;
        thread_wake(pool->threads[i]);
      }
    }
  }

  return true;
}

Thread *thread_pool_get(ThreadPool *pool, size_t index) {
  return pool->threads[index];
}

bool thread_pool_foreach(ThreadPool *pool, void (*routine)(Thread *)) {
  for (size_t i = 0; i < pool->num_threads; i++) {
    if (pool->threads[i] == NULL)
      continue;

    routine(pool->threads[i]);
  }

  return true;
}

ThreadPool thread_pool_init(size_t num_threads, size_t data_size) {
  ThreadPool result;

  result.data_member_size = data_size;
  result.data = (void **)malloc(data_size * num_threads);
  result.threads = (Thread **)calloc(num_threads, sizeof(Thread));

  result.num_threads = num_threads;

  return result;
}
