/*
 * main.c
 *
 *  Created on: 6 Feb 2014
 *      Author: giovanni
 */

#include"Launcher.h"
#include <stdlib.h>

int main(int argc, char* argv[]) {
	//TODO: validate and add program arguments
	string input(argv[1]);
	Launcher(input).launch();
	exit(0);
}
