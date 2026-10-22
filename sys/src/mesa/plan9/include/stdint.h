/* SPDX-License-Identifier: MIT */
/*
 * APE's int8_t is plain char; C wants signed char (a different type
 * to gcc: int8_t* and signed char* do not mix).
 */
#ifndef _MESA_PLAN9_STDINT_H
#define _MESA_PLAN9_STDINT_H

#define int8_t __ape_int8_t
#include_next <stdint.h>
#undef int8_t
typedef signed char int8_t;

#endif
