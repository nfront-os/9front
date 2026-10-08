#ifndef __STDARG
#define __STDARG

#ifdef __GNUC_MINOR__	/* gcc; APE code is compiled -U__GNUC__ (amd64/ape.json) */
/*
 * GCC port: gcc varargs builtins. The System V va_list is a descriptor
 * (an array type), not the char* into the argument area that 6c's
 * macros walk; the arguments of a gcc-compiled caller are not laid
 * out for that. 9front's definitions below stay for 6c.
 */
typedef __builtin_va_list va_list;

#define va_start(list, start) __builtin_va_start(list, start)
#define va_end(list) __builtin_va_end(list)
#define va_arg(list, mode) __builtin_va_arg(list, mode)
#define va_copy(dst, src) __builtin_va_copy(dst, src)
#else
typedef char *va_list;

#define va_start(list, start) \
	(list = (sizeof(start)<8 ? \
		(char *)((long long *)&(start)+1) : \
		(char *)(&(start)+1)))
#define va_end(list)
#define va_arg(list, mode)\
	((sizeof(mode) == 1)?\
		((mode*)(list += 8))[-8]:\
	(sizeof(mode) == 2)?\
		((mode*)(list += 8))[-4]:\
	(sizeof(mode) == 4)?\
		((mode*)(list += 8))[-2]:\
		((mode*)(list += sizeof(mode)))[-1])
#define va_copy(dst, src) \
	((dst) = (src))
#endif

#endif /* __STDARG */
