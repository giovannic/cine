/*
 * profiler.h
 *
 *  Created on: 12 Feb 2014
 *      Author: giovanni
 */

#ifndef PROFILER_H_
#define PROFILER_H_

#include "results.hpp"
#include <string>

using namespace std;

class Profiler{
public:
	Profile *profile;
	string *binary;

	virtual ~Profiler(){};

	virtual void start()=0;
	virtual void stop()=0;

	virtual void build_profile()=0;

	void add_binary(string *binary){
		this->binary = binary;
	}
};

#endif /* PROFILER_H_ */
