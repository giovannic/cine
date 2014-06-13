/*
 * functions.cpp
 *
 *  Created on: 8 Mar 2014
 *      Author: giovanni
 */

#include<vector>
#include "instrumenter.hpp"

#include "BPatch_image.h"
#include "PatchMgr.h"
//#include "BPatch_function.h"
using namespace std;
using namespace Dyninst::PatchAPI;


Instrumentor::Instrumentor(BPatch_image *img){


	PatchMgrPtr patcher = convert(img);
    vector<PatchFunction *> fs;

    //add all functions
	//vector<BPatch_function *> *fs = img->getProcedures(false);
    AddrSpace::ObjMap objs = patcher.get()->as()->objMap();
    for( AddrSpace::ObjMap::const_iterator i = objs.begin();
    		i != objs.end(); ++i){
    	i->second->funcs(back_inserter(fs));
    }

    Instrumenter *inst = patcher.get()->instrumenter();

	for( vector<PatchFunction *>::const_iterator i = fs.begin();
			i != fs.end(); ++i)
	{
		//img->getAddressSpace()->wrapFunction(*i, timer, (**i).get)
		PatchFunction* f = *i;
	    cout << f->name() << endl;

	    //TODO fix
	    PatchFunction *wrapper;
	    inst->wrapFunction(f, wrapper, string(f->name) + "_original");

	    /*
	     * after entry call
	     * raise user event
	     */


	    /*
	     * before exit call
	     * raise user event
	     */

	}
}

/*
 * typedef void (*BpatchUserEventCallback) (Bpatch_process *proc, void *buf, unsigned int bufsize)
 * this gives us a process, the mutatee must tell us the thread
 * TODO: optimize by directly passing it
 */

//void callMethodEntry(Bpatch_process *proc, void *buf, unsigned int bufsize){
//	cout << "entered" << endl;
//}

