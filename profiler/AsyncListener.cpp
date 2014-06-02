/*
 * Listener.cpp
 *
 *  Created on: 2 Jun 2014
 *      Author: giovanni
 */

#include "AsyncListener.h"
#include "CineMessage.h"

#include "PCProcess.h"
#include "Event.h"

using namespace std;
using namespace Dyninst::ProcControlAPI;

AsyncListener::AsyncListener(BPatch *bpatch) : Listener() {
	this->bpatch = bpatch;
}
AsyncListener::~AsyncListener() {
}

void invalidate(BPatch_process *proc,
		void *buf, unsigned int bufsize){
	InvMsg_t *inv = (InvMsg_t *)buf;
	cout << "about to invalidate" << inv->mid << endl;
}


bool AsyncListener::listen() {
	return bpatch->registerUserEventCallback(invalidate);
}
