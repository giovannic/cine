/*
 * OProfile.hpp
 *
 *  Created on: 12 Feb 2014
 *      Author: giovanni
 */

#ifndef OPROFILE_HPP_
#define OPROFILE_HPP_

#include "profiler.hpp"

class OProfile {
	void control(string arg);
	string opctl;
	pid_t pid;

public:
	string *dir;

	OProfile(string *dir);
	virtual ~OProfile();

	void start(pid_t pid);
	void stop();

	void build_profile();
};

#endif /* OPROFILE_HPP_ */
