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
	r->sp = j[JMPBUFSP] + 4;
#ifdef __GNUC__
	/* GCC port: callee-saved registers, see jmp_buf in 386/include/u.h */
	r->bp = j[2];
	r->bx = j[3];
	r->si = j[4];
	r->di = j[5];
#endif
	noted(NCONT);
}
