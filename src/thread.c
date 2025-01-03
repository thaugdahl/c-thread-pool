#include "thread.h"
#include <stdlib.h>
#include <stdio.h>

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

ThreadState thread_get_state(Thread *thread) {
  pthread_mutex_lock(&thread->control.mutex);
    ThreadState res = thread->control.state;
  pthread_mutex_unlock(&thread->control.mutex);
    return res;
}

void thread_set_state(Thread *thread, ThreadState state) {
  pthread_mutex_lock(&thread->control.mutex);
  thread->control.state = state;
  pthread_mutex_unlock(&thread->control.mutex);
}

bool thread_wake(Thread *thread) {
    return condition_var_notify(thread->control.waiter);
}

void thread_wait(Thread *thread) {
    condition_var_wait(thread->control.waiter);
}

Thread *thread_free(Thread *thread) {
  thread_free_control_block(&thread->control);
  free(thread);
  return NULL;
}

ThreadControlBlock *thread_free_control_block(ThreadControlBlock *control) {
  thread_free_control_waiter(control->waiter);
  free(control->waiter);
  control->waiter = NULL;

  pthread_mutex_destroy(&control->mutex);
  return NULL;
}

void thread_free_control_waiter(ConditionVariable *pair) {
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
  result.waiter = condition_var_init();

  return result;
}

