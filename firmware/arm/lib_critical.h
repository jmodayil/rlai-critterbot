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

typedef volatile unsigned int crit_sem;

/** Enable interrupts (after disabling them) */
void crit_enable_int();
/** Disable interrupts */
void crit_disable_int();

/**
  * Tests whether a semaphore is being used. This routine is provided for
  *  interrupts to avoid blocking in one.
  * Returns non-zero if the semaphore is held by another process.
  */
int crit_test_mutex(crit_sem * semaphore);
/** 
  * Requests a semaphore (and block until the semaphore is free).
  *  Two threads wishing to synchronize or write to the same data should 
  *  use a volatile crit_sem and call crit_get_mutex on this semaphore
  *  before accessing the data.
  */
void crit_get_mutex(crit_sem * semaphore);
/** 
  * Releases a semaphore. For simplicity, the semaphore does not contain 
  *  information as to whether the same 'thread' who got the semaphore is
  *  trying to release it. In other words, proper use of semaphores requires
  *  that the semaphore be released only after getting it.
  */
void crit_release_mutex(crit_sem * semaphore);

#endif /* LIB_CRITICAL_H */
