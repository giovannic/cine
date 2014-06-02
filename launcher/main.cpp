/*
 * main.c
 *
 *  Created on: 6 Feb 2014
 *      Author: giovanni
 */

#include"StaticLauncher.h"
#include"DynamicLauncher.h"
#include <stdlib.h>

int main(int argc, char* argv[]) {
	//TODO: validate and add program arguments
	string input(argv[1]);
//	DynamicLauncher(input).launch();
	DynamicLauncher(input).launch();
	exit(0);
}
