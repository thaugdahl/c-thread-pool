/// @file thread.h
/// @brief Thread utilities

#ifndef THREAD_H_ULSOKCVF
#define THREAD_H_ULSOKCVF

#include <pthread.h>
#include <stdbool.h>


// TODO: Refactor

typedef struct {
    pthread_cond_t condition;
    pthread_mutex_t mutex;
} CondMutexPair;

CondMutexPair *init_cond_mutex_pair(void);


//==------------------------------------------------------------==/
//= Thread Control Block
//==------------------------------------------------------------==/

typedef enum {
    ///< Ready to execute
    READY,

    ///< Waiting for condition / waker
    WAITING,

    ///< Terminated
    TERMINATED,

    ///< The thread is being destroyed
    DESTROYED
} ThreadState;

typedef struct {
    ///< The state of the thread
    ThreadState state;

    ///< Whether the thread has been instructed to terminate
    bool shouldTerminate;

    ///< The tracking ID of the thread
    size_t ID;

    ///< The access mutex for the control block
    ///< TODO: Refactor -- move outside control block
    pthread_mutex_t mutex;

    ///< pthread thread handle
    pthread_t thread;

    ///< The mutex/condition pair for the waiter / waker.
    CondMutexPair *waiter;
} ThreadControlBlock;



ThreadControlBlock init_thread_control_block(void);

void free_thread_control_waiter(CondMutexPair *pair);

ThreadControlBlock *free_thread_control_block(ThreadControlBlock *control);

//==------------------------------------------------------------==/
// Thread
//==------------------------------------------------------------==/

typedef struct {
    ThreadControlBlock control;
    void *arg;
} Thread;

/// @brief Clean up thread resources
/// @param thread The thread whose resources to free
Thread *free_thread(Thread *thread);

/// @brief Make the thread wait on a condition
/// @param thread The thread to make wait
void thread_wait(Thread *thread);

/// @brief Wake the thread by signalling a condition
/// @param thread The thread to wake
bool thread_wake(Thread *thread);

/// @brief Set the running status of the thread
/// @param thread The thread whose state to set
/// @param state The state to set on the thread.
void thread_set_state(Thread *thread, ThreadState state);

/// @brief Get the running state of the thread.
/// @param thread The thread whose state to get.
ThreadState thread_get_state(Thread *thread);

/// @brief Set the indicator that the thread should terminate
/// @param thread The thread to ask terminate
void thread_set_terminate(Thread *thread);

/// @brief Initialize the thread data
/// @param arg A pointer to the data the thread should be able to access
Thread thread_init_data(void *arg);

/// @brief Spawn a thread
/// @param routine The function the thread should invoke.
/// @param arg A pointer to the data the thread should have access to.
Thread *thread_spawn(void *(*routine)(void *), void *arg);

/// @brief Make the thread join
/// @param thread The thread to make join
/// @param res A pointer to where the result can be written.
void thread_join(Thread *thread, void **res);

#endif /* end of include guard: THREAD_H_ULSOKCVF */
