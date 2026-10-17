#ifndef __STDARG
#define __STDARG

#ifdef __GNUC_MINOR__	/* gcc; APE code is compiled -U__GNUC__ (riscv64/ape.json) */
/*
 * GCC port: gcc varargs builtins; the psABI passes the first eight
 * integer arguments in registers. jc's definitions below (from Geoff
 * Collyer's riscv64 port) stay for jc.
 */
typedef __builtin_va_list va_list;

#define va_start(list, start) __builtin_va_start(list, start)
#define va_end(list) __builtin_va_end(list)
#define va_arg(list, mode) __builtin_va_arg(list, mode)
#define va_copy(dst, src) __builtin_va_copy(dst, src)
#else
/*
 * jc: types narrower than long are widened to long then pushed;
 * types wider than long are pushed on vlong alignment boundaries.
 */
typedef	unsigned long long va_list;

#define va_start(list, start) list =\
	(sizeof(start) < 4?\
		(va_list)((long*)&(start)+1):\
		(va_list)(&(start)+1))
#define va_end(list)
#define va_arg(list, mode)\
	((sizeof(mode) == 1)?\
		((list += 4), (mode*)list)[-4]:\
	(sizeof(mode) == 2)?\
		((list += 4), (mode*)list)[-2]:\
	(sizeof(mode) == 4)?\
		((list += 4), (mode*)list)[-1]:\
		((mode*)(list = (list + sizeof(mode)+7) & ~7))[-1])
#define va_copy(dst, src) ((dst) = (src))
#endif

#endif /* __STDARG */
