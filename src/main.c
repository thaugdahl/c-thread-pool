#define _POSIX_C_SOURCE 199309L
#include "channel.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>

#include "circbuffer.h"
#include "thread.h"
#include "threadpool.h"

#define NUM_THREADS 32

#define NDEBUG

#ifdef DEBUG
#define DEBUGMSG(...) fprintf(stderr, "DEBUG " __VA_ARGS__)
#else
#define DEBUGMSG(x, ...) 0
#endif

typedef struct Array_ {
    size_t size;
    int *buffer;
} Array;

Array array_init(size_t size) {
    Array res;

    res.buffer = malloc(sizeof(int) * size);
    res.size = size;

    return res;
}

void *wait_for_stuff(void *args)
{
    __attribute__((unused)) Thread *data = (Thread *) args;
    thread_set_state(data, READY);

    Channel *channel = (Channel *) data->arg;

    while ( ! channel_is_closed(channel) ) {
        // if ( data->control.shouldTerminate == true ) break;

        DEBUGMSG("%zu: Waiting for stuff...\n", data->control.ID);
        // thread_wait(data);
        // DEBUGMSG("%zu, Got it!\n", data->control.ID);

        // Get the message
        Array *current = NULL;

        current = (Array *) channel_recv(channel);

        if ( current == NULL ) {
            DEBUGMSG("%zu, Got a NULL result!\n", data->control.ID);
            break;
        }
        DEBUGMSG("%zu, Got it!\n", data->control.ID);


        for ( size_t i = 0; i < current->size; i++ ) {
            //printf("%d ", current->buffer[i]);
        }
        // printf("\n");
        free(current->buffer);
        free(current);
    }

    DEBUGMSG("%zu OUTSIDE\n", data->control.ID);

    thread_set_state(data, TERMINATED);

    DEBUGMSG("%zu DONE SETTING STATE\n", data->control.ID);

    printf("Thread %zu exiting\n", data->control.ID);

    return NULL;
}


void give_thread_work_to_do(Channel *channel)
{
    // Giving the threads work to do
    size_t num_to_add = 50 + (size_t) (((float) rand() / (float) RAND_MAX) * (128 / NUM_THREADS));

    Array *arr = malloc(sizeof(Array));
    *arr = array_init((size_t) num_to_add);

    for ( int k = 0; k < num_to_add - 1; k++ ) {
        arr->buffer[k] = rand();
    }

    channel_send(channel, arr);
}

void close_channels(Thread *t) {
    channel_close((Channel*)t->arg);
}

int main(int argc, char *argv[])
{
    ThreadPool pool = thread_pool_init(NUM_THREADS, sizeof(CircularBuffer));

    Channel **channels = (Channel **) pool.data;

    for ( size_t i = 0; i < NUM_THREADS; i++ ) {
        channels[i] = channel_init();
        pool.threads[i] = thread_spawn(wait_for_stuff, (void *) channels[i]);
    }

    int count = 0;

    while ( true ) {

        if ( count > 100000 ) {
            thread_pool_foreach(&pool, close_channels);

            for ( size_t i = 0; i < pool.num_threads; i++ ) {
                channel_send(channels[i], NULL);
            }
            break;
        }

        for ( size_t i = 0; i < NUM_THREADS; i++ )
        {
                give_thread_work_to_do(channels[i]);
                // thread_wake(thread_pool_get(&pool, i));

                // Simulate work
                for ( int z = 0; z < 1000; z++ ) {
                    double k = 9000.0;
                    k = sqrt(k);
                }

                count++;
        }
    }

    bool waiting = true;
    while ( waiting ) {
        waiting = false;
        for ( size_t i = 0; i < NUM_THREADS; i++ ) {
            waiting = waiting || thread_get_state(thread_pool_get(&pool, i)) != TERMINATED;
            DEBUGMSG("Waiting for %zu\n", thread_pool_get(&pool, i)->control.ID);
        }
    }

    thread_pool_foreach(&pool, close_channels);

    for ( size_t i = 0; i < NUM_THREADS; i++ ) {
        thread_join(thread_pool_get(&pool, i), NULL);
        channel_destroy(channels[i]);
    }

    DEBUGMSG("%s\n", "ROOT: Everything is done\n");
    thread_pool_finalize(&pool);

    return 0;
}
