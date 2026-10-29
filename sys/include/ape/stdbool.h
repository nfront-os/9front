#ifndef __STDBOOL_H__
#define __STDBOOL_H__
/* GCC port: APE's is from before C99 ("typedef char _Bool", an error now
 * _Bool is a keyword): for its own compilers only; gcc's as its own stdbool.h
 * (in C23 bool, true and false are keywords, in C++ built in) */
#ifdef __GNUC_MINOR__
#ifndef __cplusplus
#if !defined __STDC_VERSION__ || __STDC_VERSION__ < 202311L
#define bool	_Bool
#define true	1
#define false	0
#endif
#endif
#define __bool_true_false_are_defined	1
#else
/* Strictly speaking, this should be a built-in type. */
typedef char _Bool;

#define bool _Bool
#define true 1
#define false 0
#define __bool_true_false_are_defined 1
#endif
#endif
