/*
 *  main.c
 *  
 *
 *  Created by Marc G. Bellemare on 20/07/08
 *
 *  This main.c file implements the main function for the middle board 
 *  AT9SAM7S256 chip.
 */

#include "lib_events.h"
#include "armio.h"

int main()
{

  events_init();

  armprintf("critterprompt> ");
  while (1)
  {
    if (events_has_event())
      events_do();
  }
}
