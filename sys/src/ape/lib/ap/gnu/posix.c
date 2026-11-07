/*
 * What C libraries (libgit2, sqlite, curl, LLVM) want of POSIX beyond APE: over
 * APE's own. fsync and getsockopt under link names of their own (<unistd.h>,
 * <sys/socket.h>): APE's stay as they are for the tree's programs.
 */
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <utime.h>
#include <pwd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

/* a write is done when the file server answers it (APE's fsync: EINVAL) */
int
__apecompat_fsync(int fd)
{
	(void)fd;
	return 0;
}

int
fdatasync(int fd)
{
	(void)fd;
	return 0;
}

/* note groups, not sessions */
pid_t
getsid(pid_t pid)
{
	return pid == 0 ? getpid() : pid;
}

pid_t
getpgid(pid_t pid)
{
	return pid == 0 ? getpid() : pid;
}

/* seconds: stat(5) has no more */
int
utimes(const char *path, const struct timeval *tv)
{
	struct utimbuf u, *up;

	up = NULL;
	if(tv != NULL){
		u.actime = tv[0].tv_sec;
		u.modtime = tv[1].tv_sec;
		up = &u;
	}
	return utime(path, up);
}

static int
pwcopy(struct passwd *p, struct passwd *pw, char *buf, size_t n, struct passwd **res)
{
	char *f[4], **d[4];
	size_t l;
	int i;

	*res = NULL;
	if(p == NULL)
		return errno ? errno : ENOENT;
	*pw = *p;
	f[0] = p->pw_name; d[0] = &pw->pw_name;
	f[1] = p->pw_dir; d[1] = &pw->pw_dir;
	f[2] = p->pw_shell; d[2] = &pw->pw_shell;
	f[3] = NULL;
	for(i = 0; f[i] != NULL; i++){
		l = strlen(f[i]) + 1;
		if(l > n)
			return ERANGE;
		memcpy(buf, f[i], l);
		*d[i] = buf;
		buf += l;
		n -= l;
	}
	*res = pw;
	return 0;
}

int
getpwuid_r(uid_t uid, struct passwd *pw, char *buf, size_t n, struct passwd **res)
{
	return pwcopy(getpwuid(uid), pw, buf, n, res);
}

int
getpwnam_r(const char *name, struct passwd *pw, char *buf, size_t n, struct passwd **res)
{
	return pwcopy(getpwnam(name), pw, buf, n, res);
}

int
fchown(int fd, uid_t uid, gid_t gid)
{
	(void)fd; (void)uid; (void)gid;
	return 0;
}

/* the file at least off+len long: no blocks to reserve on Plan 9's file servers */
int
posix_fallocate(int fd, off_t off, off_t len)
{
	struct stat st;

	if(fstat(fd, &st) < 0)
		return errno;
	if(st.st_size < off + len && ftruncate(fd, off + len) < 0)
		return errno;
	return 0;
}

/*
 * APE's getsockopt fails without an errno (libbsd's socket.c): curl asks SO_ERROR
 * after connect, whose connect(2) is done or failed already.
 */
#define _BSD_EXTENSION
#include <sys/socket.h>

int
__apecompat_getsockopt(int fd, int level, int opt, void *val, int *len)
{
	(void)fd;
	if(level != SOL_SOCKET || val == NULL || len == NULL || *len < (int)sizeof(int)){
		errno = EINVAL;
		return -1;
	}
	switch(opt){
	case SO_ERROR:
		*(int *)val = 0;
		break;
	case SO_SNDBUF:
	case SO_RCVBUF:
		*(int *)val = 65536;
		break;
	case SO_PEERCRED:
		/* 9front-ng: a PF_UNIX peer: the same machine and user; its pid not known */
		if(*len < (int)sizeof(struct ucred)){
			errno = EINVAL;
			return -1;
		}
		((struct ucred *)val)->pid = 0;
		((struct ucred *)val)->uid = getuid();
		((struct ucred *)val)->gid = getgid();
		*len = sizeof(struct ucred);
		return 0;
	default:
		errno = ENOPROTOOPT;
		return -1;
	}
	*len = sizeof(int);
	return 0;
}

/*
 * Resource limits: Plan 9 has none to ask for. The stack: a Rust program's are
 * stack slots of std's (64 MB, std's __plan9_thread_spawn is there), a C
 * program's the kernel's stack segment or ap/gnu/pthread.c's (16 MB).
 */
#include <sys/resource.h>

extern void *__plan9_thread_spawn(size_t, void (*)(void *), void *) __attribute__((weak));

int
getrlimit(int r, struct rlimit *l)
{
	rlim_t stack;

	stack = __plan9_thread_spawn != NULL ? 64ULL << 20 : 16ULL << 20;
	l->rlim_cur = l->rlim_max = r == RLIMIT_STACK ? stack : RLIM_INFINITY;
	return 0;
}

int
setrlimit(int r, const struct rlimit *l)
{
	(void)r; (void)l;
	return 0;
}

/* Plan 9's file servers tell no space: as much as there is room to count */
#include <sys/statvfs.h>

static void
fakevfs(struct statvfs *s)
{
	memset(s, 0, sizeof *s);
	s->f_bsize = s->f_frsize = 8192;
	s->f_blocks = s->f_bfree = s->f_bavail = 1ULL << 32;
	s->f_files = s->f_ffree = s->f_favail = 1ULL << 32;
	s->f_namemax = 255;
}

int
statvfs(const char *path, struct statvfs *s)
{
	struct stat st;

	if(stat(path, &st) < 0)
		return -1;
	fakevfs(s);
	return 0;
}

int
fstatvfs(int fd, struct statvfs *s)
{
	struct stat st;

	if(fstat(fd, &st) < 0)
		return -1;
	fakevfs(s);
	return 0;
}

#include <signal.h>
#include <stdio.h>

char *
strsignal(int sig)
{
	static char buf[32];

	snprintf(buf, sizeof buf, "signal %d", sig);
	return buf;
}
