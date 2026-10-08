#ifndef __STDARG
#define __STDARG

/*
 * GCC port: gcc varargs builtins. The System V va_list is a descriptor
 * (an array type), not the char* into the argument area that 6c's
 * macros walked; the arguments of a gcc-compiled caller are not laid
 * out for that.
 */
typedef __builtin_va_list va_list;

#define va_start(list, start) __builtin_va_start(list, start)
#define va_end(list) __builtin_va_end(list)
#define va_arg(list, mode) __builtin_va_arg(list, mode)
#define va_copy(dst, src) __builtin_va_copy(dst, src)

#endif /* __STDARG */
