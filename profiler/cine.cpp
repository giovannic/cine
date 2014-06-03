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
#include "cine.h"
#include "debug.h"
#include "CineMessage.h"
#include "dyninstRTExport.h"

//TODO:put this all into a namespace

using namespace std;
using namespace VEX;

long thread_count = 0;
long end_count = 0;
bool initialised = false;

const char *pipefile = "/tmp/cineinv";
int inv_fifo;

pthread_t init_thread;
pthread_mutex_t cine_mutex;

//--Mutex methods
//put thread calls into a different library so
//that they can distinguish vex mutexes and
//input mutexes

int cine_mutex_lock(pthread_mutex_t *mutex){
	DEBUG_PRINT(("requesting %lu \n", (long)mutex));
	threadEventsBehaviour->onRequestingLock((long)mutex);
//	return pthread_mutex_lock(mutex);
}

int cine_mutex_unlock(pthread_mutex_t *mutex){
	DEBUG_PRINT(("releasing %lu \n", (long)mutex));
	threadEventsBehaviour->onReleasingLock((long)mutex);
//	return pthread_mutex_unlock(mutex);
}

int cine_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){
	DEBUG_PRINT(("waiting cond %lu %lu \n", (long)mutex, (long)cond));
	threadEventsBehaviour->beforeReleasingLockOfAnyReplacedWaiting((long) cond, (long)mutex, true);
	threadEventsBehaviour->onReplacedWaiting((long)cond, (long)mutex, true);
//	return pthread_cond_wait(cond, mutex);
}

int cine_cond_timedwait(pthread_cond_t *cond,
		pthread_mutex_t *mutex, const struct timespec *abstime){
	DEBUG_PRINT(("waiting timed %lu %lu \n", (long)mutex, (long)cond));
	threadEventsBehaviour->beforeReleasingLockOfAnyReplacedWaiting((long)cond, (long)mutex, true);
	threadEventsBehaviour->onReplacedTimedWaiting((long)cond, (long)mutex, abstime->tv_sec, abstime->tv_nsec, true);
//	return pthread_cond_timedwait(cond, mutex, abstime);
}

int cine_cond_broadcast(pthread_cond_t *cond){
	DEBUG_PRINT(("broadcast %lu \n", (long)cond));
	threadEventsBehaviour->onBroadcastingOnObject((long)cond);
//	return pthread_cond_signal(cond);
}

int cine_cond_signal(pthread_cond_t *cond){
	DEBUG_PRINT(("signal %lu \n", (long)cond));
	threadEventsBehaviour->onSignallingOnObject((long)cond);
//	return pthread_cond_broadcast(cond);
}

int cine_sleep(unsigned int t){
	DEBUG_PRINT(("sleep %d \n", t));
	threadEventsBehaviour->onSleep(t, 0);
//	return sleep(t);
}

int cine_yield(){
	DEBUG_PRINT(("yield \n"));
	threadEventsBehaviour->onYield();
//	return pthread_yield();
}

int cine_join(pthread_t thread, void **value_ptr){
	DEBUG_PRINT(("join \n"));
	threadEventsBehaviour->onJoin((long)thread);
//	return pthread_join(thread, value_ptr);
}

//thread registering
void cine_initial_thread(){
	init_thread = pthread_self();
	threadEventsBehaviour->onThreadMainStart((long)init_thread);
    pthread_mutex_lock(&cine_mutex);
	thread_count++;
	DEBUG_PRINT(("initial %lu # %d\n", init_thread, thread_count));
    pthread_mutex_unlock(&cine_mutex);
}

void cine_new_thread(){
	pthread_t t = pthread_self();
	char n[50];
	pthread_getname_np(t, n, sizeof(n));
	threadEventsBehaviour->onStart((long)t, n);
    pthread_mutex_lock(&cine_mutex);
	thread_count++;
	DEBUG_PRINT(("new %lu # %d\n", t, thread_count));
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

int cine_wrap_thread_create(pthread_t *thread, const pthread_attr_t *attr,
	                          void *(*start_routine) (void *), void *arg){

    pthread_mutex_lock(&cine_mutex);
//	int result = orig_thread_create(thread, attr, start_routine, arg);
	int result = orig_thread_create(thread, attr, start_routine, arg);
	//Hopefully there is no switch before this executes
	threadEventsBehaviour->beforeCreatingThread((long) *thread);
	thread_count++; //increment here since creation may be delayed
	DEBUG_PRINT(("before %lu # %d\n", *thread, thread_count));
    pthread_mutex_unlock(&cine_mutex);

	return result;
}

int cine_thread_create(pthread_t *thread, const pthread_attr_t *attr,
	                          void *(*start_routine) (void *), void *arg){

    pthread_mutex_lock(&cine_mutex);
//	int result = orig_thread_create(thread, attr, start_routine, arg);
	int result = pthread_create(thread, attr, start_routine, arg);
	//Hopefully there is no switch before this executes
	threadEventsBehaviour->beforeCreatingThread((long) *thread);
	thread_count++; //increment here since creation may be delayed
	DEBUG_PRINT(("before %lu # %d\n", *thread, thread_count));
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
	DEBUG_PRINT(("child %s # %d\n", n, thread_count));
	threadEventsBehaviour->onStart((long)thread, n);
    pthread_mutex_unlock(&cine_mutex);
}

void cine_timer_entry(int id){
	DEBUG_PRINT(("entry %d %lu\n", id, pthread_self()));
	//recursive methods do not matter
	methodEventsBehaviour->afterMethodEntry(id);
}

void cine_timer_exit(int id){
	DEBUG_PRINT(("exit %d %lu\n", id, pthread_self()));
	methodEventsBehaviour->beforeMethodExit(id);
}

void cine_timer_invalidate_exit(int id){
	DEBUG_PRINT(("exit %d %lu\n", id, pthread_self()));
	if(methodEventsBehaviour->beforeMethodExit(id) || true){
		InvMsg_t *msg = new InvMsg();
		msg->mid = id;
		if (DYNINSTuserMessage(msg, id) != 0){
			DEBUG_PRINT(("message failed\n"));
		}
	}
}

void cine_teardown(){
	DEBUG_PRINT(("ending all threads and getting results\n"));
	//TODO: make sure all threads exit
	endSimulator();
	close(inv_fifo);
	exit(0);
}



void cine_exit_thread(){
	threadEventsBehaviour->onEnd();

    pthread_mutex_lock(&cine_mutex);
	thread_count--;
	DEBUG_PRINT(("thread_exit %lu # %d\n", pthread_self(), thread_count));
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
	DEBUG_PRINT(("registering %s %d \n", name, mid));
	eventLogger->registerMethod(name, mid);
}

void print_address(void *dest){
	DEBUG_PRINT(("destination %p \n", dest));
}

//		char mid[5];
//		char inv[5];
//		sprintf(mid, "%d", id);
//		if(write(inv_fifo, mid, sizeof(mid)) < 0){
//			cerr << "write failed" << endl;
//		}
//		while(mid != inv){
//			read(inv_fifo, inv, sizeof(inv));
//		}
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/errno.h>
//#include <errno.h>
//#include <fcntl.h>

//		inv_fifo = open(pipefile, O_RDWR);
