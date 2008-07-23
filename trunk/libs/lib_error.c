/*
 *  lib_error.c
 *  
 *
 *  Created by Marc G. Bellemare on 21/07/08.
 *
 *  Error-reporting driver.
 */

#include "lib_error.h"

void error_set(unsigned int errflag)
{
  error_reg |= errflag;
}

void error_clear(unsigned int errflag)
{
  error_reg &= ~errflag;
}

unsigned int error_get()
{
  return error_reg;
}

void error_disp(void) {
  int i;

  for(i = 0; i <= ERR_MAXERR; i++)
    if(error_reg & (1 << i))
      ledctl_setcolor(i, 1023, 0, 0);
}
