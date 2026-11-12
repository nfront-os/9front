/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: OpenBSD kernel API, sys/stdarg.h (gcc's builtins) */
#ifndef _OBSD_SYS_STDARG_H
#define _OBSD_SYS_STDARG_H
typedef __builtin_va_list	va_list;
#define va_start(ap, last)	__builtin_va_start(ap, last)
#define va_arg(ap, t)		__builtin_va_arg(ap, t)
#define va_copy(d, s)		__builtin_va_copy(d, s)
#define va_end(ap)		__builtin_va_end(ap)
#endif
