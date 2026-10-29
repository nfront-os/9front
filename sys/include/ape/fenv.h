/*
 * APE (GCC port): C99's <fenv.h>; the functions are musl's
 * (libap), the types and values those of musl's bits/fenv.h
 * for the machine (x86, riscv64).
 */
#ifndef __APE_FENV
#define __APE_FENV

#ifdef __riscv
/* fcsr's flags and rounding modes */
#define FE_INVALID	16
#define FE_DIVBYZERO	8
#define FE_OVERFLOW	4
#define FE_UNDERFLOW	2
#define FE_INEXACT	1

#define FE_ALL_EXCEPT	31

#define FE_TONEAREST	0
#define FE_DOWNWARD	2
#define FE_UPWARD	3
#define FE_TOWARDZERO	1

typedef unsigned int fexcept_t;
typedef unsigned int fenv_t;
#else
#define FE_INVALID	1
#define __FE_DENORM	2
#define FE_DIVBYZERO	4
#define FE_OVERFLOW	8
#define FE_UNDERFLOW	16
#define FE_INEXACT	32

#define FE_ALL_EXCEPT	63

#define FE_TONEAREST	0
#define FE_DOWNWARD	0x400
#define FE_UPWARD	0x800
#define FE_TOWARDZERO	0xc00

typedef unsigned short fexcept_t;

/* the x87 environment (fnstenv), on amd64 with SSE's mxcsr */
typedef struct {
	unsigned short __control_word;
	unsigned short __unused1;
	unsigned short __status_word;
	unsigned short __unused2;
	unsigned short __tags;
	unsigned short __unused3;
	unsigned int __eip;
	unsigned short __cs_selector;
	unsigned int __opcode:11;
	unsigned int __unused4:5;
	unsigned int __data_offset;
	unsigned short __data_selector;
	unsigned short __unused5;
#ifdef __x86_64__
	unsigned int __mxcsr;
#endif
} fenv_t;
#endif

#define FE_DFL_ENV	((const fenv_t *) -1)

int feclearexcept(int);
int fegetexceptflag(fexcept_t *, int);
int feraiseexcept(int);
int fesetexceptflag(const fexcept_t *, int);
int fetestexcept(int);

int fegetround(void);
int fesetround(int);

int fegetenv(fenv_t *);
int feholdexcept(fenv_t *);
int fesetenv(const fenv_t *);
int feupdateenv(const fenv_t *);

#endif
