/*
 * functions.cpp
 *
 *  Created on: 8 Mar 2014
 *      Author: giovanni
 */

#include<vector>
#include "instrumenter.hpp"
#include "BPatch_function.h"
#include "BPatch_point.h"
using namespace std;


void Instrumenter::insertCall(BPatch_addressSpace *app, BPatch_function *input){
	vector<BPatch_snippet *> timerArgs;
	BPatch_snippet timer;

	//arg 0, thread calling
	timerArgs.push_back(&BPatch_threadIndexExpr());
	//arg 1, function call
	//have to workout arguments
	timerArgs.push_back(&BPatch_funcCallExpr(input));

	BPatch_point * entry = input->findPoint(BPatch_entry);

	//store handle for removal
	app->insertSnippet(timer, *entry, BPatch_callAfter);

}

Instrumenter::Instrumenter(BPatch_image *img){
	/*
	 * generate timing code
	 * wrapper(function, args, size)
	 *
	 */
	app = img->getAddressSpace();

	vector<BPatch_function *> *fs = img->getProcedures(false);
	for( std::vector<BPatch_function *>::const_iterator i = fs->begin(); i != fs->end(); ++i)
	{
		BPatch_function *f = *i;
		vector<BPatch_point *> *entries = f->findPoint(BPatch_entry);
		vector<BPatch_point *> *exits = f->findPoint(BPatch_exit);
		insertCall(entries, afterEntry);
		//img->getAddressSpace()->wrapFunction(*i, timer, (**i).get)
	    cout << (**i).getDemangledName() << endl;

	}
}

