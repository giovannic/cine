/*
 * cinelib.cpp
 *
 *  Created on: 24 Apr 2014
 *      Author: giovanni
 */

#include <stdlib.h>
#include <cstddef>
#include "cine.h"
#include <pthread.h>
#include <iostream>
#include "VTF.h"

//TODO:put this all into a namespace

using namespace std;
using namespace VEX;

long thread_count = 0;
long end_count = 0;
bool initialised = false;

pthread_t init_thread;
pthread_mutex_t cine_mutex;

//--Mutex methods
//put thread calls into a different library so
//that they can distinguish vex mutexes and
//input mutexes

int cine_mutex_lock(pthread_mutex_t *mutex){
	cerr << "requesting " << (long)mutex << endl;
	threadEventsBehaviour->onRequestingLock((long)mutex);
//	return pthread_mutex_lock(mutex);
}

int cine_mutex_unlock(pthread_mutex_t *mutex){
	cerr << "releasing " << (long)mutex << endl;
	threadEventsBehaviour->onReleasingLock((long)mutex);
//	return pthread_mutex_unlock(mutex);
}

int cine_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){
	cerr << "waiting cond " << (long)mutex << " " << (long)cond << endl;
	threadEventsBehaviour->beforeReleasingLockOfAnyReplacedWaiting((long)mutex, true);
	threadEventsBehaviour->onReplacedWaiting((long)cond, true);
//	return pthread_cond_wait(cond, mutex);
}

int cine_cond_timedwait(pthread_cond_t *cond,
		pthread_mutex_t *mutex, const struct timespec *abstime){
	cerr << "waiting time " << (long)mutex << " " << (long)cond << endl;
	threadEventsBehaviour->beforeReleasingLockOfAnyReplacedWaiting((long)mutex, true);
	threadEventsBehaviour->onReplacedTimedWaiting((long)cond, abstime->tv_sec, abstime->tv_nsec, true);
//	return pthread_cond_timedwait(cond, mutex, abstime);
}

int cine_cond_broadcast(pthread_cond_t *cond){
	cerr << "broadcast " << (long)cond << endl;
	threadEventsBehaviour->onSignallingOnObject((long)cond);
//	return pthread_cond_signal(cond);
}

int cine_cond_signal(pthread_cond_t *cond){
	cerr << "signal " << (long)cond << endl;
	threadEventsBehaviour->onBroadcastingOnObject((long)cond);
//	return pthread_cond_broadcast(cond);
}

int cine_sleep(unsigned int t){
	cerr << "sleep" << endl;
	threadEventsBehaviour->onSleep(t, 0);
//	return sleep(t);
}

int cine_yield(){
	cerr << "yield" << endl;
	threadEventsBehaviour->onYield();
//	return pthread_yield();
}

int cine_join(pthread_t thread, void **value_ptr){
	cerr << "join" << endl;
	threadEventsBehaviour->onJoin((long)thread);
//	return pthread_join(thread, value_ptr);
}

//thread registering
void cine_initial_thread(){
	init_thread = pthread_self();
	threadEventsBehaviour->onThreadMainStart((long)init_thread);
    pthread_mutex_lock(&cine_mutex);
	thread_count++;
	cerr << "initial " << pthread_self() << " # " << thread_count << endl;
    pthread_mutex_unlock(&cine_mutex);
}

void cine_new_thread(){
	pthread_t t = pthread_self();
	char n[50];
	pthread_getname_np(t, n, sizeof(n));
	threadEventsBehaviour->onStart((long)t, n);
    pthread_mutex_lock(&cine_mutex);
	thread_count++;
	cerr << "new " << pthread_self() << " # " << thread_count << endl;
    pthread_mutex_unlock(&cine_mutex);
}

//initialisation
void cine_init(){
	if (initialised){
		cine_new_thread();
	} else {
		pthread_mutex_init(&cine_mutex, NULL);
		initializeSimulator(NULL);
		cine_initial_thread();
		initialised = true;
	}
}

int orig_thread_create(pthread_t *thread, const pthread_attr_t *attr,
	                          void *(*start_routine) (void *), void *arg){
	return 1; //fail if this code actually hits
}

int cine_thread_create(pthread_t *thread, const pthread_attr_t *attr,
	                          void *(*start_routine) (void *), void *arg){

    pthread_mutex_lock(&cine_mutex);
	int result = orig_thread_create(thread, attr, start_routine, arg);
	//Hopefully there is no switch before this executes
	threadEventsBehaviour->beforeCreatingThread((long) *thread);
	thread_count++; //increment here since creation may be delayed
	cerr << "before " << *thread << "# " << thread_count << endl;
    pthread_mutex_unlock(&cine_mutex);

	return result;
}

void cine_start_thread(){
	pthread_t thread = pthread_self();
	char n[50];
	pthread_getname_np(thread, n, sizeof(n));

    pthread_mutex_lock(&cine_mutex);
	threadEventsBehaviour->afterCreatingThread(); //lets hope that this does not block
    //append a number onto the end for easy reference
    sprintf(n + strlen(n) - 1 , " %lu", thread);
	cerr << "child " << n << " #" << thread_count << endl;
	threadEventsBehaviour->onStart((long)thread, n);
    pthread_mutex_unlock(&cine_mutex);
}

void cine_timer_entry(int id){
	cerr << "entry " << id << " " <<  pthread_self() << endl;
	methodEventsBehaviour->afterMethodEntry(id);
}

void cine_timer_exit(int id){
	cerr << "exit " << id << " " <<  pthread_self() << endl;
	methodEventsBehaviour->beforeMethodExit(id);
}

void cine_teardown(){
	cerr << "ending all threads and getting results" << endl;
	//TODO: make sure all threads exit
	endSimulator();
	exit(0);
}



void cine_exit_thread(){
	threadEventsBehaviour->onEnd();

    pthread_mutex_lock(&cine_mutex);
	thread_count--;
	cerr << "thread exited " << pthread_self() << " left: " << thread_count << endl;
	pthread_mutex_unlock(&cine_mutex);

	if(pthread_self() == init_thread){
		cine_teardown();
	}

	pthread_exit(0);
//	if (!thread_count){
//		if(end_count == 0){
//			end_count++;
//			pthread_mutex_unlock(&cine_mutex);
//			cine_teardown();
//		}
//	}
	//lets hope ending the simulator does not fail
//    pthread_mutex_unlock(&cine_mutex);
//	pthread_exit(0);
}

void cine_method_registration(char *name, int mid){
	cerr << "registering " << mid << endl;
	eventLogger->registerMethod(name, mid);
}
