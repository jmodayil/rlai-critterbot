/*
 *
 * os.h
 *
 * Created by Marc G. Bellemare on 1/04/08
 *
 * Define copmiler-specific flags
 */
#ifndef OS_H
#define OS_H

#ifdef __GNUC__
// Necessary to avoid duplicate definitions, behavior unknown
#define __inline static inline
#ifndef NO_RAMFUNC
#define RAMFUNC __attribute__ ((long_call, section (".fastrun")))
#else
#define RAMFUNC
#endif
#define ARM_CODE
#else
#error Unsupported compiler! Contact the authors.
#endif

#endif /* OS_H */

