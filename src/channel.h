#ifndef CHANNEL_H_BHTQ6DER
#define CHANNEL_H_BHTQ6DER

#include "circbuffer.h"
#include "thread.h"
#include "conditionvar.h"
#include <stdlib.h>

// TODO: Add unbuffered sends and receives

typedef struct Channel_ {
    // The channel needs a mutex and a condition pair
    pthread_mutex_t mutex;
    pthread_cond_t notEmpty;
    pthread_cond_t notFull;

    bool closed;

    CircularBuffer circBuffer;
} Channel;

Channel *channel_init(void);

bool channel_full(Channel *channel);

int channel_send(Channel *channel, void *data);

int channel_recv(Channel *channel, void **data);

bool channel_empty(Channel *channel);

void channel_wait(Channel *channel);

void channel_lock(Channel *channel);
void channel_unlock(Channel *channel);

void channel_destroy(Channel *chan);

void channel_close(Channel *chan);

bool channel_is_closed(Channel *chan);




#endif /* end of include guard: CHANNEL_H_BHTQ6DER */

