/** Critical section for the events driver */

.section	.text.events_has_event,"ax",%progbits
	.align	2
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
