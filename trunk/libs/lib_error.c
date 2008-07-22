/*
 *  lib_error.c
 *  
 *
 *  Created by Marc G. Bellemare on 21/07/08.
 *
 *  Error-reporting driver.
 */

unsigned int error_reg;

void error_get(unsigned int errflag)
{
  error_reg |= errflag;
}

unsigned error_set()
{
  return error_reg;
}

