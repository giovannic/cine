/*
 * instrumentor.hpp
 *
 *  Created on: 8 Mar 2014
 *      Author: giovanni
 */

#ifndef INSTRUMENTOR_HPP_
#define INSTRUMENTOR_HPP_

#include "BPatch.h"
#include "BPatch_image.h"

typedef void* position_t;

typedef enum {
	method_entry = 0,
	method_exit = 1
} vex_event;

typedef struct {
	position_t addr;
	unsigned long long int tid;
} vex_message;

class Instrumentor {
public:
	Instrumentor(BPatch_image *img);
};

#endif /* INSTRUMENTOR_HPP_ */
