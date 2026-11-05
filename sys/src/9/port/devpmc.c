/*
 * Performance counters, pmc(3): a directory per processor with its
 * counters; after NIX's port/devpmc.c and k8/pmcio.c (Lsub, Bell
 * Labs, Sandia, Vita Nuova; MIT license, see /lib/legal/nix).
 *
 *	#ε/ctrdesc		names of some configurations
 *	#ε/core			the processor the reader runs on
 *	#ε/coreNNNN/ctrNN	value of counter NN of processor NNNN
 *	#ε/coreNNNN/ctrNNctl	its configuration
 *
 * The counters are registers of each processor. A request for
 * another processor goes there: the requester puts it into the
 * target's Pmccore, sends it an interrupt (pmcipi: its own vector
 * on amd64, an SGI on arm64) and sleeps; the target's handler
 * (pmcintr) serves it, wakes the requester and interrupts the
 * requester's processor too (it may idle in halt). That works on a
 * time-sharing core (TC) as on an application core (AC, port/nix.c):
 * an AC takes no clock and no device interrupts, but this one; in
 * user mode the AC serves it and returns to the process, which
 * does not notice (not a round trip to its TC as the IPI of
 * port/nix.c). A request for the processor the requester runs on is
 * served at once, at splhi.
 *
 * Unlike NIX: NIX's TCs served requests at their next kernel entry
 * or exit (_pmcupdate in trap, kexit), an AC through runac() only
 * when the process asking was the one on that AC (its ICC call never
 * readied the caller), and any other process asking for an AC's
 * counters waited for good. Requests time out here (Etimedout).
 * NIX's libc found the processor it runs on in the Tos (getcore(2));
 * here it reads #ε/core.
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"
#include	"../port/pmc.h"

enum {
	Qdir,
	Qctrdesc,
	Qself,		/* #ε/core */
	Qcore,
	Qdata,
	Qctl,

	PmcCtlRdStr	= 4*1024,

	/* Pmcreq.op */
	Getctr	= 0,
	Setctr,
	Getctl,
	Setctl,
};

#define QTYPE(q)	((int)((q).path & 0xff))
#define QREG(q)		((int)(((q).path>>8) & 0xff))
#define QCORE(q)	((int)((q).path>>16))
#define QPATH(c, r, t)	(((uvlong)(c)<<16)|((r)<<8)|(t))

typedef struct Pmcreq Pmcreq;
typedef struct Pmccore Pmccore;

struct Pmcreq {
	int	op;
	int	regno;
	u64int	v;
	PmcCtl	ctl;
	int	rc;
};

/* a processor's requests */
struct Pmccore {
	QLock;			/* one request at a time */
	Lock	reqlk;		/* req, pending, done: requester and pmcintr */
	Rendez	r;
	Pmcreq	req;
	int	pending;	/* req waits for pmcintr */
	int	done;		/* served */
	int	waiter;		/* processor of the requester */
};

static Pmccore	pmccore[MAXMACH];
static int	npmc;		/* counters per processor */
static int	pmcdebug;

static int
pmccoreok(int core)
{
	Mach *mp;

	if(core < 0 || core >= conf.nmach || core >= MAXMACH)
		return 0;
	if((mp = MACHP(core)) == nil)
		return 0;
	if(active.machs[core])
		return 1;
	return mp->PMach.nixtype == NIXAC && mp->PMach.icc != nil;	/* NIX: an AC */
}

static void
pmcserve(Pmcreq *r)
{
	switch(r->op){
	case Getctr:
		r->rc = pmcgetctr(&r->v, r->regno);
		break;
	case Setctr:
		r->rc = pmcsetctr(r->v, r->regno);
		break;
	case Getctl:
		r->rc = pmcgetctl(&r->ctl, r->regno);
		break;
	case Setctl:
		r->rc = pmcsetctl(&r->ctl, r->regno);
		break;
	default:
		r->rc = -1;
	}
}

/*
 * The interrupt handler of pmcipi, on the processor asked.
 */
void
pmcintr(void)
{
	Pmccore *c;
	int waiter;

	c = &pmccore[m->machno];
	waiter = -1;
	ilock(&c->reqlk);
	if(c->pending){
		pmcserve(&c->req);
		c->pending = 0;
		c->done = 1;
		waiter = c->waiter;
	}
	iunlock(&c->reqlk);
	if(waiter < 0)
		return;
	/*
	 * the requester's processor may idle in halt (no MONITOR) and
	 * see it ready only at its next interrupt: interrupt it, as
	 * acexit does (port/nix.c); there pmcintr finds nothing to do.
	 */
	if(wakeup(&c->r) != nil && waiter != m->machno)
		pmcipi(waiter);
}

static int
pmcdonef(void *a)
{
	return ((Pmccore*)a)->done;
}

/*
 * Serve r on processor core; error() if it does not answer.
 */
static void
pmcrpc(int core, Pmcreq *r)
{
	Pmccore *c;
	int s, i;

	if(!pmccoreok(core))
		error(Enonexist);
	s = splhi();
	if(core == m->machno){
		pmcserve(r);
		splx(s);
		return;
	}
	splx(s);
	c = &pmccore[core];
	eqlock(c);
	if(waserror()){
		ilock(&c->reqlk);
		c->pending = 0;
		iunlock(&c->reqlk);
		qunlock(c);
		nexterror();
	}
	ilock(&c->reqlk);
	c->req = *r;
	c->done = 0;
	c->waiter = m->machno;
	c->pending = 1;
	iunlock(&c->reqlk);
	for(i = 0; !c->done; i++){
		if(i == 10)
			error(Etimedout);
		pmcipi(core);
		tsleep(&c->r, pmcdonef, c, 100);
	}
	ilock(&c->reqlk);
	*r = c->req;
	iunlock(&c->reqlk);
	poperror();
	qunlock(c);
}

static void
pmcnull(PmcCtl *p)
{
	memset(p, 0, sizeof(PmcCtl));
	p->enab = PmcCtlNullval;
	p->user = PmcCtlNullval;
	p->os = PmcCtlNullval;
	p->reset = PmcCtlNullval;
	p->nodesc = 1;
}

static int
pmcctlstr(char *str, int nstr, PmcCtl *p)
{
	int ns;

	ns = 0;
	if(p->enab && p->enab != PmcCtlNullval)
		ns += snprint(str + ns, nstr - ns, "enable\n");
	else
		ns += snprint(str + ns, nstr - ns, "disable\n");
	if(p->user && p->user != PmcCtlNullval)
		ns += snprint(str + ns, nstr - ns, "user\n");
	if(p->os && p->os != PmcCtlNullval)
		ns += snprint(str + ns, nstr - ns, "os\n");
	if(!p->nodesc)
		ns += snprint(str + ns, nstr - ns, "%s\n", p->descstr);
	else
		ns += snprint(str + ns, nstr - ns, "no desc\n");
	return ns;
}

static void
pmcinit(void)
{
	npmc = pmcarchinit();
	if(npmc > PmcMaxCtrs)
		npmc = PmcMaxCtrs;
	if(npmc < 0)
		npmc = 0;
}

static Chan*
pmcattach(char *spec)
{
	return devattach(L'ε', spec);
}

static int
pmcgen(Chan *c, char*, Dirtab*, int, int s, Dir *dp)
{
	Qid q;
	int t, core, reg;
	char *buf;

	buf = up->genbuf;	/* devdir keeps the pointer */
	if(s == DEVDOTDOT){
		mkqid(&q, Qdir, 0, QTDIR);
		devdir(c, q, "#ε", 0, eve, DMDIR|0555, dp);
		return 1;
	}
	t = QTYPE(c->qid);
	if(t == Qdir || t == Qctrdesc || t == Qself){
		if(s == 0){
			mkqid(&q, Qctrdesc, 0, QTFILE);
			devdir(c, q, "ctrdesc", 0, eve, 0444, dp);
			return 1;
		}
		if(s == 1){
			mkqid(&q, Qself, 0, QTFILE);
			devdir(c, q, "core", 0, eve, 0444, dp);
			return 1;
		}
		core = s-2;
		if(core >= conf.nmach || core >= MAXMACH)
			return -1;
		if(!pmccoreok(core))
			return 0;
		snprint(buf, sizeof up->genbuf, "core%4.4d", core);
		mkqid(&q, QPATH(core, 0, Qcore), 0, QTDIR);
		devdir(c, q, buf, 0, eve, DMDIR|0555, dp);
		return 1;
	}
	core = QCORE(c->qid);
	if(s >= 2*npmc)
		return -1;
	reg = s/2;
	if(s & 1){
		snprint(buf, sizeof up->genbuf, "ctr%2.2dctl", reg);
		mkqid(&q, QPATH(core, reg, Qctl), 0, QTFILE);
	}else{
		snprint(buf, sizeof up->genbuf, "ctr%2.2d", reg);
		mkqid(&q, QPATH(core, reg, Qdata), 0, QTFILE);
	}
	devdir(c, q, buf, 0, eve, 0600, dp);
	return 1;
}

static Walkqid*
pmcwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, nil, 0, pmcgen);
}

static int
pmcstat(Chan *c, uchar *dp, int n)
{
	return devstat(c, dp, n, nil, 0, pmcgen);
}

static Chan*
pmcopen(Chan *c, int omode)
{
	return devopen(c, omode, nil, 0, pmcgen);
}

static void
pmcclose(Chan*)
{
}

static long
pmcread(Chan *c, void *a, long n, vlong offset)
{
	Pmcreq r;
	char *s;

	switch(QTYPE(c->qid)){
	case Qdir:
	case Qcore:
		return devdirread(c, a, n, nil, 0, pmcgen);
	}

	s = smalloc(PmcCtlRdStr);
	if(waserror()){
		free(s);
		nexterror();
	}
	memset(&r, 0, sizeof r);
	r.regno = QREG(c->qid);
	switch(QTYPE(c->qid)){
	case Qdata:
		r.op = Getctr;
		pmcrpc(QCORE(c->qid), &r);
		if(r.rc < 0)
			error(Eio);
		snprint(s, PmcCtlRdStr, "%#llux", r.v);
		break;
	case Qctl:
		r.op = Getctl;
		pmcrpc(QCORE(c->qid), &r);
		if(r.rc < 0)
			error(Eio);
		pmcctlstr(s, PmcCtlRdStr, &r.ctl);
		break;
	case Qctrdesc:
		pmcdescstr(s, PmcCtlRdStr);
		break;
	case Qself:
		/* NIX: a process on an AC makes its system calls on a TC */
		snprint(s, PmcCtlRdStr, "%d", up->ac != nil? up->ac->machno: m->machno);
		break;
	default:
		error(Egreg);
	}
	n = readstr(offset, a, n, s);
	poperror();
	free(s);
	return n;
}

enum {
	Enable,
	Disable,
	User,
	Os,
	Reset,
	Debug,
};

static Cmdtab pmcctlmsg[] =
{
	Enable,		"enable",	1,
	Disable,	"disable",	1,
	User,		"user",		1,
	Os,		"os",		1,
	Reset,		"reset",	1,
	Debug,		"debug",	1,
};

static long
pmcwrite(Chan *c, void *a, long n, vlong)
{
	Cmdbuf *cb;
	Cmdtab *ct;
	Pmcreq r;
	char str[64], *p;

	if(c->qid.type == QTDIR)
		error(Eperm);
	if(n >= sizeof(str))
		error(Ebadctl);
	memmove(str, a, n);
	str[n] = '\0';

	memset(&r, 0, sizeof r);
	r.regno = QREG(c->qid);
	switch(QTYPE(c->qid)){
	case Qdata:
		r.op = Setctr;
		r.v = strtoull(str, &p, 0);
		if(p == str)
			error(Ebadarg);
		break;
	case Qctl:
		r.op = Setctl;
		pmcnull(&r.ctl);
		/* "set" takes the rest of the line, blanks and all */
		if(strncmp(str, "set ", 4) == 0){
			if((p = strchr(str, '\n')) != nil)
				*p = '\0';
			strncpy(r.ctl.descstr, str + 4, KNAMELEN-1);
			r.ctl.nodesc = 0;
			if(pmctrans(&r.ctl) < 0)
				error(Ebadctl);
			break;
		}
		cb = parsecmd(a, n);
		if(waserror()){
			free(cb);
			nexterror();
		}
		ct = lookupcmd(cb, pmcctlmsg, nelem(pmcctlmsg));
		switch(ct->index){
		case Enable:
			r.ctl.enab = 1;
			break;
		case Disable:
			r.ctl.enab = 0;
			break;
		case User:
			r.ctl.user = 1;
			break;
		case Os:
			r.ctl.os = 1;
			break;
		case Reset:
			r.ctl.reset = 1;
			break;
		case Debug:
			pmcdebug = !pmcdebug;
			break;
		}
		poperror();
		free(cb);
		break;
	default:
		error(Eperm);
	}
	pmcrpc(QCORE(c->qid), &r);
	if(r.rc < 0)
		error(Eio);
	if(pmcdebug && r.op == Setctl){
		r.op = Getctl;
		pmcrpc(QCORE(c->qid), &r);
		print("pmc: core%d ctr%d: enab %d user %d os %d %s\n",
			QCORE(c->qid), r.regno, r.ctl.enab, r.ctl.user, r.ctl.os, r.ctl.descstr);
	}
	return n;
}

Dev pmcdevtab = {
	L'ε',
	"pmc",

	pmcinit,
	devinit,
	devshutdown,
	pmcattach,
	pmcwalk,
	pmcstat,
	pmcopen,
	devcreate,
	pmcclose,
	pmcread,
	devbread,
	pmcwrite,
	devbwrite,
	devremove,
	devwstat,
};
