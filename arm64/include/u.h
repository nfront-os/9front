#define nil		((void*)0)

typedef	unsigned short	ushort;
typedef	unsigned char	uchar;
/*
 * GCC port: the gcc definitions are under __GNUC__, 9front's own stay
 * for 7c (kencc does not define __GNUC__). 7c treats long/ulong as 32
 * bits; gcc's LP64 makes unsigned long 64 bits, which would double
 * every ulong* stride: ulong is unsigned int, as amd64/include/u.h.
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
 * GCC port: 7c treats every register as caller-saved, so front's
 * jmp_buf is just sp+pc. Under AAPCS64 x19-x29 and d8-d15 are
 * callee-saved and live across setjmp() in callers above it (libthread
 * switches threads with setjmp/longjmp), so they are saved too:
 * [2..11]=x19-x28 [12]=x29 [13..20]=d8-d15 (see libc/arm64/setjmp.S).
 */
#ifdef __GNUC__
typedef uintptr	jmp_buf[22];
#else
typedef uintptr	jmp_buf[2];
#endif
#define	JMPBUFSP	0
#define	JMPBUFPC	1
#define	JMPBUFDPC	0
typedef unsigned int	mpdigit;	/* for /sys/include/mp.h */
/*
 * GCC port: USED and SET are 7c intrinsics taking any number of
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

union FPdbleword
{
	double	x;
	struct {	/* little endian */
		ulong lo;
		ulong hi;
	};
};

#ifdef __GNUC__
/* GCC port: gcc varargs builtins; the AAPCS64 va_list is a descriptor
   (a struct), not the char* into the argument area 7c's macros walk */
typedef	__builtin_va_list	va_list;
#define va_start(list, start) __builtin_va_start(list, start)
#define va_end(list) __builtin_va_end(list)
#define va_arg(list, mode) __builtin_va_arg(list, mode)
#define va_copy(dst, src) __builtin_va_copy(dst, src)
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
/* GCC port: for the sources that copy a va_list (libc/fmt, libbio) */
#define va_copy(dst, src) ((dst) = (src))
#endif
