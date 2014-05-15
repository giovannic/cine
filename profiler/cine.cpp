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

int orig_thread_create(pthread_t *thread, const pthread_attr_t *attr,
	                          void *(*start_routine) (void *), void *arg){
	return 0;
}

int cine_thread_create(pthread_t *thread, const pthread_attr_t *attr,
	                          void *(*start_routine) (void *), void *arg){
	int result = orig_thread_create(thread, attr, start_routine, arg);
	//Hopefully there is no switch before this executes
	//TODO: locking?
	if (!result){
		threadEventsBehaviour->beforeCreatingThread((long) *thread);
//		cout << "before " << *thread << endl;
	}
	return result;
}

//doesn't work, very efficiently
//TODO:change pthread_t to TID and this will work well
void cine_before_create(pthread_t *t){
	//may not be correct
	threadEventsBehaviour->beforeCreatingThread((long) *t);
	cout << "before " << *t << endl;
}

void cine_start_thread(){
	char n[50];
	pthread_t thread = pthread_self();
	pthread_getname_np(thread, n, sizeof(n));
	threadEventsBehaviour->afterCreatingThread();
	threadEventsBehaviour->onStart((long) thread, n);
	thread_count++;
//	cout << "after " << pthread_self() << " [" <<  n << "]" << endl;
}

void cine_initial_thread(){
	pthread_t t = pthread_self();
	threadEventsBehaviour->beforeCreatingThread((long) t);
	threadEventsBehaviour->afterCreatingThread();
	threadEventsBehaviour->onThreadMainStart((long) pthread_self());
	thread_count++;
	cout << "init thread " << pthread_self() << endl;
}

void cine_timer_entry(int id){
	cout << "entry " << id << endl;
	methodEventsBehaviour->afterMethodEntry(id);
}

void cine_timer_exit(int id){
	cout << "exit " << id << endl;
	methodEventsBehaviour->beforeMethodExit(id);
}

void cine_get_results(){
	endSimulator();
}

void cine_join_thread(){
	threadEventsBehaviour->onJoin((long) pthread_self());
}

void cine_exit_thread(){
	threadEventsBehaviour->onEnd();
	cout << "thread exited" << endl;
	thread_count--;
	if (!thread_count){
		cine_get_results();
	}
}

void cine_method_registration(char *name, int mid){
	eventLogger->registerMethod(name, mid);
}
