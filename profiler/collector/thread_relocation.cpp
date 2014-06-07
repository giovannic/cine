/*
 * origlib.cpp
 *
 *  Created on: 24 Apr 2014
 *      Author: giovanni
 */

#include <pthread.h>
#include <thread_relocation.h>

int orig_mutex_lock(pthread_mutex_t *mutex){
	return 1;
}

int orig_mutex_unlock(pthread_mutex_t *mutex){
	return 1;
}

int orig_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){
	return 1;
}

int orig_cond_timedwait(pthread_cond_t *cond,
		pthread_mutex_t *mutex, const struct timespec *abstime){
	return 1;
}

int orig_cond_broadcast(pthread_cond_t *cond){
	return 1;
}

int orig_cond_signal(pthread_cond_t *cond){
	return 1;
}

int orig_sleep(unsigned int t){
	return 1;
}

int orig_yield(){
	return 1;
}

int orig_join(pthread_t thread, void **value_ptr){
	return 1;
}

int orig_create(pthread_t *thread, const pthread_attr_t *attr,
	                          void *(*start_routine) (void *), void *arg){
	return 1; //fail if this code actually hits
}
