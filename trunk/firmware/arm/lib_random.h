/*
 * lib_random.h
 *
 * Created by Marc G. Bellemare 
 * Last modified: 27 August 2009
 *
 * Pseudo-random number generator.
 */

#ifndef LIB_RANDOM_H
#define LIB_RANDOM_H

#define PSEUDO_RAND_MAX   2459843
#define PSEUDO_RAND_SEED  5
#define PSEUDO_RAND_MUL   733
#define PSEUDO_RAND_OFFSET 47

/* Returns a number between 0 and PSEUDO_RAND_MAX-1 */
int armrandom();

#endif
