/*
 * debug.h
 *
 *  Created on: 3 Jun 2014
 *      Author: giovanni
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef CINEDEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

#endif /* DEBUG_H_ */
