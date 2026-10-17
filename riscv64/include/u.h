#define nil		((void*)0)

typedef	unsigned short	ushort;
typedef	unsigned char	uchar;
/*
 * GCC port: the gcc definitions are under __GNUC__, the kencc ones stay
 * for jc (kencc does not define __GNUC__). jc treats long/ulong as 32
 * bits; gcc's LP64 makes unsigned long 64 bits: ulong is unsigned int,
 * as in amd64/include/u.h and arm64/include/u.h.
 */
#ifdef __GNUC__
typedef	unsigned int	ulong;
#else
typedef	unsigned long	ulong;
#endif
typedef	unsigned int	uint;
typedef	signed char	schar;
typedef	long long	vlong;
typedef	unsigned long long uvlong;
typedef vlong	intptr;
typedef uvlong	uintptr;
typedef unsigned long long usize;
typedef	uint		Rune;
typedef union FPdbleword FPdbleword;
/*
 * GCC port: jc treats every register as caller-saved, so its jmp_buf is
 * just sp+pc. Under the RISC-V psABI s0-s11 and fs0-fs11 are
 * callee-saved and live across setjmp() in callers above it (libthread
 * switches threads with setjmp/longjmp), so they are saved too:
 * [2..13]=s0-s11 [14..25]=fs0-fs11 (see libc/riscv64/setjmp.S).
 */
#ifdef __GNUC__
typedef uintptr	jmp_buf[26];
#else
typedef uintptr	jmp_buf[2];
#endif
#define	JMPBUFSP	0
#define	JMPBUFPC	1
#define	JMPBUFDPC	0
typedef unsigned int	mpdigit;	/* for /sys/include/mp.h */
/*
 * GCC port: USED and SET are kencc intrinsics taking any number of
 * arguments and generating no code.
 */
#ifdef __GNUC__
#define USED(...) ((void)(0, ##__VA_ARGS__))
#define SET(...) ((void)0)
#endif
typedef unsigned char u8int;
typedef unsigned short u16int;
typedef unsigned int	u32int;
typedef unsigned long long u64int;
typedef signed char s8int;
typedef signed short s16int;
typedef signed int s32int;
typedef signed long long s64int;

/* FCSR: rounding mode in frm (bits 5-7), accrued exceptions in fflags */
#define	FPINEX	0		/* trap enables: none on risc-v */
#define	FPUNFL	0
#define	FPOVFL	0
#define	FPZDIV	0
#define	FPINVAL	0
#define	FPRNR	(0<<5)		/* rounding modes */
#define	FPRZ	(1<<5)
#define	FPRPINF	(3<<5)
#define	FPRNINF	(2<<5)
#define	FPRMASK	(7<<5)
#define	FPPEXT	0		/* precision */
#define	FPPSGL	0
#define	FPPDBL	0
#define	FPPMASK	0
#define	FPAINEX	(1<<0)		/* accrued exceptions */
#define	FPAUNFL	(1<<1)
#define	FPAOVFL	(1<<2)
#define	FPAZDIV	(1<<3)
#define	FPAINVAL (1<<4)

union FPdbleword
{
	double	x;
	struct {	/* little endian */
		ulong lo;
		ulong hi;
	};
};

#ifdef __GNUC__
/* GCC port: gcc varargs builtins (the psABI passes the first eight
   integer arguments in a0-a7, not on the stack as jc does) */
typedef	__builtin_va_list	va_list;
#define va_start(list, start) __builtin_va_start(list, start)
#define va_end(list) __builtin_va_end(list)
#define va_arg(list, mode) __builtin_va_arg(list, mode)
#define va_copy(dst, src) __builtin_va_copy(dst, src)
#else
/*
 * stdarg - little-endian 64-bit, jc: types narrower than long are
 * widened to long then pushed; types wider than long are pushed on
 * vlong alignment boundaries.
 */
typedef	uintptr	va_list;
#define va_start(list, start) list =\
	(sizeof(start) < 4?\
		(va_list)((long*)&(start)+1):\
		(va_list)(&(start)+1))
#define va_end(list)\
	USED(list)
#define va_arg(list, mode)\
	((sizeof(mode) == 1)?\
		((list += 4), (mode*)list)[-4]:\
	(sizeof(mode) == 2)?\
		((list += 4), (mode*)list)[-2]:\
	(sizeof(mode) == 4)?\
		((list += 4), (mode*)list)[-1]:\
		((mode*)(list = (list + sizeof(mode)+7) & ~7))[-1])
/* GCC port: for the sources that copy a va_list (libc/fmt, libbio) */
#define va_copy(dst, src) ((dst) = (src))
#endif
