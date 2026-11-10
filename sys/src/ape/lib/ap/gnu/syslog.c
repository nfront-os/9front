/*
 * POSIX's syslog as Plan 9's syslog(2) does it: "sysname date message"
 * appended to /sys/log/ident (the log must exist: an administrator
 * makes it, as for Plan 9's programs), with %m for strerror(errno).
 * The log is opened at each message (LOG_NDELAY: at openlog too, kept),
 * so a log made later is used. ident defaults to the program's name.
 */
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

static pthread_mutex_t lk = PTHREAD_MUTEX_INITIALIZER;
static char ident[64];
static int opts;
static int mask = 0xff;
static int logfd = -1;


static void
defident(void)
{
	char buf[128], *p;
	int fd, n;

	if(ident[0] != 0)
		return;
	/* the program's name, as ps shows it */
	snprintf(buf, sizeof buf, "/proc/%d/args", getpid());
	if((fd = open(buf, O_RDONLY)) >= 0){
		n = read(fd, buf, sizeof buf - 1);
		close(fd);
		if(n > 0){
			buf[n] = 0;
			buf[strcspn(buf, " \t\n")] = 0;
			p = strrchr(buf, '/');
			snprintf(ident, sizeof ident, "%s", p != NULL ? p+1 : buf);
			return;
		}
	}
	snprintf(ident, sizeof ident, "syslog");
}

static int
openit(void)
{
	char path[96];

	if(logfd >= 0)
		return logfd;
	snprintf(path, sizeof path, "/sys/log/%s", ident);
	return open(path, O_WRONLY|O_APPEND);
}

void
openlog(const char *id, int o, int facility)
{
	(void)facility;
	pthread_mutex_lock(&lk);
	if(id != NULL)
		snprintf(ident, sizeof ident, "%s", id);
	defident();
	opts = o;
	if(logfd >= 0){
		close(logfd);
		logfd = -1;
	}
	if(o & LOG_NDELAY)
		logfd = openit();
	pthread_mutex_unlock(&lk);
}

void
closelog(void)
{
	pthread_mutex_lock(&lk);
	if(logfd >= 0)
		close(logfd);
	logfd = -1;
	pthread_mutex_unlock(&lk);
}

int
setlogmask(int m)
{
	int old;

	pthread_mutex_lock(&lk);
	old = mask;
	if(m != 0)
		mask = m;
	pthread_mutex_unlock(&lk);
	return old;
}

void
vsyslog(int pri, const char *fmt, va_list a)
{
	char msg[1024], f[512], sys[64], date[32], line[1200];
	const char *s;
	char *d, *e;
	time_t t;
	struct tm tm;
	int fd, n, err;

	err = errno;
	if((mask & LOG_MASK(LOG_PRI(pri))) == 0)
		return;
	/* %m: the error's text */
	d = f;
	e = f + sizeof f - 1;
	for(s = fmt; *s != 0 && d < e; s++){
		if(s[0] == '%' && s[1] == 'm'){
			d += snprintf(d, e - d, "%s", strerror(err));
			if(d > e)
				d = e;
			s++;
		}else if(s[0] == '%' && s[1] == '%'){
			*d++ = *s++;
			if(d < e)
				*d++ = *s;
		}else
			*d++ = *s;
	}
	*d = 0;
	vsnprintf(msg, sizeof msg, f, a);
	n = strlen(msg);
	while(n > 0 && msg[n-1] == '\n')
		msg[--n] = 0;

	pthread_mutex_lock(&lk);
	defident();
	sys[0] = 0;
	if((fd = open("/dev/sysname", O_RDONLY)) >= 0){
		n = read(fd, sys, sizeof sys - 1);
		sys[n > 0 ? n : 0] = 0;
		close(fd);
	}
	t = time(NULL);
	localtime_r(&t, &tm);
	strftime(date, sizeof date, "%b %e %H:%M:%S", &tm);
	if(opts & LOG_PID)
		n = snprintf(line, sizeof line, "%s %s %s[%d]: %s\n", sys, date, ident, getpid(), msg);
	else
		n = snprintf(line, sizeof line, "%s %s %s: %s\n", sys, date, ident, msg);
	if(n >= (int)sizeof line)
		n = sizeof line - 1;
	fd = openit();
	if(fd < 0 || write(fd, line, n) != n){
		if(opts & LOG_CONS){
			int c;

			if((c = open("/dev/cons", O_WRONLY)) >= 0){
				write(c, line, n);
				close(c);
			}
		}
	}
	if(fd >= 0 && fd != logfd)
		close(fd);
	pthread_mutex_unlock(&lk);
	if(opts & LOG_PERROR){
		if(opts & LOG_PID)
			fprintf(stderr, "%s[%d]: %s\n", ident, getpid(), msg);
		else
			fprintf(stderr, "%s: %s\n", ident, msg);
	}
	errno = err;
}

void
syslog(int pri, const char *fmt, ...)
{
	va_list a;

	va_start(a, fmt);
	vsyslog(pri, fmt, a);
	va_end(a);
}
