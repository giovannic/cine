/*
 * functions.cpp
 *
 *  Created on: 8 Mar 2014
 *      Author: giovanni
 */

#include<vector>
#include "instrumentor.hpp"

using namespace std;

Instrumentor::Instrumentor(BPatch_image *img){
	vector<BPatch_function *> *fs = img->getProcedures(false);
	for( std::vector<BPatch_function *>::const_iterator i = fs->begin(); i != fs->end(); ++i)
	    std::cout << *i << ' ';
}


