/*
 * functions.cpp
 *
 *  Created on: 8 Mar 2014
 *      Author: giovanni
 */

#include<vector>
#include "instrumentor.hpp"
#include "BPatch_function.h"
using namespace std;



Instrumentor::Instrumentor(BPatch_image *img){
	vector<BPatch_function *> *fs = img->getProcedures(false);
	for( std::vector<BPatch_function *>::const_iterator i = fs->begin(); i != fs->end(); ++i)
	{
		//img->getAddressSpace()->wrapFunction(*i, timer, (**i).get)
	    cout << (**i).getDemangledName() << endl;
	}
}


