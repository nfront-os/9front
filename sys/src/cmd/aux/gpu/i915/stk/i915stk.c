/*
 * SPDX-License-Identifier: MIT
 *
 * aux/i915stk: the call chain of each process of a group as program
 * counters, by its frame pointers (aux/i915 is compiled with them), for
 * a log: the addresses are looked up on the build machine
 * (addr2line -e amd64/bin/aux/i915).  Only amd64.
 *	aux/i915stk pid...
 */
#include <u.h>
#include <libc.h>
#include <ureg.h>

static int
rd(int fd, uvlong a, uvlong *v)
{
	return pread(fd, v, sizeof *v, a) == sizeof *v ? 0 : -1;
}

static void
stack(char *pid)
{
	char path[64];
	struct Ureg u;
	uvlong bp, nbp, pc;
	int fd, mfd, i;

	snprint(path, sizeof path, "/proc/%s/regs", pid);
	if((fd = open(path, OREAD)) < 0 || pread(fd, &u, sizeof u, 0) != sizeof u){
		print("%s: regs: %r\n", pid);
		if(fd >= 0)
			close(fd);
		return;
	}
	close(fd);
	snprint(path, sizeof path, "/proc/%s/mem", pid);
	if((mfd = open(path, OREAD)) < 0){
		print("%s: mem: %r\n", pid);
		return;
	}
	print("%s: pc %#llux", pid, u.pc);
	bp = u.bp;
	/*
	 * in a system call: libc's stubs (9syscall) put the call's number in
	 * BP, keep the caller's BP at 0(SP) and return from 0x50(SP)
	 */
	if(bp < 0x1000){
		if(rd(mfd, u.sp, &bp) < 0 || rd(mfd, u.sp + 0x50, &pc) < 0)
			bp = 0;
		else
			print(" %#llux", pc);
	}
	for(i = 0; i < 40 && bp != 0 && (bp & 7) == 0; i++){
		if(rd(mfd, bp + 8, &pc) < 0 || rd(mfd, bp, &nbp) < 0)
			break;
		print(" %#llux", pc);
		if(nbp <= bp)
			break;
		bp = nbp;
	}
	print("\n");
	close(mfd);
}

void
main(int argc, char **argv)
{
	int i;

	ARGBEGIN{
	default:
		fprint(2, "usage: aux/i915stk pid...\n");
		exits("usage");
	}ARGEND
	for(i = 0; i < argc; i++)
		stack(argv[i]);
	exits(nil);
}
