/*
 * SPDX-License-Identifier: MIT
 *
 * POSIX functions Mesa uses that 9front's APE and the toolchain's
 * compat library lack (headers in ../include).
 */
#define _POSIX_SOURCE
#define _BSD_EXTENSION
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <poll.h>
#include <syslog.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <sys/resource.h>

#undef sysconf
extern long sysconf(int);
extern char **environ;

int
setenv(const char *name, const char *val, int overwrite)
{
	char *s;
	size_t n;

	if(name == NULL || *name == 0 || strchr(name, '=') != NULL){
		errno = EINVAL;
		return -1;
	}
	if(!overwrite && getenv(name) != NULL)
		return 0;
	n = strlen(name) + 1 + strlen(val) + 1;
	if((s = malloc(n)) == NULL)
		return -1;
	strcpy(s, name);
	strcat(s, "=");
	strcat(s, val);
	return putenv(s);
}

int
unsetenv(const char *name)
{
	char **p, **q;
	size_t n;

	if(name == NULL || *name == 0 || strchr(name, '=') != NULL){
		errno = EINVAL;
		return -1;
	}
	n = strlen(name);
	for(p = q = environ; *p != NULL; p++)
		if(strncmp(*p, name, n) != 0 || (*p)[n] != '=')
			*q++ = *p;
	*q = NULL;
	return 0;
}

/* a value of the form "N name" from /dev/swap */
static long long
swapval(char *name)
{
	char buf[1024], *s, *e;
	int fd, n;
	size_t l;

	if((fd = open("/dev/swap", O_RDONLY)) < 0)
		return -1;
	n = read(fd, buf, sizeof buf-1);
	close(fd);
	if(n <= 0)
		return -1;
	buf[n] = 0;
	l = strlen(name);
	for(s = buf; *s; s = e){
		if((e = strchr(s, '\n')) != NULL)
			*e++ = 0;
		else
			e = s + strlen(s);
		n = strlen(s);
		if(n > l && strcmp(s+n-l, name) == 0 && s[n-l-1] == ' ')
			return strtoll(s, NULL, 0);
	}
	return -1;
}

long
mesa_plan9_sysconf(int name)
{
	long long mem, pg;

	switch(name){
	case 0x7001:	/* _SC_PHYS_PAGES */
		mem = swapval("memory");
		pg = swapval("pagesize");
		if(mem <= 0 || pg <= 0)
			return -1;
		return mem / pg;
	case 0x7002:	/* _SC_PAGESIZE */
		pg = swapval("pagesize");
		return pg > 0 ? pg : 4096;
	}
	return sysconf(name);
}

int
pthread_sigmask(int how, const sigset_t *set, sigset_t *old)
{
	return sigprocmask(how, set, old) < 0 ? errno : 0;
}

int
pthread_getcpuclockid(pthread_t t, clockid_t *id)
{
	(void)t;
	(void)id;
	return ENOSYS;
}

int
pthread_barrier_init(pthread_barrier_t *b, const pthread_barrierattr_t *a, unsigned count)
{
	(void)a;
	if(count == 0)
		return EINVAL;
	pthread_mutex_init(&b->m, NULL);
	pthread_cond_init(&b->c, NULL);
	b->count = count;
	b->waiting = 0;
	b->generation = 0;
	return 0;
}

int
pthread_barrier_destroy(pthread_barrier_t *b)
{
	pthread_cond_destroy(&b->c);
	pthread_mutex_destroy(&b->m);
	return 0;
}

int
pthread_barrier_wait(pthread_barrier_t *b)
{
	unsigned gen;

	pthread_mutex_lock(&b->m);
	gen = b->generation;
	if(++b->waiting == b->count){
		b->waiting = 0;
		b->generation++;
		pthread_cond_broadcast(&b->c);
		pthread_mutex_unlock(&b->m);
		return PTHREAD_BARRIER_SERIAL_THREAD;
	}
	while(gen == b->generation)
		pthread_cond_wait(&b->c, &b->m);
	pthread_mutex_unlock(&b->m);
	return 0;
}

int
flock(int fd, int op)
{
	(void)fd;
	(void)op;
	return 0;
}

int
poll(struct pollfd *fds, nfds_t n, int timeout)
{
	(void)fds;
	(void)n;
	(void)timeout;
	errno = ENOSYS;
	return -1;
}

static const char *logident = "mesa";

void
openlog(const char *ident, int opt, int fac)
{
	(void)opt;
	(void)fac;
	if(ident != NULL)
		logident = ident;
}

void
syslog(int pri, const char *fmt, ...)
{
	va_list arg;

	(void)pri;
	fprintf(stderr, "%s: ", logident);
	va_start(arg, fmt);
	vfprintf(stderr, fmt, arg);
	va_end(arg);
	fputc('\n', stderr);
}

void
closelog(void)
{
}

void*
mmap(void *a, size_t n, int prot, int flags, int fd, off_t off)
{
	(void)a;
	(void)n;
	(void)prot;
	(void)flags;
	(void)fd;
	(void)off;
	errno = ENOSYS;
	return MAP_FAILED;
}

int
munmap(void *a, size_t n)
{
	(void)a;
	(void)n;
	errno = EINVAL;
	return -1;
}

int
getpriority(int which, int who)
{
	(void)which;
	(void)who;
	errno = 0;
	return 0;
}

int
setpriority(int which, int who, int prio)
{
	(void)which;
	(void)who;
	(void)prio;
	return 0;
}

/* mkstemp with a suffix of n bytes after the XXXXXX */
int
mkstemps(char *tmpl, int n)
{
	char *x, save[64];
	size_t l;
	int fd;

	l = strlen(tmpl);
	if(n < 0 || (size_t)n >= sizeof save || l < (size_t)n + 6){
		errno = EINVAL;
		return -1;
	}
	x = tmpl + l - n;
	memcpy(save, x, n);
	*x = 0;
	fd = mkstemp(tmpl);
	memcpy(x, save, n);
	if(fd >= 0 && n > 0){
		char *old;

		/* mkstemp made the file without the suffix: rename it */
		if((old = strdup(tmpl)) == NULL){
			close(fd);
			return -1;
		}
		old[l - n] = 0;
		if(rename(old, tmpl) < 0){
			close(fd);
			remove(old);
			free(old);
			return -1;
		}
		free(old);
	}
	return fd;
}
