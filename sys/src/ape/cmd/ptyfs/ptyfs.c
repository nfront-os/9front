/*
 * ptyfs: pseudo-terminals for APE (sys/src/ape/lib/bsd/pty.c starts it
 * as /bin/ape/ptyfs and mounts /srv/ptyfs after /dev). Opening ptyclone
 * makes a pty N: the opener holds its master ttymN (a terminal emulator:
 * what it writes is typed, what it reads is shown), the slave pttyN is
 * the terminal of the programs in it. For them it is /dev/cons: APE's
 * ioctl TIOCSCTTY binds pttyN on /dev/cons and pconsctlN on /dev/consctl
 * (rawon, rawoff as cons(3)), so native programs run in it as in a rio
 * window, POSIX programs with termios.
 *
 * Between the two sides the line discipline: input is edited by lines
 * (ICANON: erase, kill, word erase, EOF), echoed (ECHO), CR made NL
 * (ICRNL); the interrupt and quit characters (ISIG) post the notes
 * interrupt and quit to the terminal's note group (tcsetpgrp,
 * TIOCSCTTY); output's NL becomes CR NL (OPOST, ONLCR). When the master
 * closes, the note group gets hangup and the slave reads end of file;
 * when the slave was open and is no more, the master reads end of file.
 *
 * Control, in the pty's ptyctlN (APE's tcgetattr.c, ioctl.c; the old
 * protocol at the file offset -2 needs negative offsets, which 9front's
 * kernel refuses): reading gives "IOR iflag oflag cflag lflag cc... pgrp
 * rows cols xpixels ypixels" (flags and characters in hex, as APE's
 * termios.h has them); writing "IOW iflag oflag cflag lflag cc..." sets
 * them, "IOW note pid" the note group, "IOW winsize rows cols xpixels
 * ypixels" the size.
 *
 * usage: ptyfs [-D] [-s srvname]	(default /srv/ptyfs)
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>

/* APE's termios.h */
enum {
	ICRNL = 0x002,
	IGNCR = 0x008,
	INLCR = 0x020,
	ISTRIP = 0x080,
	IXON = 0x200,

	OPOST = 01,
	ONLCR = 04,

	CREAD = 0x002,
	CS8 = 0x010,

	ECHO = 0x001,
	ECHOE = 0x002,
	ECHOK = 0x004,
	ECHONL = 0x008,
	ICANON = 0x010,
	IEXTEN = 0x020,
	ISIG = 0x040,
	NOFLSH = 0x080,

	VEOF = 0,
	VEOL,
	VERASE,
	VINTR,
	VKILL,
	VMIN,
	VQUIT,
	VSUSP,
	VTIME,
	VSTART,
	VSTOP,
	NCCS,

	CWERASE = 027,	/* ^W, with IEXTEN (APE's NCCS has no VWERASE) */
};

enum {
	Maxline = 4096,	/* a line being edited */
	Maxout = 256*1024,	/* output not yet read: then a writer waits */
	Maxin = 256*1024,	/* input not yet read: then the master waits */
};

/*
 * a pty's files: qid.path Ptyqid | n<<4 | type; APE knows a pty by it
 * (ap/plan9/tcgetattr.c: _ptynum), as fstat's name is the one the file
 * was opened by (ptyclone, /dev/cons it is bound on)
 */
#define Ptyqid	(0x505459ULL<<40)	/* "PTY" */

enum {
	Qclone = 1,
	Qmaster,
	Qslave,
	Qctl,
	Qptyctl,
};

typedef struct Rec Rec;
typedef struct Pty Pty;

/*
 * input for the slave, in records: a canonical read takes from one at
 * most (a line, or an empty one for EOF); raw input is a record still
 * open (done 0) that bytes are added to
 */
struct Rec {
	Rec	*next;
	char	*data;
	int	n, off, cap;
	int	eof;
	int	done;
};

struct Pty {
	int	n;
	File	*master, *slave, *ctl, *ptyctl;
	int	nmaster, nslave;	/* open fids */
	int	hadslave;
	ulong	iflag, oflag, cflag, lflag;
	uchar	cc[NCCS];
	int	pgrp;
	int	row, col, xpix, ypix;
	int	raw;	/* consctl rawon */
	ulong	cookedl;	/* lflag before rawon */
	char	line[Maxline];
	int	nline;
	Rec	*in, *inlast;
	long	nin;
	char	*out;
	long	nout, outcap;
	Req	*sreads, *mreads, *swrites, *mwrites;	/* waiting */
};

static Tree *tree;
static int nextn;

static void
defaults(Pty *p)
{
	p->iflag = ICRNL|IXON;
	p->oflag = OPOST|ONLCR;
	p->cflag = CREAD|CS8;
	p->lflag = ISIG|ICANON|ECHO|ECHOE|ECHOK|IEXTEN;
	memset(p->cc, 0, sizeof p->cc);
	p->cc[VEOF] = 004;	/* ^D */
	p->cc[VERASE] = 0177;	/* DEL, a terminal emulator's backspace */
	p->cc[VINTR] = 003;	/* ^C */
	p->cc[VKILL] = 025;	/* ^U */
	p->cc[VMIN] = 1;
	p->cc[VQUIT] = 034;	/* ^\ */
	p->cc[VSUSP] = 032;	/* ^Z: no job control, dropped */
	p->cc[VSTART] = 021;
	p->cc[VSTOP] = 023;
	p->row = 24;
	p->col = 80;
}

static int
qtype(File *f)
{
	return f->qid.path & 0xF;
}

static uvlong
ptyqid(int n, int type)
{
	return Ptyqid | (uvlong)n<<4 | type;
}

/* ---- queues of requests (linked through Req.aux) ---- */

static void
enqueue(Req **q, Req *r)
{
	Req **l;

	r->aux = nil;
	for(l = q; *l != nil; l = (Req**)&(*l)->aux)
		;
	*l = r;
}

static Req*
dequeue(Req **q)
{
	Req *r;

	if((r = *q) != nil)
		*q = r->aux;
	return r;
}

static int
unqueue(Req **q, Req *r)
{
	Req **l;

	for(l = q; *l != nil; l = (Req**)&(*l)->aux)
		if(*l == r){
			*l = r->aux;
			return 1;
		}
	return 0;
}

/* ---- output: what the slave writes and the echo, for the master ---- */

static void
outbyte(Pty *p, int c)
{
	if(p->nout >= p->outcap){
		p->outcap = p->outcap ? 2*p->outcap : 4096;
		p->out = erealloc9p(p->out, p->outcap);
	}
	p->out[p->nout++] = c;
}

static void
output(Pty *p, char *s, long n)
{
	long i;

	for(i = 0; i < n; i++){
		if(s[i] == '\n' && (p->oflag & (OPOST|ONLCR)) == (OPOST|ONLCR))
			outbyte(p, '\r');
		outbyte(p, s[i]);
	}
}

/* ---- input: what the master writes, through the line discipline ---- */

static Rec*
addrec(Pty *p, char *s, int n, int eof, int done)
{
	Rec *r;

	r = emalloc9p(sizeof *r);
	r->cap = n > 64 ? n : 64;
	r->data = emalloc9p(r->cap);
	memmove(r->data, s, n);
	r->n = n;
	r->eof = eof;
	r->done = done;
	if(p->inlast != nil)
		p->inlast->next = r;
	else
		p->in = r;
	p->inlast = r;
	p->nin += n;
	return r;
}

static void
freerec(Pty *p)
{
	Rec *r;

	r = p->in;
	p->in = r->next;
	if(p->in == nil)
		p->inlast = nil;
	p->nin -= r->n - r->off;
	free(r->data);
	free(r);
}

static void
rawbyte(Pty *p, int c)
{
	Rec *r;
	char b;

	r = p->inlast;
	if(r == nil || r->done || r->eof){
		b = c;
		addrec(p, &b, 1, 0, 0);
		return;
	}
	if(r->n == r->cap){
		r->cap *= 2;
		r->data = erealloc9p(r->data, r->cap);
	}
	r->data[r->n++] = c;
	p->nin++;
}

static void
flushinput(Pty *p)
{
	while(p->in != nil)
		freerec(p);
	p->nline = 0;
}

static void
signote(Pty *p, char *note)
{
	if(p->pgrp > 0)
		postnote(PNGROUP, p->pgrp, note);
}

/* the line edited so far is a record: at a newline, or EOF (then without it) */
static void
endline(Pty *p, int eof)
{
	addrec(p, p->line, p->nline, eof && p->nline == 0, 1);
	p->nline = 0;
}

/* the line's last rune erased; its bytes */
static int
eraserune(Pty *p)
{
	int n;

	if(p->nline == 0)
		return 0;
	n = 1;
	while(n < p->nline && (p->line[p->nline-n] & 0xC0) == 0x80)
		n++;
	p->nline -= n;
	if((p->lflag & (ECHO|ECHOE)) == (ECHO|ECHOE))
		output(p, "\b \b", 3);
	return n;
}

static int
blank(int c)
{
	return c == ' ' || c == '\t';
}

static void
inbyte(Pty *p, int c)
{
	char b;

	if(p->iflag & ISTRIP)
		c &= 0x7F;
	if(c == '\r'){
		if(p->iflag & IGNCR)
			return;
		if(p->iflag & ICRNL)
			c = '\n';
	}else if(c == '\n' && (p->iflag & INLCR))
		c = '\r';
	b = c;
	if(p->lflag & ISIG){
		if(c != 0 && c == p->cc[VINTR]){
			if(!(p->lflag & NOFLSH))
				flushinput(p);
			if(p->lflag & ECHO)
				output(p, "^C", 2);
			signote(p, "interrupt");
			return;
		}
		if(c != 0 && c == p->cc[VQUIT]){
			if(!(p->lflag & NOFLSH))
				flushinput(p);
			if(p->lflag & ECHO)
				output(p, "^\\", 2);
			signote(p, "quit");
			return;
		}
		if(c != 0 && c == p->cc[VSUSP])
			return;
	}
	if(!(p->lflag & ICANON)){
		rawbyte(p, c);
		if(p->lflag & ECHO)
			output(p, &b, 1);
		return;
	}
	if(c != 0 && c == p->cc[VERASE] || c == '\b'){
		eraserune(p);
		return;
	}
	if(c != 0 && c == p->cc[VKILL]){
		if((p->lflag & (ECHO|ECHOE)) == (ECHO|ECHOE))
			while(eraserune(p) > 0)
				;
		else{
			p->nline = 0;
			if(p->lflag & ECHOK)
				output(p, "\n", 1);
		}
		return;
	}
	if(c == CWERASE && (p->lflag & IEXTEN)){
		while(p->nline > 0 && blank(p->line[p->nline-1]))
			eraserune(p);
		while(p->nline > 0 && !blank(p->line[p->nline-1]))
			eraserune(p);
		return;
	}
	if(c != 0 && c == p->cc[VEOF]){
		endline(p, 1);
		return;
	}
	if(c == '\n' || c != 0 && c == p->cc[VEOL]){
		p->line[p->nline++] = c;	/* Maxline-1 at most before: room */
		if(p->lflag & (ECHO|ECHONL))
			output(p, &b, 1);
		endline(p, 0);
		return;
	}
	if(p->nline >= Maxline-1)
		return;	/* full: only its end goes in */
	p->line[p->nline++] = c;
	if(p->lflag & ECHO)
		output(p, &b, 1);
}

/* ICANON off: the line being edited is raw input */
static void
setlflag(Pty *p, ulong l)
{
	if((p->lflag & ICANON) && !(l & ICANON) && p->nline > 0){
		addrec(p, p->line, p->nline, 0, 0);
		p->nline = 0;
	}
	p->lflag = l;
}

/* ---- reads and writes of the two sides ---- */

static int
readable(Pty *p)
{
	Rec *r;

	for(r = p->in; r != nil; r = r->next)
		if(r->eof || r->n > r->off)
			return 1;
	return 0;
}

static void
slaveread(Pty *p, Req *r)
{
	long n, m, want;
	Rec *q;
	int done;

	want = r->ifcall.count;
	n = 0;
	while(n < want && (q = p->in) != nil){
		if(q->eof){
			if(n == 0)
				freerec(p);	/* EOF: this read is empty */
			break;
		}
		m = q->n - q->off;
		if(m > want - n)
			m = want - n;
		memmove(r->ofcall.data + n, q->data + q->off, m);
		q->off += m;
		p->nin -= m;
		n += m;
		if(q->off < q->n)
			break;	/* the read is full */
		done = q->done;
		if(!done && q->next == nil)
			break;	/* raw input still open for more */
		freerec(p);
		if(done && (p->lflag & ICANON))
			break;	/* canonical: a line per read */
	}
	r->ofcall.count = n;
	respond(r, nil);
}

static void
masterread(Pty *p, Req *r)
{
	long n;

	n = r->ifcall.count;
	if(n > p->nout)
		n = p->nout;
	memmove(r->ofcall.data, p->out, n);
	memmove(p->out, p->out + n, p->nout - n);
	p->nout -= n;
	r->ofcall.count = n;
	respond(r, nil);
}

static void
slavewrite(Pty *p, Req *r)
{
	output(p, r->ifcall.data, r->ifcall.count);
	r->ofcall.count = r->ifcall.count;
	respond(r, nil);
}

static void
masterwrite(Pty *p, Req *r)
{
	long i;

	for(i = 0; i < r->ifcall.count; i++)
		inbyte(p, (uchar)r->ifcall.data[i]);
	r->ofcall.count = r->ifcall.count;
	respond(r, nil);
}

/* answer what waits and can be answered now */
static void
kick(Pty *p)
{
	Req *r;
	int again;

	do{
		again = 0;
		while(p->mwrites != nil && p->nin < Maxin){
			masterwrite(p, dequeue(&p->mwrites));
			again = 1;
		}
		while(p->sreads != nil){
			if(readable(p))
				slaveread(p, dequeue(&p->sreads));
			else if(p->nmaster == 0 || !(p->lflag & ICANON) && p->cc[VMIN] == 0){
				/* hung up: end of file; VMIN 0 (and VTIME 0): what there is */
				r = dequeue(&p->sreads);
				r->ofcall.count = 0;
				respond(r, nil);
			}else
				break;
			again = 1;
		}
		while(p->swrites != nil && p->nout < Maxout){
			if(p->nmaster == 0){
				respond(dequeue(&p->swrites), "hangup");
				continue;
			}
			slavewrite(p, dequeue(&p->swrites));
			again = 1;
		}
		while(p->mreads != nil){
			if(p->nout > 0)
				masterread(p, dequeue(&p->mreads));
			else if(p->hadslave && p->nslave == 0){
				r = dequeue(&p->mreads);
				r->ofcall.count = 0;	/* the slave is gone: end of file */
				respond(r, nil);
			}else
				break;
			again = 1;
		}
	}while(again);
}

/* ---- control: ptyctlN ---- */

static char*
ctltext(Pty *p)
{
	char *s;
	int i, n;

	s = emalloc9p(256);
	n = snprint(s, 256, "IOR %4.4lux %4.4lux %4.4lux %4.4lux ", p->iflag, p->oflag, p->cflag, p->lflag);
	for(i = 0; i < NCCS; i++)
		n += snprint(s+n, 256-n, "%2.2ux ", p->cc[i]);
	snprint(s+n, 256-n, "%d %d %d %d %d\n", p->pgrp, p->row, p->col, p->xpix, p->ypix);
	return s;
}

static char*
ctlwrite(Pty *p, char *s)
{
	char *f[4+NCCS+1];
	int n, i;
	ulong l;

	n = tokenize(s, f, nelem(f));
	if(n < 1 || strcmp(f[0], "IOW") != 0)
		return "bad control message";
	if(n == 3 && strcmp(f[1], "note") == 0){
		p->pgrp = atoi(f[2]);
		return nil;
	}
	if(n == 6 && strcmp(f[1], "winsize") == 0){
		p->row = atoi(f[2]);
		p->col = atoi(f[3]);
		p->xpix = atoi(f[4]);
		p->ypix = atoi(f[5]);
		return nil;
	}
	if(n < 5)
		return "bad control message";
	p->iflag = strtoul(f[1], nil, 16);
	p->oflag = strtoul(f[2], nil, 16);
	p->cflag = strtoul(f[3], nil, 16);
	l = strtoul(f[4], nil, 16);
	for(i = 0; i < NCCS && 5+i < n; i++)
		p->cc[i] = strtoul(f[5+i], nil, 16);
	setlflag(p, l);
	return nil;
}

/* consctl: rawon, rawoff (as cons(3)); others are taken and ignored */
static void
consctl(Pty *p, Fid *fid, char *s)
{
	if(strncmp(s, "rawon", 5) == 0){
		if(!p->raw){
			p->cookedl = p->lflag;
			setlflag(p, p->lflag & ~(ICANON|ECHO));
		}
		if(fid->aux == nil){
			fid->aux = (void*)1;
			p->raw++;
		}
	}else if(strncmp(s, "rawoff", 6) == 0){
		if(fid->aux != nil){
			fid->aux = nil;
			if(--p->raw == 0)
				setlflag(p, p->cookedl);
		}
	}
}

/* ---- the file tree ---- */

static Pty*
newpty(char *uid)
{
	Pty *p;
	char name[32];
	int n;

	p = emalloc9p(sizeof *p);
	defaults(p);
	n = p->n = nextn++;
	snprint(name, sizeof name, "ttym%d", n);
	p->master = createfile(tree->root, name, uid, 0620, p);
	snprint(name, sizeof name, "ptty%d", n);
	p->slave = createfile(tree->root, name, uid, 0620, p);
	snprint(name, sizeof name, "pconsctl%d", n);
	p->ctl = createfile(tree->root, name, uid, 0220, p);
	snprint(name, sizeof name, "ptyctl%d", n);
	p->ptyctl = createfile(tree->root, name, uid, 0660, p);
	if(p->master == nil || p->slave == nil || p->ctl == nil || p->ptyctl == nil)
		sysfatal("createfile: %r");
	p->ptyctl->qid.path = ptyqid(n, Qptyctl);
	p->master->qid.path = ptyqid(n, Qmaster);
	p->slave->qid.path = ptyqid(n, Qslave);
	p->ctl->qid.path = ptyqid(n, Qctl);
	return p;
}

/* nobody holds it: its files go */
static void
gone(Pty *p)
{
	if(p->nmaster > 0 || p->nslave > 0)
		return;
	flushinput(p);
	free(p->out);
	p->out = nil;
	removefile(p->master);
	removefile(p->slave);
	removefile(p->ctl);
	removefile(p->ptyctl);
}

static void
fsopen(Req *r)
{
	Pty *p;
	File *f;

	f = r->fid->file;
	if(f == nil || f == tree->root){
		respond(r, nil);
		return;
	}
	if(qtype(f) == Qclone){
		p = newpty(r->fid->uid);
		/* the fid is the master now */
		closefile(f);
		incref(p->master);
		r->fid->file = p->master;
		r->fid->qid = p->master->qid;
		r->ofcall.qid = p->master->qid;
		p->nmaster++;
		respond(r, nil);
		return;
	}
	p = f->aux;
	switch(qtype(f)){
	case Qmaster:
		p->nmaster++;
		break;
	case Qslave:
		p->nslave++;
		p->hadslave = 1;
		break;
	}
	respond(r, nil);
}

static void
fsread(Req *r)
{
	Pty *p;
	File *f;
	char *s;

	f = r->fid->file;
	p = f->aux;
	if(qtype(f) == Qptyctl){
		s = ctltext(p);
		readstr(r, s);
		free(s);
		respond(r, nil);
		return;
	}
	switch(qtype(f)){
	case Qmaster:
		enqueue(&p->mreads, r);
		break;
	case Qslave:
		enqueue(&p->sreads, r);
		break;
	default:
		respond(r, "permission denied");
		return;
	}
	kick(p);
}

static void
fswrite(Req *r)
{
	Pty *p;
	File *f;
	char *s, *e;

	f = r->fid->file;
	p = f->aux;
	if(qtype(f) == Qctl || qtype(f) == Qptyctl){
		s = emalloc9p(r->ifcall.count + 1);
		memmove(s, r->ifcall.data, r->ifcall.count);
		s[r->ifcall.count] = 0;
		if(qtype(f) == Qctl){
			consctl(p, r->fid, s);
			e = nil;
		}else
			e = ctlwrite(p, s);
		free(s);
		if(e == nil)
			r->ofcall.count = r->ifcall.count;
		respond(r, e);
		kick(p);
		return;
	}
	switch(qtype(f)){
	case Qmaster:
		enqueue(&p->mwrites, r);
		break;
	case Qslave:
		enqueue(&p->swrites, r);
		break;
	default:
		respond(r, "permission denied");
		return;
	}
	kick(p);
}

static void
fsflush(Req *r)
{
	Req *o;
	Pty *p;
	File *f;

	o = r->oldreq;
	f = o->fid->file;
	if(f != nil && f != tree->root && qtype(f) != Qclone){
		p = f->aux;
		if(unqueue(&p->sreads, o) || unqueue(&p->mreads, o) || unqueue(&p->swrites, o) || unqueue(&p->mwrites, o))
			respond(o, "interrupted");
	}
	respond(r, nil);
}

/* a fid closed: the side's count down, raw mode it set off; the master's last: hangup */
static void
fsdestroyfid(Fid *fid)
{
	Pty *p;
	File *f;

	f = fid->file;
	if(f == nil || f == tree->root || fid->omode == -1 || qtype(f) == Qclone)
		return;
	p = f->aux;
	switch(qtype(f)){
	case Qmaster:
		if(--p->nmaster == 0){
			kick(p);	/* readers get end of file before the note */
			signote(p, "hangup");
		}
		break;
	case Qslave:
		if(--p->nslave == 0)
			kick(p);
		break;
	case Qctl:
		if(fid->aux != nil){
			fid->aux = nil;
			if(--p->raw == 0)
				setlflag(p, p->cookedl);
		}
		break;
	}
	gone(p);
}

static Srv fs = {
	.open = fsopen,
	.read = fsread,
	.write = fswrite,
	.flush = fsflush,
	.destroyfid = fsdestroyfid,
};

/*
 * what the program that started it had open (APE's mkserver forks with
 * RFFDG: a terminal's window, its sockets, the pipe its output goes to,
 * maybe a pty of this ptyfs) is not ptyfs': all of it closed (#d lists
 * it), so that no reader waits for its end and no pty stays held;
 * 0, 1, 2 /dev/null (with -D standard error stays, for the messages)
 */
static void
closefds(int keeperr)
{
	Dir *d;
	int fd, i, n, k;

	if((fd = open("#d", OREAD)) < 0)
		return;
	n = dirreadall(fd, &d);
	close(fd);
	for(i = 0; i < n; i++){
		if(strstr(d[i].name, "ctl") != nil)
			continue;
		k = atoi(d[i].name);
		if(k != fd && !(keeperr && k == 2))
			close(k);
	}
	free(d);
	if((fd = open("/dev/null", ORDWR)) < 0)
		return;
	if(fd != 0)
		dup(fd, 0);
	if(fd != 1)
		dup(fd, 1);
	if(!keeperr && fd != 2)
		dup(fd, 2);
	if(fd > 2)
		close(fd);
}

static void
usage(void)
{
	fprint(2, "usage: %s [-D] [-s srvname]\n", argv0);
	exits("usage");
}

void
main(int argc, char **argv)
{
	char *srv, path[64];
	File *c;

	srv = "ptyfs";
	ARGBEGIN{
	case 'D':
		chatty9p++;
		break;
	case 's':
		srv = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND
	if(argc != 0)
		usage();
	closefds(chatty9p > 0);

	tree = alloctree(nil, nil, DMDIR|0555, nil);
	fs.tree = tree;
	if((c = createfile(tree->root, "ptyclone", nil, 0666, nil)) == nil)
		sysfatal("createfile: %r");
	c->qid.path = Qclone;
	closefile(c);

	/* a stale one of a ptyfs gone (APE's mkserver removes it too) */
	snprint(path, sizeof path, "/srv/%s", srv);
	remove(path);
	postsrv(&fs, srv);
	exits(nil);
}
