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

TEST_CASE( "Loader", "[loader]" ) {

	//basic
	string path = "/home/giovanni/project/whatif/ctests/testinputs/threads";
	//REQUIRE( Launcher(&path).launch() );

	//arguments
	string echo = "/bin/echo";
	string args = "hello world";

	Launcher l = Launcher(&echo);
	l.add_arguments(&args);

	REQUIRE( l.launch() );

	//profiler
	l = Launcher(&path);
	string opdir = "/usr/local/bin";
	l.profiler = new OProfile(&opdir);
	REQUIRE( l.launch() );
}
