/*
 * lib_random.c
 *
 * Created by Marc G. Bellemare 
 * Last modified: 27 August 2009
 *
 * Pseudo-random number generator.
 */

#include "lib_random.h"

int randomValue = PSEUDO_RAND_SEED;

int armrandom() {
  randomValue = (randomValue * PSEUDO_RAND_MUL + PSEUDO_RAND_OFFSET) %
    PSEUDO_RAND_MAX;

  return randomValue;
}
