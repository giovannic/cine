/*
 * main.c
 *
 *  Created on: 6 Feb 2014
 *      Author: giovanni
 */

#include"launcher.hpp"
#include <stdlib.h>

int main(int argc, char* argv[]) {
	//TODO: validate arguments
	Launcher(argv[1]).launch();
	exit(0);
}
