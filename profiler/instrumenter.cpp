/*
 * functions.cpp
 *
 *  Created on: 8 Mar 2014
 *      Author: giovanni
 */

#include <vector>
#include "instrumenter.hpp"
#include "BPatch_function.h"
#include "BPatch_point.h"
using namespace std;

/*

void Instrumenter::insertCall(BPatch_addressSpace *app, BPatch_function *input){
	vector<BPatch_snippet *> timerArgs;
	BPatch_function entryTimer;
	BPatch_function exitTimer;
	//TODO find mid for method
	int mid;

	//arg 0, thread calling
	timerArgs.push_back(&BPatch_threadIndexExpr());
	//arg 1, method id
	timerArgs.push_back(&BPatch_constExpr(mid));

	BPatch_funcCallExpr start(entryTimer, timerArgs);
	BPatch_funcCallExpr stop(exitTimer, timerArgs);

	BPatch_point * entry = input->findPoint(BPatch_entry);
	BPatch_point * exit= input->findPoint(BPatch_exit);

	//store handle for removal
	app->insertSnippet(start, *entry, BPatch_callAfter);
	app->insertSnippet(stop, *exit, BPatch_callBefore);
}
*/

Instrumenter::Instrumenter(BPatch_image *img){
	/*
	 * generate timing code
	 * wrapper(function, args, size)
	 *
	 */
	app = img->getAddressSpace();

    //TODO: check that libcine is there
	if(!app->loadLibrary("libvex.so")){
		cout << "failed" << endl;
	} else {
		cout << "succeeded" << endl;
	}

	vector<BPatch_function *> *fs = img->getProcedures(false);
/*
	for( std::vector<BPatch_function *>::const_iterator i = fs->begin(); i != fs->end(); ++i)
	{
		BPatch_function *f = *i;
		//insertCall(entries, f);

		//img->getAddressSpace()->wrapFunction(*i, timer, (**i).get)
	    cout << (**i).getDemangledName() << endl;

	}
	*/
}
