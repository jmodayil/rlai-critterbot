/*
 *  lib_critical.h
 *  
 *
 *  Created by Marc G. Bellemare on 30/08/08
 *
 *  Critical code written in assembly
 *
 */
#ifndef LIB_CRITICAL_H
#define LIB_CRITICAL_H

typedef unsigned int crit_sem;

/** Enable interrupts (after disabling them) */
void crit_enable_int();
/** Disable interrupts */
void crit_disable_int();

/** Set a semaphore */
void crit_set_mutex(crit_sem * semaphore);
/** Clears a semaphore */
void crit_clear_mutex(crit_sem * semaphore);

#endif /* LIB_CRITICAL_H */
