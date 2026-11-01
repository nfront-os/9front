/*
 * NIX tube device, after NIX's port/devtube.c (Lsub, Bell Labs,
 * Sandia, Vita Nuova; MIT license, see /lib/legal/nix).
 *
 * #=/ctl is a clone file: each open makes a new file #=/n (a Tio).
 * Writing "fd inaddr outaddr" to it sends what is read from fd
 * through the user's tube at inaddr and writes what comes through the
 * tube at outaddr to fd (either address may be 0), each done by a
 * kernel process with the user's segments. Reading gives that line.
 * Writing "up|down addr" makes a kernel process do 10 upsems or
 * downsems on the user's semaphore at addr, "send|recv addr" 10
 * tsends or trecvs of pointers on the user's tube of pointers there
 * (tests of the kernel's entry points, port/syssem.c).
 *
 * Differences to NIX: NIX's tin and tout processes were stubs (one
 * spun for ever); here each message is one read of up to msz bytes,
 * padded with zeros, and one write of msz bytes. Closing the last
 * reference to #=/n stops them (a note). An open of #=/n refers to
 * that Tio (NIX made another).
 */
#include	"u.h"
#include	"../port/lib.h"
#include	"mem.h"
#include	"dat.h"
#include	"fns.h"
#include	"../port/error.h"
#include	"../port/tube.h"

typedef struct Tio Tio;

enum
{
	Qdir,
	Qctl,
	Qdata
};

/*
 * data read from c is sent to tin;
 * data received from tout is written into c
 */
struct Tio
{
	Ref;			/* opens and kernel processes */
	QLock;			/* the processes below */
	int	nopen;
	int	closed;		/* no more opens: the processes stop */
	Rendez	segr;		/* the writer waits here for ... */
	int	tinsegs;	/* ... the processes to have its segments */
	int	toutsegs;
	ulong	path;
	char	*user;
	ulong	perm;
	char	*conf;
	Proc	*uproc;
	Proc	*tinproc;
	Proc	*toutproc;
	KTube	*tin;
	KTube	*tout;
	Chan	*c;
	Tio	*next;
};

static ulong qgen;
static Tio *ftio;
static QLock ftiolck;

/* up takes p's segments */
static void
dupsegs(Proc *p)
{
	int i;
	Segment *s;

	qlock(&p->seglock);
	qlock(&up->seglock);
	for(i = 0; i < NSEG; i++)
		if((s = p->seg[i]) != nil && up->seg[i] == nil){
			incref(s);
			attachseg(up, i, s);
		}
	qunlock(&up->seglock);
	qunlock(&p->seglock);
}

static void
tioput(Tio *ft)
{
	if(decref(ft) > 0)
		return;
	if(ft->c != nil)
		cclose(ft->c);
	if(ft->tin != nil)
		freektube(ft->tin);
	if(ft->tout != nil)
		freektube(ft->tout);
	free(ft->conf);
	free(ft->user);
	free(ft);
}

/* the process *pp of ft starts; 0 if ft is closed already */
static int
tiostart(Tio *ft, Proc **pp)
{
	int ok;

	qlock(ft);
	*pp = up;
	ok = !ft->closed;
	qunlock(ft);
	return ok;
}

static void
tioexit(Tio *ft, Proc **pp)
{
	qlock(ft);
	*pp = nil;
	qunlock(ft);
	tioput(ft);
	pexit("", 1);
}

static void
tinproc(void *arg)
{
	Tio *ft;
	KTube *kt;
	Chan *c;
	uchar *buf;
	long n;
	vlong off;

	ft = arg;
	dupsegs(ft->uproc);
	ft->tinsegs = 1;
	wakeup(&ft->segr);
	kt = ft->tin;
	c = ft->c;
	off = c->offset;
	buf = nil;
	if(tiostart(ft, &ft->tinproc) && !waserror()){
		buf = smalloc(kt->msz);
		for(;;){
			memset(buf, 0, kt->msz);
			n = devtab[c->type]->read(c, buf, kt->msz, off);
			if(n <= 0)
				break;
			off += n;
			tsend(kt, buf);
		}
		poperror();
	}
	free(buf);
	tioexit(ft, &ft->tinproc);
}

static void
toutproc(void *arg)
{
	Tio *ft;
	KTube *kt;
	Chan *c;
	uchar *buf;
	vlong off;

	ft = arg;
	dupsegs(ft->uproc);
	ft->toutsegs = 1;
	wakeup(&ft->segr);
	kt = ft->tout;
	c = ft->c;
	off = c->offset;
	buf = nil;
	if(tiostart(ft, &ft->toutproc) && !waserror()){
		buf = smalloc(kt->msz);
		for(;;){
			trecv(kt, buf);
			off += devtab[c->type]->write(c, buf, kt->msz, off);
		}
	}
	free(buf);
	tioexit(ft, &ft->toutproc);
}

static int
tiosegs(void *a)
{
	Tio *ft;

	ft = a;
	return (ft->tin == nil || ft->tinsegs) && (ft->tout == nil || ft->toutsegs);
}

/* with ft locked; the caller waits for tiosegs after unlocking it */
static void
ftstart(Tio *ft)
{
	ft->uproc = up;
	if(ft->tin != nil){
		incref(ft);
		kproc("tin", tinproc, ft);
	}
	if(ft->tout != nil){
		incref(ft);
		kproc("tout", toutproc, ft);
	}
}

static void
ftstop(Tio *ft)
{
	qlock(ft);
	ft->closed = 1;
	if(ft->tinproc != nil)
		postnote(ft->tinproc, 1, "tube closed", NUser);
	if(ft->toutproc != nil)
		postnote(ft->toutproc, 1, "tube closed", NUser);
	qunlock(ft);
}

static int
ftgen(Chan *c, char*, Dirtab*, int, int s, Dir *dp)
{
	Qid q;
	Tio *ft;

	if(s == DEVDOTDOT){
		devdir(c, c->qid, "#=", 0, eve, DMDIR|0555, dp);
		return 1;
	}

	if(s == 0){
		mkqid(&q, Qctl, 0, QTFILE);
		devdir(c, q, "ctl", 0, eve, 0666, dp);
		return 1;
	}
	s--;

	qlock(&ftiolck);
	for(ft = ftio; ft != nil && s > 0; ft = ft->next)
		s--;

	if(ft == nil){
		qunlock(&ftiolck);
		return -1;
	}

	mkqid(&q, ft->path, 0, QTFILE);
	snprint(up->genbuf, sizeof up->genbuf, "%lud", ft->path);
	devdir(c, q, up->genbuf, 0, ft->user, ft->perm, dp);
	qunlock(&ftiolck);
	return 1;
}

static Chan*
ftattach(char *spec)
{
	return devattach('=', spec);
}

static Walkqid*
ftwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, 0, 0, ftgen);
}

static int
ftstat(Chan *c, uchar *db, int n)
{
	return devstat(c, db, n, 0, 0, ftgen);
}

static Chan*
ftopen(Chan *c, int omode)
{
	Tio *ft, **ftl;

	if(c->qid.type&QTDIR){
		if(omode & ORCLOSE)
			error(Eperm);
		if(omode != OREAD)
			error(Eisdir);
		c->mode = openmode(omode);
		c->flag |= COPEN;
		c->offset = 0;
		return c;
	}
	qlock(&ftiolck);
	if(c->qid.path == Qctl){
		ft = smalloc(sizeof *ft);
		ft->ref = 1;
		kstrdup(&ft->user, up->user);
		ft->perm = 0660;
		ft->path = Qdata + qgen++;
		for(ftl = &ftio; *ftl != nil; ftl = &(*ftl)->next)
			;
		*ftl = ft;
	}else{
		for(ft = ftio; ft != nil; ft = ft->next)
			if(ft->path == c->qid.path)
				break;
		if(ft == nil){
			qunlock(&ftiolck);
			error(Enonexist);
		}
		if(strcmp(up->user, ft->user) != 0 && !iseve()){
			qunlock(&ftiolck);
			error(Eperm);
		}
		incref(ft);
	}
	ft->nopen++;
	mkqid(&c->qid, ft->path, 0, QTFILE);
	qunlock(&ftiolck);
	c->aux = ft;
	c->mode = openmode(omode);
	c->flag |= COPEN;
	c->offset = 0;
	return c;
}

static void
ftclose(Chan *c)
{
	Tio *ft, **ftl;

	if((c->qid.type&QTDIR) || (c->flag&COPEN) == 0 || (ft = c->aux) == nil)
		return;
	qlock(&ftiolck);
	if(--ft->nopen == 0){
		for(ftl = &ftio; *ftl != nil; ftl = &(*ftl)->next)
			if(*ftl == ft){
				*ftl = ft->next;
				break;
			}
		ftstop(ft);
	}
	qunlock(&ftiolck);
	tioput(ft);
}

static long
ftread(Chan *c, void *a, long n, vlong offset)
{
	Tio *ft;
	char *s;

	if(c->qid.type & QTDIR)
		return devdirread(c, a, n, 0, 0, ftgen);
	ft = c->aux;
	s = ft->conf;
	if(s == nil)
		s = "none\n";
	return readstr(offset, a, n, s);
}

typedef struct Testarg Testarg;
struct Testarg
{
	Rendez;
	Proc	*uproc;
	Usem	*s;
	KTube	*kt;
	int	isup;		/* or issend */
	int	started;
};

static int
started(void *x)
{
	Testarg *arg = x;

	return arg->started;
}

/*
 * Test semaphores shared with the user.
 * Only 10 ups or downs.
 */
static void
semproc(void *x)
{
	Testarg *arg = x;
	int i, isup;
	Usem *s;

	dupsegs(arg->uproc);
	s = arg->s;
	isup = arg->isup;
	arg->started = 1;
	wakeup(arg);	/* the caller frees arg */
	if(!waserror()){
		for(i = 0; i < 10; i++){
			if(isup){	/* let the user block... */
				tsleep(&up->sleep, return0, 0, 300);
				upsem(s);
			}else
				downsem(s, 0);
		}
		poperror();
	}
	pexit("", 1);
}

static void
tubeproc(void *x)
{
	Testarg *arg = x;
	int i, issend;
	uintptr addr;
	KTube *kt;

	dupsegs(arg->uproc);
	kt = arg->kt;
	issend = arg->isup;
	arg->started = 1;
	wakeup(arg);	/* the caller frees arg */
	addr = 0x1000;
	if(!waserror()){
		for(i = 0; i < 10; i++){
			if(issend){	/* let the user block... */
				tsleep(&up->sleep, return0, 0, 300);
				tsend(kt, &addr);
			}else
				trecv(kt, &addr);
			addr++;
		}
		poperror();
	}
	freektube(kt);
	pexit("", 1);
}

static void
testsem(uintptr va, int isup)
{
	Segment *sg;
	Testarg *arg;
	Usem *s;

	evenaddr(va);
	validaddr(va, sizeof(int), 1);
	sg = seg(up, va, 0);
	if(sg == nil)
		error("no such semaphore");
	s = segmksem(sg, (int*)va);
	arg = smalloc(sizeof *arg);
	arg->uproc = up;
	arg->s = s;
	arg->isup = isup;
	kproc("sem", semproc, arg);
	while(waserror())
		;
	sleep(arg, started, arg);
	poperror();
	free(arg);
}

static void
testtube(uintptr va, int issend)
{
	KTube *kt;
	Testarg *arg;

	kt = u2ktube((Tube*)va);
	if(kt->msz != sizeof(uintptr)){
		freektube(kt);
		error("not a tube of pointers");
	}
	arg = smalloc(sizeof *arg);
	arg->uproc = up;
	arg->kt = kt;
	arg->isup = issend;
	kproc("tube", tubeproc, arg);
	while(waserror())
		;
	sleep(arg, started, arg);
	poperror();
	free(arg);
}

/*
 * Ctls:
 *	up sem_address		-- make the kernel do 10 ups there
 *	down sem_address	-- make the kernel do 10 downs there
 *	send tube_address	-- make the kernel send ptrs
 *	recv tube_address	-- make the kernel recv ptrs
 */
static void
testing(char *tok[])
{
	if(strcmp(tok[0], "up") == 0)
		testsem(strtoull(tok[1], 0, 0), 1);
	else if(strcmp(tok[0], "down") == 0)
		testsem(strtoull(tok[1], 0, 0), 0);
	else if(strcmp(tok[0], "send") == 0)
		testtube(strtoull(tok[1], 0, 0), 1);
	else if(strcmp(tok[0], "recv") == 0)
		testtube(strtoull(tok[1], 0, 0), 0);
	else
		error("unknown testing ctl");
}

static long
ftwrite(Chan *c, void *a, long n, vlong)
{
	char *tok[5], buf[128];
	int ntok, fd;
	uintptr a1, a2;
	Tio *ft;
	Chan *fc;

	if(c->qid.type & QTDIR)
		error(Eperm);
	if(n > sizeof buf - 1)
		error("config string is too large");
	memmove(buf, a, n);
	buf[n] = 0;
	ntok = tokenize(buf, tok, nelem(tok));
	if(ntok == 2){
		testing(tok);
		return n;
	}
	if(ntok != 3)
		error("config must be 'fd addr addr'");
	ft = c->aux;
	qlock(ft);
	if(waserror()){
		qunlock(ft);
		nexterror();
	}
	if(ft->c != nil)
		error("can't configure twice");
	a1 = strtoull(tok[1], 0, 0);
	a2 = strtoull(tok[2], 0, 0);
	if(a1 == 0 && a2 == 0)
		error("both in and out tubes are nil");
	fd = strtoul(tok[0], 0, 0);
	fc = fdtochan(fd, -1, 0, 1);	/* check errors and incref */
	if(waserror()){
		cclose(fc);
		nexterror();
	}
	if(a1 != 0 && fc->mode != OREAD && fc->mode != ORDWR)
		error(Ebadusefd);
	if(a2 != 0 && fc->mode != OWRITE && fc->mode != ORDWR)
		error(Ebadusefd);
	if(a1 != 0)
		ft->tin = u2ktube((Tube*)a1);
	if(a2 != 0)
		ft->tout = u2ktube((Tube*)a2);
	poperror();
	ft->c = fc;
	ft->conf = smalloc(n+2);	/* n bytes + \n + \0 */
	memmove(ft->conf, a, n);
	strcpy(ft->conf+n, "\n");
	ftstart(ft);
	qunlock(ft);
	poperror();
	while(waserror())
		;
	sleep(&ft->segr, tiosegs, ft);
	poperror();
	return n;
}

Dev tubedevtab = {
	'=',
	"tube",

	devreset,
	devinit,
	devshutdown,
	ftattach,
	ftwalk,
	ftstat,
	ftopen,
	devcreate,
	ftclose,
	ftread,
	devbread,
	ftwrite,
	devbwrite,
	devremove,
	devwstat,
};
