/*
 * launcher_test.c
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "launcher.hpp"
#include "OProfile.hpp"
#include <unistd.h>
#include <iostream>
#include <sstream>

using namespace std;

TEST_CASE( "Loader", "[loader]" ) {

	char cwd[1024];
	REQUIRE (getcwd(cwd, sizeof(cwd)) != NULL);

	//basic
	stringstream path(cwd);
	path << "testinputs/threads";
	string threadfile(path.str());
	//REQUIRE( Launcher(&path).launch() );

	//arguments
	string echo = "/bin/echo";
	string args = "hello world";

	Launcher l = Launcher(&echo);
	l.add_arguments(&args);

	REQUIRE( l.launch() );

	//profiler
	//l = Launcher(&threadfile);
	//string opdir = "/usr/local/bin";
	//REQUIRE( l.launch() );
}
