#include "conditionvar.h"
#include <stdlib.h>

ConditionVariable *condition_var_init(void) {
    ConditionVariable *result = malloc(sizeof(ConditionVariable));

    pthread_cond_init(&result->condition, NULL);
    pthread_mutex_init(&result->mutex, NULL);

    return result;
}

void condition_var_free(ConditionVariable *cond) {
    pthread_cond_destroy(&cond->condition);
    pthread_mutex_destroy(&cond->mutex);
    free(cond);
}


bool condition_var_notify(ConditionVariable *cond)
{
    pthread_mutex_lock(&cond->mutex);
    pthread_cond_signal(&cond->condition);
    pthread_mutex_unlock(&cond->mutex);

    return true;
}

bool condition_var_broadcast(ConditionVariable *cond) {
    pthread_mutex_lock(&cond->mutex);
    pthread_cond_broadcast(&cond->condition);
    pthread_mutex_unlock(&cond->mutex);

    return true;
}

void condition_var_wait(ConditionVariable *cond)
{
    pthread_mutex_lock(&cond->mutex);
    pthread_cond_wait(&cond->condition, &cond->mutex);
    pthread_mutex_unlock(&cond->mutex);
}
