#include "thread.h"
#include <stdlib.h>

void thread_join(Thread *thread, void **res) {
  pthread_join(thread->control.thread, res);
}

Thread *thread_spawn(void *(*routine)(void *), void *arg) {
  Thread *data = malloc(sizeof(Thread));
  *data = thread_init_data(arg);
  pthread_create(&data->control.thread, NULL, routine, data);

  return data;
}

Thread thread_init_data(void *arg) {
  Thread result;
  result.control = init_thread_control_block();
  result.arg = arg;

  return result;
}

void thread_set_terminate(Thread *thread) {
  pthread_mutex_lock(&thread->control.mutex);
  thread->control.shouldTerminate = true;
  pthread_mutex_unlock(&thread->control.mutex);
}

ThreadState thread_get_state(Thread *thread) { return thread->control.state; }
void thread_set_state(Thread *thread, ThreadState state) {
  pthread_mutex_lock(&thread->control.mutex);
  thread->control.state = state;
  pthread_mutex_unlock(&thread->control.mutex);
}

bool thread_wake(Thread *thread) {
  pthread_mutex_lock(&thread->control.waiter->mutex);
  pthread_cond_signal(&thread->control.waiter->condition);
  pthread_mutex_unlock(&thread->control.waiter->mutex);

  return true;
}

void thread_wait(Thread *thread) {
  pthread_mutex_lock(&thread->control.waiter->mutex);
  pthread_cond_wait(&thread->control.waiter->condition,
                    &thread->control.waiter->mutex);
  pthread_mutex_unlock(&thread->control.waiter->mutex);
}

Thread *free_thread(Thread *thread) {
  free_thread_control_block(&thread->control);
  free(thread);
  return NULL;
}

ThreadControlBlock *free_thread_control_block(ThreadControlBlock *control) {
  free_thread_control_waiter(control->waiter);
  free(control->waiter);
  control->waiter = NULL;

  pthread_mutex_destroy(&control->mutex);
  return NULL;
}

void free_thread_control_waiter(CondMutexPair *pair) {
  pthread_mutex_destroy(&pair->mutex);
  pthread_cond_destroy(&pair->condition);
}

ThreadControlBlock init_thread_control_block(void) {
  static size_t i = 4;

  ThreadControlBlock result;
  result.state = READY;
  result.ID = i++;
  pthread_mutex_init(&result.mutex, NULL);
  result.shouldTerminate = false;
  result.waiter = init_cond_mutex_pair();

  return result;
}

CondMutexPair *init_cond_mutex_pair(void) {
  CondMutexPair *result = malloc(sizeof(CondMutexPair));

  pthread_cond_init(&result->condition, NULL);
  pthread_mutex_init(&result->mutex, NULL);

  return result;
}
