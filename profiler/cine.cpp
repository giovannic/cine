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

using namespace std;
using namespace VEX;

int orig_thread_create(pthread_t *thread, const pthread_attr_t *attr,
	                          void *(*start_routine) (void *), void *arg){
	return 0;
}

int cine_thread_create(pthread_t *thread, const pthread_attr_t *attr,
	                          void *(*start_routine) (void *), void *arg){
	int result = orig_thread_create(thread, attr, start_routine, arg);
	//Hopefully there is no switch before this executes
	if (!result){
		char n[50];
		pthread_getname_np(*thread, n, sizeof(n));
		threadEventsBehaviour->onStart((long) result, n);
	}
	return result;
}

int orig_start_thread(void * arg){
	return 1;
}

void cine_start_thread(){
	char n[50];
	pthread_t thread = pthread_self();
	pthread_getname_np(thread, n, sizeof(n));
	cout << "Successful start call from " << n << endl;
	threadEventsBehaviour->onStart((long) thread, n);
}

void cine_initial_thread(){
	threadEventsBehaviour->onThreadMainStart(pthread_self());
}

void cine_exit_thread(){
	threadEventsBehaviour->onEnd();
}

void cine_timer_entry(int id){
	methodEventsBehaviour->afterMethodEntry(id);
}

void cine_timer_exit(int id){
	methodEventsBehaviour->beforeMethodExit(id);
}

