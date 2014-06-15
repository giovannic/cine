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

Instrumenter *callbackInst;

AsyncListener::AsyncListener(Instrumenter *inst, BPatch *bpatch) : Listener(inst) {
	this->bpatch = bpatch;
}
AsyncListener::~AsyncListener() {
}

void invalidate(BPatch_process *proc,
		void *buf, unsigned int bufsize){
	InvMsg_t *inv = (InvMsg_t *)buf;
	callbackInst->removeTime(inv->mid);
}


bool AsyncListener::listen() {
	callbackInst = inst;
	return bpatch->registerUserEventCallback(invalidate);
}
