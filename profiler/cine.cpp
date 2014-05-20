/*
 * cinelib.cpp
 *
 *  Created on: 24 Apr 2014
 *      Author: giovanni
 */

#include <cstddef>
#include "cine.h"
#include <pthread.h>
#include <iostream>
#include "VTF.h"

//TODO:put this all into a namespace

using namespace std;
using namespace VEX;

long thread_count = 0;
long end_count = 1;

pthread_mutex_t cine_mutex;
//--Mutex methods
//put thread calls into a different library so
//that they can distinguish vex mutexes and
//input mutexes

int orig_pthread_mutex_lock(pthread_mutex_t *mutex){
	return 1;
}

int cine_mutex_lock(pthread_mutex_t *mutex,
    const pthread_mutexattr_t *attr){
	cerr << "waiting" << endl;
	return orig_pthread_mutex_lock(mutex);
}

int orig_pthread_mutex_unlock(pthread_mutex_t *mutex){
	return 1;
}

int cine_mutex_unlock(pthread_mutex_t *mutex,
    const pthread_mutexattr_t *attr){
	cerr << "releasing" << endl;
	return orig_pthread_mutex_unlock(mutex);
}

//initialisation
void cine_init(){
	pthread_mutex_init(&cine_mutex, NULL);
//	initializeSimulator(NULL);
}

int orig_thread_create(pthread_t *thread, const pthread_attr_t *attr,
	                          void *(*start_routine) (void *), void *arg){
	return 0;
}

int cine_thread_create(pthread_t *thread, const pthread_attr_t *attr,
	                          void *(*start_routine) (void *), void *arg){
    orig_pthread_mutex_lock(&cine_mutex);
	int result = orig_thread_create(thread, attr, start_routine, arg);
	//Hopefully there is no switch before this executes
	//TODO: locking?
	if (!result){
//		threadEventsBehaviour->beforeCreatingThread((long) *thread);
		cerr << "before " << *thread << endl;
	}
    orig_pthread_mutex_unlock(&cine_mutex);
	return result;

}

//doesn't work, very efficiently
//TODO:change pthread_t to TID and this will work well
void cine_before_create(pthread_t *t){
	//may not be correct
//	threadEventsBehaviour->beforeCreatingThread((long) *t);
}

void cine_start_thread(){
	char n[50];
	pthread_t thread = pthread_self();
	pthread_getname_np(thread, n, sizeof(n));
//	threadEventsBehaviour->afterCreatingThread();
//	threadEventsBehaviour->onStart((long) thread, n);
    orig_pthread_mutex_lock(&cine_mutex);
	thread_count++;
	cerr << "after " << pthread_self() << " [" <<  n << "]" << "# " << thread_count << endl;
    orig_pthread_mutex_unlock(&cine_mutex);
//	cerr << "-after " << pthread_self() << " [" <<  n << "]" << "# " << thread_count << endl;
}

void cine_initial_thread(){
	pthread_t t = pthread_self();
	//Do you even do this?
//	threadEventsBehaviour->beforeCreatingThread((long) t);
	cine_start_thread();
}

void cine_timer_entry(int id){
	cerr << "entry " << id << " " <<  pthread_self() << endl;
//	methodEventsBehaviour->afterMethodEntry(id);
}

void cine_timer_exit(int id){
	cerr << "exit " << id << " " <<  pthread_self() << endl;
//	methodEventsBehaviour->beforeMethodExit(id);
}

void cine_get_results(){
	cerr << "ending" << endl;
//	endSimulator();
}

void cine_join_thread(){
	cerr << "join" << endl;
	//to be debugged
//	threadEventsBehaviour->onJoin((long) pthread_self());
}

void cine_exit_thread(){
//	threadEventsBehaviour->onEnd();
    orig_pthread_mutex_lock(&cine_mutex);
	thread_count--;
	cerr << "thread exited " << pthread_self() << " left: " << thread_count << endl;
	if (!thread_count){
		end_count--;
		if(end_count == 0){
			cine_get_results();
		}
	}
	//lets hope ending the simulator does not fail
    orig_pthread_mutex_unlock(&cine_mutex);
}

void cine_method_registration(char *name, int mid){
	cerr << "registering " << mid << endl;
//	eventLogger->registerMethod(name, mid);
}
