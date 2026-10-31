/*
 * NIX: kernels without application cores (386).
 * See port/nix.c; *nixac is ignored, execac(2) fails.
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"

int	nixnac;

int
nixboot(int)
{
	return 0;
}

int
isac(int)
{
	return 0;
}

int
runacore(Ureg*)
{
	up->procctl = 0;
	return 0;
}

Mach*
getac(Proc*, int)
{
	error("no application cores");
}

void
putacs(Proc*, Mach*)
{
}

void
putac(Proc *p)
{
	p->ac = nil;
}

void
procctlcore(Proc*, int core)
{
	if(core != 0)
		error("no application cores");
}

Mach*
acflush(Proc*, ulong*)
{
	return nil;
}

int
acflushed(Mach*, Proc*, ulong)
{
	return 1;
}
