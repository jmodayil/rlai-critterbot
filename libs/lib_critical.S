/** Critical section for the events driver */

/*.section	.text.events_has_event,"ax",%progbits
	.align	4
	.global	events_has_event
	.type	events_has_event, %function
events_has_event:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
// Load 0 into r0 and the address of events_status into r3
	mov r2, #0
  ldr	r3, =events_status
// Simultaneously read [r3, #0] and set it to 0
// Note: it is mentioned how SWP is slow and LRDEX/STREX should be used,
//  but swp is exactly what I want here; if too slow, add ldr r2, [r3] first
//  to read events_status, and swp only if events_status == 1
  swp r0, r2, [r3]
	bx	lr
*/
  .arm
  .section	.fastrun.crit_disable_int,"ax",%progbits
	.align	4
  .global crit_disable_int
  .type crit_disable_int, %function
crit_disable_int:
// Call the software interrupt with value 1 to disable the IRQ
  swi   0x000001
  bx    lr

  .section	.fastrun.crit_enable_int,"ax",%progbits
  .global crit_enable_int
  .type crit_enable_int, %function
crit_enable_int:
// Call the SWI with value 0 to re-enable the IRQ
  swi   0x000000
  bx    lr

  .section  .fastrun.crit_test_mutex,"ax",%progbits
  .align	4
	.global	crit_test_mutex
	.type	crit_test_mutex, %function
// r0 holds a pointer to a semaphore
crit_test_mutex:
  // Simply read the mutex and return it 
  ldr   r0, [r0]
   bx   lr

.section  .fastrun.crit_get_mutex,"ax",%progbits
  .align	4
	.global	crit_get_mutex
	.type	crit_get_mutex, %function
// r0 holds a pointer to a semaphore
crit_get_mutex:
  mov   r2, #1

// Test-and-set r0 (set it to 1)
mutex_test_and_set: 
  swp   r1, r2, [r0]
// Determine whether the mutex has already been grabbed
  tst   r1, #0x1

// If so, try again
  bne   mutex_test_and_set

// Now that we have successfully set the mutex, return
   bx   lr
  
  .section  .fastrun.crit_release_mutex,"ax",%progbits
  .align	4
	.global	crit_release_mutex
	.type	crit_release_mutex, %function
// r0 holds a pointer to a semaphore
crit_release_mutex:
  mov   r2, #0

// Release the mutex by setting it to 0
  str   r2, [r0, #0] 

// Return
   bx   lr
