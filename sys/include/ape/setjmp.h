#ifndef __SETJMP_H
#define __SETJMP_H
#pragma lib "/$M/lib/ape/libap.a"

#ifdef __x86_64__
/*
 * GCC port: amd64's setjmp also saves the System V callee-saved
 * registers: sp, pc, bp, bx, r12-r15; sigjmp_buf adds two longs
 * (ape/lib/ap/amd64/setjmp.s, notetramp.c)
 */
typedef long long jmp_buf[8];
#ifdef _POSIX_SOURCE
typedef long long sigjmp_buf[10];
#endif
#else
typedef int jmp_buf[10];
#ifdef _POSIX_SOURCE
typedef int sigjmp_buf[10];
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int setjmp(jmp_buf);
extern void longjmp(jmp_buf, int);

#ifdef _POSIX_SOURCE
extern int sigsetjmp(sigjmp_buf, int);
extern void siglongjmp(sigjmp_buf, int);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SETJMP_H */
