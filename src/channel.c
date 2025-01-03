#include "channel.h"

Channel *channel_init(void) {
    Channel *result = (Channel *)malloc(sizeof(Channel));

    pthread_mutex_init(&result->mutex, NULL);
    pthread_cond_init(&result->notEmpty, NULL);
    pthread_cond_init(&result->notFull, NULL);

    result->circBuffer = circular_buffer_init(128);

    return result;
}

void channel_destroy(Channel *channel)
{
    circular_buffer_free(&channel->circBuffer);
    pthread_mutex_destroy(&channel->mutex);
    pthread_cond_destroy(&channel->notFull);
    pthread_cond_destroy(&channel->notEmpty);
    free(channel);
}

int channel_send(Channel *channel, void *data)
{

    if ( channel_is_closed(channel) ) {
        return -1;
    }

    channel_lock(channel);

    // Wait for the channel to be ready
    while ( channel_full(channel) ) {
        pthread_cond_wait(&channel->notFull, &channel->mutex);
    }

    circular_buffer_insert(&channel->circBuffer, data);

    pthread_cond_signal(&channel->notEmpty);
    channel_unlock(channel);

    return 0;
}

int channel_recv(Channel *channel, void **data) {
     // channel_lock(channel);


    // Wait for channel to get data
     while ( channel_empty(channel) ) {

        // Need to check if the channel was closed
        // If the receiving thread was notified and resumed,
        // but nothing is added to the channel, then it is likely
        // that the channel was closed.
        if ( channel_is_closed(channel) ) {
            return -1;
        }

        channel_lock(channel);
        pthread_cond_wait(&channel->notEmpty, &channel->mutex);
        channel_unlock(channel);
     }

    circular_buffer_take(&channel->circBuffer, data);

    pthread_cond_signal(&channel->notFull);
    channel_unlock(channel);

    return 0;
 }


bool channel_full(Channel *channel) {
    return circular_buffer_full(&channel->circBuffer);
}

bool channel_empty(Channel *channel) {
    return circular_buffer_empty(&channel->circBuffer);
}

void channel_lock(Channel *channel) {
    pthread_mutex_lock(&channel->mutex);
}

void channel_unlock(Channel *channel) {
    pthread_mutex_unlock(&channel->mutex);
}

void channel_close(Channel *chan) {
    pthread_mutex_lock(&chan->mutex);
    chan->closed = true;
    pthread_cond_broadcast(&chan->notEmpty);
    pthread_cond_broadcast(&chan->notFull);
    pthread_mutex_unlock(&chan->mutex);
}

bool channel_is_closed(Channel *chan)
{
    pthread_mutex_lock(&chan->mutex);
    bool res = chan->closed;
    pthread_mutex_unlock(&chan->mutex);
    return res;
}
