/*
 * ui(2): a client of ui(4).  The files are mounted (uiopen: ui's /srv
 * entry; uinew: ui started in a new window first), a transaction is one
 * write of ctl, an event one line of event.
 */
#include <u.h>
#include <libc.h>
#include <ui.h>

enum {
	Waitms = 10*1000,	/* for a new ui's /srv entry */
};

Ui*
uiopen(char *srv, char *mtpt)
{
	Ui *u;
	char *s, *p;
	int fd;

	quotefmtinstall();	/* uiset's and uinew's %q */
	if(mtpt == nil)
		mtpt = "/mnt/ui";
	s = smprint("/srv/%s", srv);
	fd = open(s, ORDWR);
	free(s);
	if(fd < 0)
		return nil;
	if(mount(fd, -1, mtpt, MREPL, "") < 0){
		close(fd);
		return nil;
	}
	u = mallocz(sizeof *u, 1);
	u->mtpt = strdup(mtpt);
	u->srv = strdup(srv);
	p = smprint("%s/ctl", mtpt);
	u->ctl = open(p, OWRITE);
	free(p);
	p = smprint("%s/event", mtpt);
	u->ev = open(p, OREAD);
	free(p);
	if(u->ctl < 0 || u->ev < 0){
		uiclose(u);
		return nil;
	}
	return u;
}

/* ui in a new window of rio or wsys (window(1)); its files then */
Ui*
uinew(char *title, char *mtpt)
{
	static int n;
	char *srv, *cmd, *s;
	Ui *u;
	int i, pid, w;

	srv = smprint("ui.%s.%d.%d", getuser(), getpid(), n++);
	cmd = smprint("/bin/ui -s %s", srv);
	switch(pid = fork()){
	case -1:
		free(srv);
		free(cmd);
		return nil;
	case 0:
		execl("/bin/window", "window", cmd, nil);
		_exits("window");
	}
	free(cmd);
	/* window(1) returns once the window is there; ui posts its files soon after */
	do
		w = waitpid();
	while(w != pid && w >= 0);
	s = smprint("/srv/%s", srv);
	for(i = 0; i < Waitms/100 && access(s, AEXIST) < 0; i++)
		sleep(100);
	free(s);
	u = uiopen(srv, mtpt);
	free(srv);
	if(u == nil){
		werrstr("ui: no new window's files: %r");
		return nil;
	}
	if(title != nil && uictl(u, "title %q", title) < 0){
		uiclose(u);
		return nil;
	}
	return u;
}

/* one transaction: the formatted text's lines, all or none */
int
uictl(Ui *u, char *fmt, ...)
{
	va_list arg;
	char *s;
	int n, r;

	va_start(arg, fmt);
	s = vsmprint(fmt, arg);
	va_end(arg);
	if(s == nil)
		return -1;
	n = strlen(s);
	r = write(u->ctl, s, n) == n ? 0 : -1;
	free(s);
	return r;
}

/*
 * a value as ui(4) reads it: as it is, else in rc's quotes, where \n is a
 * new line and \\ a backslash (a textarea's lines stay on one line)
 */
static char*
uiquote(char *v)
{
	char *q, *p;
	int n;

	n = strlen(v);
	if(n > 0 && v[0] != ' ' && v[0] != '\t' && v[n-1] != ' ' && v[n-1] != '\t' && v[0] != '\'' && strchr(v, '\n') == nil)
		return strdup(v);
	if((q = malloc(2*n + 3)) == nil)
		return nil;
	p = q;
	*p++ = '\'';
	for(; *v; v++)
		switch(*v){
		case '\'':	*p++ = '\''; *p++ = '\''; break;
		case '\\':	*p++ = '\\'; *p++ = '\\'; break;
		case '\n':	*p++ = '\\'; *p++ = 'n'; break;
		default:	*p++ = *v;
		}
	*p++ = '\'';
	*p = 0;
	return q;
}

/* a property, its value quoted as ui reads it */
int
uiset(Ui *u, char *id, char *key, char *value)
{
	char *q;
	int r;

	if((q = uiquote(value)) == nil)
		return -1;
	r = uictl(u, "set %s %s %s", id, key, q);
	free(q);
	return r;
}

/* the next event, waiting for one; -1 when ui is gone */
int
uievent(Ui *u, Uievent *e)
{
	char *nl, *f[3];
	int n, l;

	for(;;){
		if((nl = memchr(u->buf, '\n', u->nbuf)) != nil){
			l = nl - u->buf;
			n = l < sizeof e->line - 1 ? l : sizeof e->line - 1;
			memmove(e->line, u->buf, n);
			e->line[n] = 0;
			u->nbuf -= l+1;
			memmove(u->buf, nl+1, u->nbuf);
			break;
		}
		if(u->nbuf == sizeof u->buf)
			u->nbuf = 0;	/* no line in all of it: none to keep */
		n = read(u->ev, u->buf+u->nbuf, sizeof u->buf - u->nbuf);
		if(n <= 0)
			return -1;
		u->nbuf += n;
	}
	/* kind id rest */
	memset(f, 0, sizeof f);
	n = getfields(e->line, f, 3, 1, " ");
	e->kind = f[0];
	e->id = n > 1 ? f[1] : nil;
	e->arg = n > 2 ? f[2] : "";
	return 0;
}

void
uiclose(Ui *u)
{
	if(u == nil)
		return;
	if(u->ctl >= 0)
		close(u->ctl);
	if(u->ev >= 0)
		close(u->ev);
	if(u->mtpt != nil)
		unmount(nil, u->mtpt);
	free(u->mtpt);
	free(u->srv);
	free(u);
}
