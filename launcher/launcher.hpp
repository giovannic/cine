/*
 * launcher.hpp
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */

#ifndef LAUNCHER_HPP_
#define LAUNCHER_HPP_
#include <vector>
#include "OProfile.hpp"

using namespace std;

class Launcher{
public:
	string *input;
	vector<string> *args;
	OProfile *profiler;

	Launcher(string *input);
	bool launch();
	void add_arguments(string *args);
};


#endif /* LAUNCHER_HPP_ */
