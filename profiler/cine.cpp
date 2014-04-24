/*
 * cinelib.cpp
 *
 *  Created on: 24 Apr 2014
 *      Author: giovanni
 */

#include <cstddef>
#include "cine.hpp"
#include "dyninstRTExport.h"
#include <iostream>

using namespace std;

void timer(int type, long long int tid, int mid){

  vex_msg_t msg;
  msg.event = type;
  msg.tid = tid;
  //msg.tid = /*(unsigned long) pthread_self(); */ getpid();
  //msg.mid

  if (0 != DYNINSTuserMessage(&msg, sizeof(vex_msg_t))) {
    cerr << "DYNINSTuserMessage failed\n" << endl;
  }
}
