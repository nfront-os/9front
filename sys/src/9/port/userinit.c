#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"

/*
 * The initcode array contains the binary text of the first
 * user process. Its job is to invoke the exec system call
 * for /boot/boot.
 * Initcode does not link with standard plan9 libc _main()
 * trampoline due to size constrains. Instead it is linked
 * with a small machine specific trampoline init9.s that
 * only sets the base address register and passes arguments
 * to startboot() (see port/initcode.c).
 */
#include	"initcode.i"

/*
 * GCC port, exec/ELF64-loader test only (see MEMORY.md, scripts/
 * gen-exectest-payloads.sh): two real-toolchain test binaries (a gcc-
 * compiled static ELF64 executable and a real `GOOS=plan9 GOARCH=amd64
 * go build` a.out binary), embedded here since no real filesystem
 * exists yet to exec() them from. Mapped read-only into the bootstrap
 * test process's address space below so scripts/testinit.S can copy
 * them into a #g (devsegment) shared segment -- the only way to give
 * sysexec() an actual Chan to read from without a filesystem -- and
 * exec() them, exercising the ELF64 loader and the a.out loader with
 * genuine compiler output instead of hand-assembled code.
 */
#include	"testelf.i"
#include	"testgo.i"

#define TESTELFVA	0x500000ULL
#define TESTGOVA	0x700000ULL

static void
loadrotext(int seg, uintptr base, uchar *data, ulong len)
{
	KMap *k;
	Page *p;
	ulong off, n;

	attachseg(up, seg, newseg(SG_TEXT | SG_RONLY, base, (len+BY2PG-1)/BY2PG));
	for(off = 0; off < len; off += BY2PG){
		p = newpage(base+off, nil);
		k = kmap(p);
		n = len-off;
		if(n > BY2PG)
			n = BY2PG;
		memmove((uchar*)VA(k), data+off, n);
		if(n < BY2PG)
			memset((uchar*)VA(k)+n, 0, BY2PG-n);
		kunmap(k);
		segpage(up->seg[seg], p);
	}
}

/*
 * The first kernel process starts here.
 */
static void
proc0(void*)
{
	KMap *k;
	Page *p;

	spllo();
	if(waserror())
		panic("proc0: %s", up->errstr);

	up->pgrp = newpgrp();
	up->egrp = smalloc(sizeof(Egrp));
	up->egrp->Ref.ref = 1;
	up->fgrp = dupfgrp(nil);
	up->rgrp = newrgrp();

	/*
	 * These are o.k. because rootinit is null.
	 * Then early kproc's will have a root and dot.
	 */
	up->slash = namec("#/", Atodir, 0, 0);
	pathclose(up->slash->path);
	up->slash->path = newpath("/");
	up->dot = cclone(up->slash);

	/*
	 * Setup Text and Stack segments for initcode.
	 */
	qlock(&up->seglock);
	attachseg(up, SSEG, newseg(SG_STACK | SG_NOEXEC, USTKTOP-USTKSIZE, USTKSIZE / BY2PG));
	attachseg(up, TSEG, newseg(SG_TEXT | SG_RONLY, UTZERO, 1));
	qunlock(&up->seglock);

	p = newpage(UTZERO, nil);
	k = kmap(p);
	assert(sizeof(initcode) <= BY2PG);
	memmove((uchar*)VA(k), initcode, sizeof(initcode));
	memset((uchar*)VA(k)+sizeof(initcode), 0, BY2PG-sizeof(initcode));
	kunmap(k);
	segpage(up->seg[TSEG], p);

	/* GCC port, exec/ELF64-loader test only -- see comment above */
	qlock(&up->seglock);
	loadrotext(SEG1, TESTELFVA, testelf, sizeof(testelf));
	loadrotext(SEG2, TESTGOVA, testgo, sizeof(testgo));
	qunlock(&up->seglock);

	/*
	 * Become a user process.
	 */
	up->kp = 0;
	up->noswap = 0;
	up->privatemem = 0;
	procpriority(up, PriNormal, 0);
	procsetup(up);

	flushmmu();

	poperror();

	/*
	 * init0():
	 *	call chandevinit()
	 *	setup environment variables
	 *	prepare the stack for initcode
	 *	switch to usermode to run initcode
	 */
	init0();

	/* init0 will never return */
	panic("init0");
}

void
userinit(void)
{
	up = nil;
	kstrdup(&eve, "");
	kproc("*init*", proc0, nil);
}
