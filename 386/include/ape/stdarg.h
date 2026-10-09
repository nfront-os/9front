#ifndef __STDARG
#define __STDARG

#ifdef __GNUC_MINOR__	/* gcc; APE code is compiled -U__GNUC__ (386/ape.json) */
/*
 * GCC port: gcc varargs builtins, as amd64/include/ape/stdarg.h (gcc
 * need not keep the named arguments in the argument area that 8c's
 * macros walk). 9front's definitions below stay for 8c.
 */
typedef __builtin_va_list va_list;

#define va_start(list, start) __builtin_va_start(list, start)
#define va_end(list) __builtin_va_end(list)
#define va_arg(list, mode) __builtin_va_arg(list, mode)
#define va_copy(dst, src) __builtin_va_copy(dst, src)
#else
typedef char *va_list;

#define va_start(list, start) list = (sizeof(start)<4 ? (char *)((int *)&(start)+1) : \
(char *)(&(start)+1))
#define va_end(list)
#define va_arg(list, mode) ((mode*)(list += sizeof(mode)))[-1]
#define va_copy(dst, src) ((dst) = (src))
#endif
#endif /* __STDARG */
