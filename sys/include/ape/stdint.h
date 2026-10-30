#ifndef _STDINT_GENERIC_H_
#define _STDINT_GENERIC_H_ 1

#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port, for the gcc driver's programs: C99's macros APE's stdint.h has
 * not had (the constant macros, the limits of intptr_t, ptrdiff_t, sig_atomic_t,
 * wchar_t, wint_t); intmax_t and uintmax_t are the compiler's (__INTMAX_TYPE__:
 * long on amd64, APE's long long the same size), as printf's %jd and C++'s
 * std::intmax_t take them: APE's typedefs are renamed while the header is read.
 * intptr_t and uintptr_t too (__INTPTR_TYPE__: long on amd64 and arm64, as size_t
 * and on every LP64 system; APE's long long the same size, but another type:
 * a uintptr_t * was no size_t *; on 386 long as size_t, APE's int). */
#define intmax_t	__ape_intmax_t
#define uintmax_t	__ape_uintmax_t
#define intptr_t	__ape_intptr_t
#define uintptr_t	__ape_uintptr_t
#endif

/*
 * Default for 32 bit architectures, overriden by
 * /$objtype/include/ape/stdint.h if needed.
 */
#ifndef _STDINT_ARCH_H_
typedef int _intptr_t;
typedef unsigned int _uintptr_t;
#endif

typedef signed char int8_t;	/* C: a signed integer type; plain char is not one (gcc: char* and signed char* differ) */
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
typedef long long intmax_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long long uintmax_t;

typedef int8_t	int_fast8_t;
typedef int16_t	int_fast16_t;
typedef int32_t	int_fast32_t;
typedef int64_t	int_fast64_t;
;
typedef int8_t	int_least8_t;
typedef int16_t	int_least16_t;
typedef int32_t	int_least32_t;
typedef int64_t	int_least64_t;

typedef uint8_t		uint_fast8_t;
typedef uint16_t	uint_fast16_t;
typedef uint32_t	uint_fast32_t;
typedef uint64_t	uint_fast64_t;

typedef uint8_t		uint_least8_t;
typedef uint16_t	uint_least16_t;
typedef uint32_t	uint_least32_t;
typedef uint64_t	uint_least64_t;

typedef _intptr_t intptr_t;
typedef _uintptr_t uintptr_t;

#define INT8_MIN	((int8_t)0x80)
#define INT16_MIN	((int16_t)0x8000)
#define INT32_MIN	((int32_t)0x80000000)
#define INT64_MIN	((int64_t)0x8000000000000000LL)
#define INTMAX_MIN	INT64_MIN

#define UINT8_MIN	0
#define UINT16_MIN	0 
#define UINT32_MIN	0 
#define UINT64_MIN	0
#define UINTMAX_MIN	UINT64_MIN

#define INT_FAST8_MIN	INT8_MIN
#define INT_FAST16_MIN	INT16_MIN
#define INT_FAST32_MIN	INT32_MIN
#define INT_FAST64_MIN	INT64_MIN

#define UINT_FAST8_MIN	UINT8_MIN
#define UINT_FAST16_MIN	UINT16_MIN
#define UINT_FAST32_MIN	UINT32_MIN
#define UINT_FAST64_MIN	UINT64_MIN

#define INT_LEAST8_MIN	INT8_MIN
#define INT_LEAST16_MIN	INT16_MIN
#define INT_LEAST32_MIN	INT32_MIN
#define INT_LEAST64_MIN	INT64_MIN

#define UINT_LEAST8_MIN		UINT8_MIN
#define UINT_LEAST16_MIN	UINT16_MIN
#define UINT_LEAST32_MIN	UINT32_MIN
#define UINT_LEAST64_MIN	UINT64_MIN

#define INT8_MAX	0x7f
#define INT16_MAX	0x7fff
#define INT32_MAX	0x7fffffff
#define INT64_MAX	0x7fffffffffffffffLL
#define INTMAX_MAX	INT64_MAX

#define UINT8_MAX	0xff
#define UINT16_MAX	0xffff
#define UINT32_MAX	0xffffffffL
#define UINT64_MAX	0xffffffffffffffffULL
#define UINTMAX_MAX	UINT64_MAX

#define INT_FAST8_MAX	INT8_MAX
#define INT_FAST16_MAX	INT16_MAX
#define INT_FAST32_MAX	INT32_MAX
#define INT_FAST64_MAX	INT64_MAX

#define UINT_FAST8_MAX	UINT8_MAX
#define UINT_FAST16_MAX	UINT16_MAX
#define UINT_FAST32_MAX	UINT32_MAX
#define UINT_FAST64_MAX	UINT64_MAX

#define INT_LEAST8_MAX	INT8_MAX
#define INT_LEAST16_MAX	INT16_MAX
#define INT_LEAST32_MAX	INT32_MAX
#define INT_LEAST64_MAX	INT64_MAX

#define UINT_LEAST8_MAX		UINT8_MAX
#define UINT_LEAST16_MAX	UINT16_MAX
#define UINT_LEAST32_MAX	UINT32_MAX
#define UINT_LEAST64_MAX	UINT64_MAX

/* 
 * Right now, all of our size_t types are 32 bit, even on
 * 64 bit architectures.
 */
#define SIZE_MIN	UINT32_MIN
#ifdef __SIZE_MAX__	/* GCC port: size_t is unsigned long, 64 bit under gcc on amd64 and arm64; 8c/6c/7c's is 32 */
#define SIZE_MAX	__SIZE_MAX__
#else
#define SIZE_MAX	UINT32_MAX
#endif

#if defined(__GNUC__) || defined(_APE_GNU)
#undef intmax_t
#undef uintmax_t
#undef intptr_t
#undef uintptr_t
typedef __INTPTR_TYPE__ intptr_t;
typedef __UINTPTR_TYPE__ uintptr_t;

typedef __INTMAX_TYPE__ intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;
#undef INTMAX_MIN
#undef INTMAX_MAX
#undef UINTMAX_MIN
#undef UINTMAX_MAX
#define INTMAX_MIN	(-__INTMAX_MAX__-1)
#define INTMAX_MAX	__INTMAX_MAX__
#define UINTMAX_MIN	0
#define UINTMAX_MAX	__UINTMAX_MAX__

#ifndef INT8_C
#define INT8_C(c)	c
#define INT16_C(c)	c
#define INT32_C(c)	c
#define INT64_C(c)	c ## LL
#define UINT8_C(c)	c
#define UINT16_C(c)	c
#define UINT32_C(c)	c ## U
#define UINT64_C(c)	c ## ULL
#define INTMAX_C(c)	__INTMAX_C(c)
#define UINTMAX_C(c)	__UINTMAX_C(c)
#endif

#ifndef INTPTR_MAX
#if __SIZEOF_POINTER__ == 8
#define INTPTR_MIN	INT64_MIN
#define INTPTR_MAX	INT64_MAX
#define UINTPTR_MAX	UINT64_MAX
#else
#define INTPTR_MIN	INT32_MIN
#define INTPTR_MAX	INT32_MAX
#define UINTPTR_MAX	UINT32_MAX
#endif
#endif
#ifndef PTRDIFF_MAX
#define PTRDIFF_MIN	(-__PTRDIFF_MAX__-1)
#define PTRDIFF_MAX	__PTRDIFF_MAX__
#endif
#ifndef SIG_ATOMIC_MAX
#define SIG_ATOMIC_MIN	INT32_MIN
#define SIG_ATOMIC_MAX	INT32_MAX
#endif
#ifndef WCHAR_MAX
#define WCHAR_MIN	__WCHAR_MIN__
#define WCHAR_MAX	__WCHAR_MAX__
#endif
#ifndef WINT_MAX
#define WINT_MIN	__WINT_MIN__
#define WINT_MAX	__WINT_MAX__
#endif
#endif
#endif
