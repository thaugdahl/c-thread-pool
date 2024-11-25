#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "circbuffer.h"
#include "thread.h"
#include "threadpool.h"

#define NUM_THREADS 32

#define DEBUG

#ifdef DEBUG
#define DEBUGMSG(...) fprintf(stderr, "DEBUG " __VA_ARGS__)
#else
#define DEBUGMSG(x, ...) 0
#endif

const char * const TO_OUTPUT = "Hello World!";

void *wait_for_stuff(void *args)
{
    Thread *data = (Thread *) args;
    thread_set_state(data, READY);

    CircularBuffer *buffer = (CircularBuffer *) data->arg;

    while ( true ) {
        if ( data->control.shouldTerminate == true ) break;

        DEBUGMSG("%zu: Waiting for stuff...\n", data->control.ID);
        thread_wait(data);
        DEBUGMSG("%zu, Got it!\n", data->control.ID);

        int current = 0;
        while ( circular_buffer_take(buffer, &current) ) {
            printf("%d ", current);
        }
        printf("\n");

    }

    DEBUGMSG("%zu OUTSIDE\n", data->control.ID);

    thread_set_state(data, TERMINATED);

    return NULL;
}

void give_thread_work_to_do(CircularBuffer *buffer)
{
    // Giving the threads work to do
    int num_to_add = 50 + (int) (((float) rand() / (float) RAND_MAX) * (128 / NUM_THREADS));
    for ( int k = 0; k < num_to_add; k++ ) {
        circular_buffer_insert(buffer, rand());
    }
}

int main(int argc, char *argv[])
{
    // Thread *thread = thread_spawn(wait_for_stuff, (void *) NULL);

    // Thread * threads[16];
    // CircularBuffer buffers[16];

    ThreadPool pool = thread_pool_init(NUM_THREADS, sizeof(CircularBuffer));
    CircularBuffer *buffers = (CircularBuffer *) pool.data;

    for ( size_t i = 0; i < NUM_THREADS; i++ ) {
        buffers[i] = circular_buffer_init(4096);
        pool.threads[i] = thread_spawn(wait_for_stuff, (void *) &buffers[i]);
    }

    int count = 0;

    while ( true ) {

        if ( count > 1000 ) {
            thread_pool_foreach(&pool, thread_set_terminate);
            break;
        }

        for ( size_t i = 0; i < NUM_THREADS; i++ )
        {
                // TODO: Lock to handle race conditions
                give_thread_work_to_do(&buffers[i]);

                thread_wake(thread_pool_get(&pool, i));

                // Simulate work
                for ( int z = 0; z < 1000; z++ ) {
                    double k = 9000.0;
                    k = sqrt(k);
                }

                count++;
        }
    }

    for ( size_t i = 0; i < NUM_THREADS; i++ ) {
        while ( thread_get_state(thread_pool_get(&pool, i)) != TERMINATED ) {
            thread_wake(thread_pool_get(&pool, i));
        }
    }

    for ( size_t i = 0; i < NUM_THREADS; i++ ) {
        thread_join(thread_pool_get(&pool, i), NULL);
        free(buffers[i].buffer);
    }

    DEBUGMSG("%s\n", "ROOT: Everything is done\n");


    thread_pool_finalize(&pool);

    return 0;
}
