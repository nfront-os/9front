/*
 * APE (GCC port): <alloca.h>, as glibc and musl have it: gcc's builtin.
 */
#ifndef __APE_ALLOCA_H
#define __APE_ALLOCA_H
#include <stddef.h>
#undef alloca
#define alloca(n)	__builtin_alloca(n)
#endif
