#ifndef __FLOAT
#define __FLOAT
/* IEEE, default rounding */

#define FLT_ROUNDS	1
#define FLT_RADIX	2

#define FLT_DIG		6
#define FLT_EPSILON	1.19209290e-07
#define FLT_MANT_DIG	24
#define FLT_MAX		3.40282347e+38
#define FLT_MAX_10_EXP	38
#define FLT_MAX_EXP	128
#define FLT_MIN		1.17549435e-38
#define FLT_MIN_10_EXP	-37
#define FLT_MIN_EXP	-125

#define DBL_DIG		15
#define DBL_EPSILON	2.2204460492503131e-16
#define DBL_MANT_DIG	53
#define DBL_MAX		1.797693134862315708145e+308
#define DBL_MAX_10_EXP	308
#define DBL_MAX_EXP	1024
#define DBL_MIN		2.225073858507201383090233e-308
#define DBL_MIN_10_EXP	-307
#define DBL_MIN_EXP	-1021
#define LDBL_MANT_DIG	DBL_MANT_DIG
#define LDBL_EPSILON	DBL_EPSILON
#define LDBL_DIG	DBL_DIG
#define LDBL_MIN_EXP	DBL_MIN_EXP
#define LDBL_MIN	DBL_MIN
#define LDBL_MIN_10_EXP	DBL_MIN_10_EXP
#define LDBL_MAX_EXP	DBL_MAX_EXP
#define LDBL_MAX	DBL_MAX
#define LDBL_MAX_10_EXP	DBL_MAX_10_EXP

typedef 	union FPdbleword FPdbleword;
union FPdbleword
{
	double	x;
	struct {	/* little endian */
		long lo;
		long hi;
	};
};

#ifdef _RESEARCH_SOURCE
/* define stuff needed for floating conversion */
#define IEEE_8087	1
#define Sudden_Underflow 1
#endif
#ifdef _PLAN9_SOURCE
/* FPCR (control) */
#define	FPINEX		(1<<12)
#define	FPUNFL		(1<<11)
#define	FPOVFL		(1<<10)
#define	FPZDIV		(1<<9)
#define	FPINVAL		(1<<8)

#define	FPRNR		(0<<22)
#define	FPRPINF		(1<<22)
#define	FPRNINF		(2<<22)
#define	FPRZ		(3<<22)

#define	FPRMASK		(3<<22)

/* FPSR (status) */
#define	FPPEXT	0
#define	FPPSGL	0
#define	FPPDBL	0
#define	FPPMASK	0
#define	FPAINEX		(1<<4)
#define	FPAUNFL		(1<<3)
#define	FPAOVFL		(1<<2)
#define	FPAZDIV		(1<<1)
#define	FPAINVAL	(1<<0)
#endif
#if defined(__GNUC__) || defined(_APE_GNU)
/*
 * GCC port, for the gcc driver's programs: <float.h> with the compiler's values. APE's (/$objtype/
 * include/ape/float.h) describes kencc's types: its long double is a
 * double, under gcc it is the x87's 80-bit type, and LDBL_MAX was a
 * double constant (printf's %Lf read garbage). C99's and C11's macros
 * APE lacks besides. APE's header stays for the rest (FPdbleword, the
 * FP* bits of _PLAN9_SOURCE); its limits are replaced, as gcc's own
 * float.h defines them.
 */


#undef FLT_RADIX
#undef FLT_ROUNDS
#undef FLT_DIG
#undef FLT_EPSILON
#undef FLT_MANT_DIG
#undef FLT_MAX
#undef FLT_MAX_10_EXP
#undef FLT_MAX_EXP
#undef FLT_MIN
#undef FLT_MIN_10_EXP
#undef FLT_MIN_EXP
#undef DBL_DIG
#undef DBL_EPSILON
#undef DBL_MANT_DIG
#undef DBL_MAX
#undef DBL_MAX_10_EXP
#undef DBL_MAX_EXP
#undef DBL_MIN
#undef DBL_MIN_10_EXP
#undef DBL_MIN_EXP
#undef LDBL_DIG
#undef LDBL_EPSILON
#undef LDBL_MANT_DIG
#undef LDBL_MAX
#undef LDBL_MAX_10_EXP
#undef LDBL_MAX_EXP
#undef LDBL_MIN
#undef LDBL_MIN_10_EXP
#undef LDBL_MIN_EXP

#define FLT_RADIX	__FLT_RADIX__
/* the current rounding mode (fesetround) */
#define FLT_ROUNDS	(__builtin_flt_rounds())
#define FLT_EVAL_METHOD	__FLT_EVAL_METHOD__
#define DECIMAL_DIG	__DECIMAL_DIG__

#define FLT_MANT_DIG	__FLT_MANT_DIG__
#define FLT_DIG		__FLT_DIG__
#define FLT_EPSILON	__FLT_EPSILON__
#define FLT_MIN		__FLT_MIN__
#define FLT_MAX		__FLT_MAX__
#define FLT_MIN_EXP	__FLT_MIN_EXP__
#define FLT_MAX_EXP	__FLT_MAX_EXP__
#define FLT_MIN_10_EXP	__FLT_MIN_10_EXP__
#define FLT_MAX_10_EXP	__FLT_MAX_10_EXP__

#define DBL_MANT_DIG	__DBL_MANT_DIG__
#define DBL_DIG		__DBL_DIG__
#define DBL_EPSILON	__DBL_EPSILON__
#define DBL_MIN		__DBL_MIN__
#define DBL_MAX		__DBL_MAX__
#define DBL_MIN_EXP	__DBL_MIN_EXP__
#define DBL_MAX_EXP	__DBL_MAX_EXP__
#define DBL_MIN_10_EXP	__DBL_MIN_10_EXP__
#define DBL_MAX_10_EXP	__DBL_MAX_10_EXP__

#define LDBL_MANT_DIG	__LDBL_MANT_DIG__
#define LDBL_DIG	__LDBL_DIG__
#define LDBL_EPSILON	__LDBL_EPSILON__
#define LDBL_MIN	__LDBL_MIN__
#define LDBL_MAX	__LDBL_MAX__
#define LDBL_MIN_EXP	__LDBL_MIN_EXP__
#define LDBL_MAX_EXP	__LDBL_MAX_EXP__
#define LDBL_MIN_10_EXP	__LDBL_MIN_10_EXP__
#define LDBL_MAX_10_EXP	__LDBL_MAX_10_EXP__

/* C11 */
#if (defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L) \
	|| (defined __cplusplus && __cplusplus >= 201103L)
#define FLT_DECIMAL_DIG		__FLT_DECIMAL_DIG__
#define DBL_DECIMAL_DIG		__DBL_DECIMAL_DIG__
#define LDBL_DECIMAL_DIG	__LDBL_DECIMAL_DIG__
#define FLT_HAS_SUBNORM		__FLT_HAS_DENORM__
#define DBL_HAS_SUBNORM		__DBL_HAS_DENORM__
#define LDBL_HAS_SUBNORM	__LDBL_HAS_DENORM__
#define FLT_TRUE_MIN		__FLT_DENORM_MIN__
#define DBL_TRUE_MIN		__DBL_DENORM_MIN__
#define LDBL_TRUE_MIN		__LDBL_DENORM_MIN__
#endif
#endif
#endif /* __FLOAT */
