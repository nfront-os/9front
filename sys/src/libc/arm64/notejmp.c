#include <u.h>
#include <libc.h>
#include <ureg.h>

void
notejmp(void *vr, jmp_buf j, int ret)
{
	struct Ureg *r = vr;

	r->r0 = ret;
	if(ret == 0)
		r->r0 = 1;
	r->pc = j[JMPBUFPC];
	r->sp = j[JMPBUFSP];
#ifdef __GNUC__
	/*
	 * GCC port: the callee-saved registers setjmp saved (jmp_buf in
	 * arm64/include/u.h); d8-d15 are not in the Ureg
	 */
	r->r19 = j[2];
	r->r20 = j[3];
	r->r21 = j[4];
	r->r22 = j[5];
	r->r23 = j[6];
	r->r24 = j[7];
	r->r25 = j[8];
	r->r26 = j[9];
	r->r27 = j[10];
	r->r28 = j[11];
	r->r29 = j[12];
#endif
	noted(NCONT);
}
