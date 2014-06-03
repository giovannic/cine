/* This file was automatically generated.  Do not edit! */
#ifndef CINE_H_
#define CINE_H_

void cine_method_registration(char *name,int mid);
void cine_exit_thread();
void cine_teardown();
void cine_timer_exit(int id);
void cine_timer_entry(int id);
void cine_start_thread();
int cine_thread_create(pthread_t *thread,const pthread_attr_t *attr,void *(*start_routine)(void *),void *arg);
int orig_thread_create(pthread_t *thread,const pthread_attr_t *attr,void *(*start_routine)(void *),void *arg);
void cine_init();
void cine_new_thread();
void cine_initial_thread();
int cine_join(pthread_t thread,void **value_ptr);
int cine_yield();
int cine_sleep(unsigned int t);
int cine_cond_signal(pthread_cond_t *cond);
int cine_cond_broadcast(pthread_cond_t *cond);
int cine_cond_timedwait(pthread_cond_t *cond,pthread_mutex_t *mutex,const struct timespec *abstime);
int cine_cond_wait(pthread_cond_t *cond,pthread_mutex_t *mutex);
int cine_mutex_unlock(pthread_mutex_t *mutex);
int cine_mutex_lock(pthread_mutex_t *mutex);

extern pthread_mutex_t cine_mutex;
extern pthread_t init_thread;
extern bool initialised;
extern long end_count;
extern long thread_count;
/*
extern using namespace VEX;
extern using namespace std;
*/

#endif /* CINE_H_ */


