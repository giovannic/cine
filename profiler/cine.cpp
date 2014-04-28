/*
 * cinelib.cpp
 *
 *  Created on: 24 Apr 2014
 *      Author: giovanni
 */

#include <cstddef>
#include "cine.hpp"
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
		cout << n << " with id " << (long) thread << endl;
	}
	return result;
}

/*
void cine_thread_create(){
	pthread_t self = pthread_self();
	char n[100];
	pthread_getname_np(self, n, sizeof(n));
	cout << "thread " << (unsigned long) self << " " << n << " created" << endl;
}
*/
