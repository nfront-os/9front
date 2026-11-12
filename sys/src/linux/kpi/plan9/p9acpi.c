/*
 * SPDX-License-Identifier: MIT
 *
 * LinuxKPI for 9front, the Plan 9 side: ACPI for the drivers' methods
 * (_DSM, AMD's ATIF and ATCS, _ATRM, _DDC) through aux/acpi's aml file:
 * the machine has one AML interpreter (two would not know each other's
 * mutexes and embedded controller transactions).  aux/acpi is found as
 * /dev/aml or /srv/acpi, or started (it posts /srv/acpi; a second one
 * refuses).  A node is the path of a name (\_SB.PCI0.GFX0), kept for
 * good, so that the same name is the same pointer.  The requests and
 * the values' encoding: aux/acpi.c.
 */
#include <u.h>
#include <libc.h>
#include "../include/kpi/p9.h"

typedef struct Path Path;
struct Path {
	Path	*next;
	char	s[];
};

static QLock lk;
static int state;	/* 0: not tried, 1: connected, -1: no ACPI */
static int amlfd = -1;
static Path *paths;
static char root[] = "\\";

/* the one copy of path s; lk held */
static char*
intern(char *s)
{
	Path *p;

	if(s == nil)
		return nil;
	if(strcmp(s, "\\") == 0)
		return root;
	for(p = paths; p != nil; p = p->next)
		if(strcmp(p->s, s) == 0)
			return p->s;
	if((p = malloc(sizeof(*p) + strlen(s) + 1)) == nil)
		return nil;
	strcpy(p->s, s);
	p->next = paths;
	paths = p;
	return p->s;
}

/* aux/acpi, started here if it does not run */
static void
startacpi(void)
{
	int i;

	switch(rfork(RFPROC|RFFDG|RFNAMEG|RFNOTEG|RFENVG|RFREND|RFNOWAIT)){
	case -1:
		return;
	case 0:
		execl("/bin/aux/acpi", "acpi", "-s", "acpi", nil);
		_exits("exec aux/acpi");
	}
	for(i = 0; i < 100 && access("/srv/acpi", AEXIST) < 0; i++)
		sleep(100);
}

/* lk held */
static int
connect(void)
{
	int fd;

	if(state != 0)
		return state;
	state = -1;
	if(access("#P/acpitbls", AREAD) < 0)
		return -1;
	if((amlfd = open("/dev/aml", ORDWR)) < 0){
		if(access("/srv/acpi", AEXIST) < 0)
			startacpi();
		if((fd = open("/srv/acpi", ORDWR)) < 0){
			fprint(2, "kpi: acpi: no aux/acpi: %r\n");
			return -1;
		}
		if(mount(fd, -1, "/dev", MAFTER, "") < 0){
			fprint(2, "kpi: acpi: mount /srv/acpi: %r\n");
			close(fd);
			return -1;
		}
		if((amlfd = open("/dev/aml", ORDWR)) < 0){
			fprint(2, "kpi: acpi: /dev/aml: %r (an old aux/acpi?)\n");
			return -1;
		}
	}
	state = 1;
	return 1;
}

/* a request, its answer (malloc'd) or nil; lk held */
static char*
request(char *req)
{
	char *r;
	long n, m;

	if(connect() <= 0)
		return nil;
	if(write(amlfd, req, strlen(req)) != strlen(req))
		return nil;
	n = 0;
	r = nil;
	for(;;){
		if((r = realloc(r, n + 8192 + 1)) == nil)
			return nil;
		if((m = pread(amlfd, r + n, 8192, n)) <= 0)
			break;
		n += m;
	}
	r[n] = 0;
	return r;
}

int
p9_acpi_ok(void)
{
	int r;

	qlock(&lk);
	r = connect();
	qunlock(&lk);
	return r > 0 ? 0 : -1;
}

/* a request whose answer is a path */
static void*
pathreq(char *req)
{
	char *r, *x;

	if(req == nil)
		return nil;
	qlock(&lk);
	r = request(req);
	x = intern(r);
	qunlock(&lk);
	free(r);
	free(req);
	return x;
}

void*
p9_acpi_walk(void *node, const char *path)
{
	if(path == nil || strpbrk(path, " \t\n") != nil)
		return nil;
	return pathreq(smprint("walk %s %s", node != nil ? (char*)node : "-", path));
}

void*
p9_acpi_parent(void *node)
{
	if(node == nil || node == root)
		return nil;
	return pathreq(smprint("parent %s", (char*)node));
}

int
p9_acpi_name(void *node, char *buf, int n)
{
	snprint(buf, n, "%s", node != nil ? (char*)node : root);
	return 0;
}

void*
p9_acpi_pcidev(int bus, int dev, int fn)
{
	return pathreq(smprint("pci %d %d %d", bus, dev, fn));
}

int
p9_acpi_hid(void *node, char *buf, int n)
{
	char *r, *q;

	if(node == nil || (q = smprint("hid %s", (char*)node)) == nil)
		return -1;
	qlock(&lk);
	r = request(q);
	qunlock(&lk);
	free(q);
	if(r == nil)
		return -1;
	snprint(buf, n, "%s", r);
	free(r);
	return 0;
}

int
p9_acpi_devices(int (*fn)(void *node, void *arg), void *arg)
{
	char *r, *p, *e, *sp;
	void *x;
	int v;

	qlock(&lk);
	r = request("devices");
	qunlock(&lk);
	if(r == nil)
		return -1;
	v = 0;
	for(p = r; *p != 0 && v == 0; p = e){
		if((e = strchr(p, '\n')) == nil)
			e = p + strlen(p);
		else
			*e++ = 0;
		if((sp = strchr(p, ' ')) != nil)
			*sp = 0;
		qlock(&lk);
		x = intern(p);
		qunlock(&lk);
		if(x != nil)
			v = fn(x, arg);
	}
	free(r);
	return v;
}

/* our value in aux/acpi's encoding */
static int
encval(Fmt *f, const struct p9_acpival *v, int depth)
{
	uchar *b;
	int i;

	if(depth > 8)
		return -1;
	switch(v->type){
	case P9_ACPI_INTEGER:
		fmtprint(f, "i%llux;", v->integer);
		return 0;
	case P9_ACPI_STRING:
		fmtprint(f, "s%d:%.*s", v->len, v->len, (char*)v->ptr);
		return 0;
	case P9_ACPI_BUFFER:
		b = v->ptr;
		fmtprint(f, "b%d:", v->len);
		for(i = 0; i < v->len; i++)
			fmtprint(f, "%.2ux", b[i]);
		return 0;
	case P9_ACPI_PACKAGE:
		fmtprint(f, "p%d:", v->len);
		for(i = 0; i < v->len; i++)
			if(encval(f, &v->elem[i], depth+1) < 0)
				return -1;
		return 0;
	case P9_ACPI_REFERENCE:
		if(v->node == nil)
			return -1;
		fmtprint(f, "r%d:%s", (int)strlen(v->node), (char*)v->node);
		return 0;
	}
	return -1;
}

static int
hexval(int c)
{
	if(c >= '0' && c <= '9')
		return c - '0';
	if(c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if(c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

/* aux/acpi's encoding as our value (into v); 0 or -1; lk held (intern) */
static int
decval(char **pp, char *e, struct p9_acpival *v, int depth)
{
	char *p, *t;
	uchar *b;
	int i, n, h, l;

	memset(v, 0, sizeof(*v));
	p = *pp;
	if(p >= e || depth > 8)
		return -1;
	if(*p == '-'){
		*pp = p + 1;
		return 0;	/* none: ANY */
	}
	if(*p == 'i'){
		v->type = P9_ACPI_INTEGER;
		v->integer = strtoull(p+1, &t, 16);
		if(*t != ';')
			return -1;
		*pp = t + 1;
		return 0;
	}
	n = strtol(p+1, &t, 10);
	if(*t != ':' || n < 0 || n > e - t)
		return -1;
	t++;
	switch(*p){
	case 's':
		v->type = P9_ACPI_STRING;
		v->len = n;
		if((v->ptr = malloc(n + 1)) == nil)
			return -1;
		memmove(v->ptr, t, n);
		((char*)v->ptr)[n] = 0;
		*pp = t + n;
		return 0;
	case 'b':
		if(2*n > e - t)
			return -1;
		v->type = P9_ACPI_BUFFER;
		v->len = n;
		if((v->ptr = malloc(n ? n : 1)) == nil)
			return -1;
		b = v->ptr;
		for(i = 0; i < n; i++){
			if((h = hexval(t[2*i])) < 0 || (l = hexval(t[2*i+1])) < 0)
				return -1;
			b[i] = h<<4 | l;
		}
		*pp = t + 2*n;
		return 0;
	case 'p':
		v->type = P9_ACPI_PACKAGE;
		v->len = n;
		if((v->elem = mallocz((n ? n : 1) * sizeof(*v->elem), 1)) == nil)
			return -1;
		*pp = t;
		for(i = 0; i < n; i++)
			if(decval(pp, e, &v->elem[i], depth+1) < 0)
				return -1;
		return 0;
	case 'r':
		v->type = P9_ACPI_REFERENCE;
		p = smprint("%.*s", n, t);
		v->node = intern(p);
		free(p);
		*pp = t + n;
		return v->node != nil ? 0 : -1;
	}
	return -1;
}

static void
freeval(struct p9_acpival *v)
{
	int i;

	if(v->type == P9_ACPI_PACKAGE && v->elem != nil){
		for(i = 0; i < v->len; i++)
			freeval(&v->elem[i]);
		free(v->elem);
	}
	if(v->type == P9_ACPI_STRING || v->type == P9_ACPI_BUFFER)
		free(v->ptr);
}

void
p9_acpi_free(struct p9_acpival *v)
{
	if(v == nil)
		return;
	freeval(v);
	free(v);
}

int
p9_acpi_eval(void *node, int nargs, const struct p9_acpival *args, struct p9_acpival **ret)
{
	struct p9_acpival *v;
	char *q, *r, *p;
	Fmt f;
	int i;

	if(ret != nil)
		*ret = nil;
	if(node == nil || nargs < 0 || nargs > 7)
		return -1;
	fmtstrinit(&f);
	fmtprint(&f, "eval %s %d ", (char*)node, nargs);
	for(i = 0; i < nargs; i++)
		if(encval(&f, &args[i], 0) < 0){
			free(fmtstrflush(&f));
			return -1;
		}
	if((q = fmtstrflush(&f)) == nil)
		return -1;
	qlock(&lk);
	r = request(q);
	free(q);
	if(r == nil){
		qunlock(&lk);
		return -1;
	}
	if(ret != nil){
		p = r;
		if((v = mallocz(sizeof(*v), 1)) == nil || decval(&p, r + strlen(r), v, 0) < 0){
			if(v != nil)
				p9_acpi_free(v);
			qunlock(&lk);
			free(r);
			return -1;
		}
		*ret = v;
	}
	qunlock(&lk);
	free(r);
	return 0;
}
