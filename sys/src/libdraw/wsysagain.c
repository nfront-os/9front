/*
 * 9front-ng (../9front-ui PLAN stage 7): a window that outlives its
 * wsys(4).  When reads of the window's files fail and the wsys that
 * $wsys names (/srv/wsys.user.pid) is gone (killed, crashed; not when
 * it ends in order, that posts hangup), wsysagain waits for the user's
 * next wsys and makes the window there, as newwindow does: mounted with
 * the attach name new, bound before /dev, $wsys set to it.  The caller
 * then opens the window's files again and draws everything anew.
 * As p9win's wsys.rs (../9front-rust) does for Rust programs.
 * $wsys and $user are read (and $wsys written) as /env files: APE's
 * getenv keeps the environment of the program's start, and its result
 * is not the caller's to free.
 */
#include <u.h>
#include <libc.h>
#include <draw.h>
#ifdef _POSIX_SOURCE
#include <errno.h>
#endif

static int
readenv(char *name, char *buf, int n)
{
	char path[64];
	int fd, m;

	snprint(path, sizeof path, "/env/%s", name);
	if((fd = open(path, OREAD)) < 0)
		return -1;
	m = read(fd, buf, n-1);
	close(fd);
	if(m <= 0)
		return -1;
	buf[m] = 0;
	return m;
}

/* the pid in /srv/wsys.user.pid, 0 if none */
static int
srvpid(char *srv)
{
	char *p;

	if((p = strrchr(srv, '.')) == nil)
		return 0;
	return atoi(p+1);
}

static int
alive(int pid)
{
	char path[32];

	snprint(path, sizeof path, "/proc/%d", pid);
	return access(path, AEXIST) == 0;
}

/*
 * the window's files are on a mount that hung up: its wsys is gone (its
 * procs may still be going: a dying wsys takes a while on a GPU), not
 * just the window (then wsys answers otherwise)
 */
static int
hungup(void)
{
	int fd;

	if((fd = open("/mnt/wsys/winname", OREAD)) >= 0){
		close(fd);
		return 0;
	}
#ifdef _POSIX_SOURCE
	/* APE: "i/o on hungup channel" is ESHUTDOWN (its errstr is taken for errno) */
	return errno == ESHUTDOWN;
#else
	{
		char err[ERRMAX];

		rerrstr(err, sizeof err);
		return strstr(err, "hungup") != nil;
	}
#endif
}

/* the window's wsys is gone; asked after a read of its files failed */
int
wsyslost(void)
{
	char buf[256];
	int pid;

	/* only wsys' windows (rio's: /srv/rio.user.pid go with it, as they did) */
	if(readenv("wsys", buf, sizeof buf) < 0 || strncmp(buf, "/srv/wsys.", 10) != 0 || (pid = srvpid(buf)) <= 0)
		return 0;
	return !alive(pid) || hungup();
}

/* the user's newest wsys still running: its /srv entry into buf */
static int
newest(char *buf, int n)
{
	Dir *d;
	char user[64], prefix[80];
	int fd, i, nd, pid, best, l;

	if(readenv("user", user, sizeof user) < 0)
		strcpy(user, "none");
	snprint(prefix, sizeof prefix, "wsys.%s.", user);
	l = strlen(prefix);
	if((fd = open("/srv", OREAD)) < 0)
		return -1;
	nd = dirreadall(fd, &d);
	close(fd);
	best = 0;
	for(i = 0; i < nd; i++){
		if(strncmp(d[i].name, prefix, l) != 0)
			continue;
		pid = atoi(d[i].name+l);
		if(pid > best && alive(pid)){
			best = pid;
			snprint(buf, n, "/srv/%s", d[i].name);
		}
	}
	free(d);
	return best > 0 ? 0 : -1;
}

/*
 * $wsys names a wsys that runs and the window's mount is not hung up: the
 * window is there (made again by another).  Not only its process: a killed
 * wsys' procs stay a while after its mount hung up
 */
static int
wsysalive(void)
{
	char buf[256];
	int pid;

	return readenv("wsys", buf, sizeof buf) > 0 && (pid = srvpid(buf)) > 0 && alive(pid) && !hungup();
}

/* how long to wait for the next wsys: $wsyswait seconds (0: no end), else 5 minutes */
static long
waitsecs(void)
{
	char buf[32];

	if(readenv("wsyswait", buf, sizeof buf) > 0)
		return atol(buf);
	return 300;
}

static int
again(char *rect)
{
	char srv[256], spec[128];
	int fd;
	long secs, waited;

	if(rect != nil)
		snprint(spec, sizeof spec, "new -r %s -pid %d", rect, getpid());
	else
		snprint(spec, sizeof spec, "new -pid %d", getpid());
	secs = waitsecs();
	for(waited = 0; secs <= 0 || waited < secs; waited++){
		if(wsysalive())
			return 0;
		if(newest(srv, sizeof srv) == 0 && (fd = open(srv, ORDWR)) >= 0){
			/* the dead one's files out of /dev (bound before it), the new window's in */
			unmount("/mnt/wsys", "/dev");
			if(mount(fd, -1, "/mnt/wsys", MREPL, spec) >= 0){
				if(bind("/mnt/wsys", "/dev", MBEFORE) < 0)
					return -1;
				if((fd = create("/env/wsys", OWRITE, 0664)) >= 0){
					write(fd, srv, strlen(srv));
					close(fd);
				}
				return 0;
			}
			close(fd);
		}
		sleep(1000);
	}
	return -1;	/* none came: the program ends, as under rio */
}

/*
 * the window again in the next wsys, at rect ("x0 y0 x1 y1", where it was;
 * nil: where wsys puts it); waits for that wsys, a look a second, for
 * $wsyswait seconds (0: no end; 5 minutes without it), then -1.  More
 * than one may ask (a toolkit's mouse reader, Mesa's WSI): the first makes
 * the window, for the others $wsys runs again and they return
 */
static QLock againlk;

int
wsysagain(char *rect)
{
	int r;

	qlock(&againlk);
	r = again(rect);
	qunlock(&againlk);
	return r;
}
