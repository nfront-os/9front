#define nil		((void*)0)
typedef	unsigned short	ushort;
typedef	unsigned char	uchar;
/*
 * GCC port: the gcc definitions are under __GNUC__, 9front's own stay
 * for 6c on Plan 9 (kencc does not define __GNUC__).
 *
 * Plan 9's 6c treats `long`/`ulong` as 32 bits even on amd64
 * (uintptr/vlong/u64int are the explicit 64-bit types) -- the whole tree
 * assumes sizeof(ulong)==4 (e.g. ulong* array indexing, on-disk formats).
 * GCC's LP64 model makes plain `unsigned long` 64 bits on amd64, which
 * silently doubles every ulong* stride; use unsigned int instead to match
 * 6c's actual size, not the spelling.
 */
#ifdef __GNUC__
typedef unsigned int	ulong;
#else
typedef unsigned long	ulong;
#endif
typedef unsigned int	uint;
typedef signed char	schar;
typedef	long long	vlong;
typedef	unsigned long long uvlong;
typedef long long	intptr;
typedef unsigned long long uintptr;
typedef unsigned long long usize;
typedef	uint		Rune;
typedef union FPdbleword FPdbleword;
/*
 * GCC port: 6c treats every register as caller-saved, so front's
 * jmp_buf is just sp+pc. Under System V %rbx/%rbp/%r12-%r15 are
 * callee-saved and live across setjmp() in callers above it (libthread
 * switches threads with setjmp/longjmp), so they are saved too:
 * [2]=bp [3]=bx [4]=r12 [5]=r13 [6]=r14 [7]=r15 (see libc/amd64/setjmp.s).
 */
#ifdef __GNUC__
typedef uintptr		jmp_buf[8];
#else
typedef uintptr		jmp_buf[2];
#endif
#define	JMPBUFSP	0
#define	JMPBUFPC	1
#define	JMPBUFDPC	0
typedef unsigned int	mpdigit;	/* for /sys/include/mp.h */
typedef unsigned char	u8int;
typedef unsigned short	u16int;

/*
 * gcc port: USED and SET are 8c/6c intrinsics taking any number of
 * arguments and generating no code (they only silence "used"/"set and
 * not used" diagnostics).
 */
#ifdef __GNUC__
#define USED(...) ((void)(0, ##__VA_ARGS__))
#define SET(...) ((void)0)
#endif
typedef unsigned int	u32int;
typedef unsigned long long u64int;
typedef signed char s8int;
typedef signed short s16int;
typedef signed int s32int;
typedef signed long long s64int;

/* MXCSR */
/* fcr */
#define	FPFTZ	(1<<15)	/* amd64 */
#define	FPINEX	(1<<12)
#define	FPUNFL	(1<<11)
#define	FPOVFL	(1<<10)
#define	FPZDIV	(1<<9)
#define	FPDNRM	(1<<8)	/* amd64 */
#define	FPINVAL	(1<<7)
#define	FPDAZ	(1<<6)	/* amd64 */
#define	FPRNR	(0<<13)
#define	FPRZ	(3<<13)
#define	FPRPINF	(2<<13)
#define	FPRNINF	(1<<13)
#define	FPRMASK	(3<<13)
#define	FPPEXT	0
#define	FPPSGL	0
#define	FPPDBL	0
#define	FPPMASK	0
/* fsr */
#define	FPAINEX	(1<<5)
#define	FPAUNFL	(1<<4)
#define	FPAOVFL	(1<<3)
#define	FPAZDIV	(1<<2)
#define	FPADNRM	(1<<1)	/* not in plan 9 */
#define	FPAINVAL	(1<<0)
union FPdbleword
{
	double	x;
	struct {	/* little endian */
		uint lo;
		uint hi;
	};
};

#ifdef __GNUC__
#ifndef _VA_LIST_DEFINED
typedef	__builtin_va_list	va_list;
#define _VA_LIST_DEFINED
#endif
/* GCC port: use gcc varargs builtins; __builtin_va_list is an array type
   here, so the raw-pointer Plan 9 macros cannot assign/increment it */
#define va_start(list, start) __builtin_va_start(list, start)
#define va_end(list) __builtin_va_end(list)
#define va_arg(list, mode) __builtin_va_arg(list, mode)
#else
typedef	char*	va_list;
#define va_start(list, start) list =\
	(sizeof(start) < 8?\
		(char*)((vlong*)&(start)+1):\
		(char*)(&(start)+1))
#define va_end(list)\
	USED(list)
#define va_arg(list, mode)\
	((sizeof(mode) == 1)?\
		((list += 8), (mode*)list)[-8]:\
	(sizeof(mode) == 2)?\
		((list += 8), (mode*)list)[-4]:\
	(sizeof(mode) == 4)?\
		((list += 8), (mode*)list)[-2]:\
		((list += sizeof(mode)), (mode*)list)[-1])
#endif
