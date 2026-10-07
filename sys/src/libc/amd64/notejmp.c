#include <u.h>
#include <libc.h>
#include <ureg.h>

void
notejmp(void *vr, jmp_buf j, int ret)
{
	struct Ureg *r = vr;

	r->ax = ret;
	if(ret == 0)
		r->ax = 1;
	r->pc = j[JMPBUFPC];
	r->sp = j[JMPBUFSP] + 8;
	/* GCC port: callee-saved registers, see jmp_buf in amd64/include/u.h */
	r->bp = j[2];
	r->bx = j[3];
	r->r12 = j[4];
	r->r13 = j[5];
	r->r14 = j[6];
	r->r15 = j[7];
	noted(NCONT);
}
