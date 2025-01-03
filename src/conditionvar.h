#ifndef CONDITIONVAR_H_JCDM9NWT
#define CONDITIONVAR_H_JCDM9NWT

#include <pthread.h>
#include <stdbool.h>

typedef struct {
    pthread_cond_t condition;
    pthread_mutex_t mutex;
} ConditionVariable;

ConditionVariable *condition_var_init(void);
void condition_var_free(ConditionVariable *cond);

bool condition_var_notify(ConditionVariable *cond);
bool condition_var_broadcast(ConditionVariable *cond);
void condition_var_wait(ConditionVariable *cond);


#endif /* end of include guard: CONDITIONVAR_H_JCDM9NWT */

