#ifndef __STDARG
#define __STDARG

#ifdef __GNUC_MINOR__	/* gcc; APE code is compiled -U__GNUC__ (arm64/ape.json) */
/*
 * GCC port: gcc varargs builtins. The AAPCS64 va_list is a descriptor
 * (a struct), not the char* into the argument area that 7c's macros
 * walk. 9front's definitions below stay for 7c.
 */
typedef __builtin_va_list va_list;

#define va_start(list, start) __builtin_va_start(list, start)
#define va_end(list) __builtin_va_end(list)
#define va_arg(list, mode) __builtin_va_arg(list, mode)
#define va_copy(dst, src) __builtin_va_copy(dst, src)
#else
typedef char *va_list;

#define va_start(list, start) list = (sizeof(start)<8 ? (char *)((long long *)&(start)+1) : \
(char *)(&(start)+1))
#define va_end(list)
#define va_arg(list, mode)\
	((sizeof(mode) == 1)?\
		((mode*)(list += 8))[-8]:\
	(sizeof(mode) == 2)?\
		((mode*)(list += 8))[-4]:\
	(sizeof(mode) == 4)?\
		((mode*)(list += 8))[-2]:\
		((mode*)(list += sizeof(mode)))[-1])
#define va_copy(dst, src) ((dst) = (src))
#endif

#endif /* __STDARG */
