#ifndef LIB_THERMO_H
#define LIB_THERMO_H

#include "AT91SAM7S256.h"
#include "lib_AT91SAM7S256.h"
#include "armio.h"
#include "compiler.h"

#define THERMO_NUM_THERMS 8
#define THERMO_VAL_SIZE 3
#define THERMO_COMMAND_SIZE 2

extern event_s thermo_event_s;
void thermo_twi_init(void);
void thermo_command_init(void);
int thermo_get_val(int i);
int thermo_init(void);
int thermo_event(void);
ARM_CODE RAMFUNC void thermo_isr(void);

#endif // LIB_THERMO_H
