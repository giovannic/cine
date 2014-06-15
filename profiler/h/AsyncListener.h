/*
 * Listener.h
 *
 *  Created on: 2 Jun 2014
 *      Author: giovanni
 */

#ifndef ALISTENER_H_
#define ALISTENER_H_

#include "Listener.h"
#include "BPatch.h"
using namespace Dyninst;

class AsyncListener : public Listener {
public:
	AsyncListener(Instrumenter *inst, BPatch *bpatch);
	virtual ~AsyncListener();
	bool listen();
private:
	BPatch *bpatch;
};


#endif /* ALISTENER_H_ */
