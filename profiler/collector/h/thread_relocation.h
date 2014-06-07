/* This file was automatically generated.  Do not edit! */
int orig_create(pthread_t *thread,const pthread_attr_t *attr,void *(*start_routine)(void *),void *arg);
int orig_join(pthread_t thread,void **value_ptr);
int orig_yield();
int orig_sleep(unsigned int t);
int orig_cond_signal(pthread_cond_t *cond);
int orig_cond_broadcast(pthread_cond_t *cond);
int orig_cond_timedwait(pthread_cond_t *cond,pthread_mutex_t *mutex,const struct timespec *abstime);
int orig_cond_wait(pthread_cond_t *cond,pthread_mutex_t *mutex);
int orig_mutex_unlock(pthread_mutex_t *mutex);
int orig_mutex_lock(pthread_mutex_t *mutex);
