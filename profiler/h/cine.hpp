/*
 * cinelib.hpp
 *
 *  Created on: 24 Apr 2014
 *      Author: giovanni
 */

#ifndef CINELIB_HPP_
#define CINELIB_HPP_

#define ENTRY 0
#define EXIT 1

typedef struct {
  int event;
  unsigned long long int tid;
  int mid;
} vex_msg_t;

#endif /* CINELIB_HPP_ */
