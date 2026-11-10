#ifndef	__UNISTD_H
#define	__UNISTD_H

#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port, for the gcc driver's programs: execv, execle, execve, execvp as POSIX (char
 * *const[]) and sysconf (ap/gnu/xsysconf.c): APE's declarations renamed while the
 * header is read */
#include <sys/types.h>
#define execv	__ape_execv
#define execle	__ape_execle
#define execve	__ape_execve
#define execvp	__ape_execvp
#define sysconf	__ape_sysconf
#endif
#ifndef _POSIX_SOURCE
#error  This header file is not defined in pure ANSI
#endif
#pragma lib "/$M/lib/ape/libap.a"

#define _POSIX_VERSION	199309L
#define _POSIX_ASYNC_IO -1
#define _POSIX_CHOWN_RESTRICTED 1
#define _POSIX_NO_TRUNC 1
#define _POSIX_PRIO_IO -1
#define _POSIX_SYNC_IO -1
#define _POSIX_VDISABLE -1

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif
#ifndef _SSIZE_T
#define _SSIZE_T
typedef long ssize_t;
#endif
#ifndef NULL
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif
#endif

/* access */
#define	R_OK		4
#define	W_OK		2
#define	X_OK		1
#define	F_OK		0	/* test for existence */

/* lockf */
#define	F_ULOCK		0	/* unlock a previously locked region */
#define	F_LOCK		1	/* lock a region for exclusive use */
#define	F_TLOCK		2	/* test and lock a region for exclusive use */
#define	F_TEST		3	/* test a region for a previous lock */

/* lseek */
#ifndef SEEK_SET		/* also defined in stdio.h */
#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2
#endif

/* sysconf argument */
#define	_SC_ARG_MAX		1	/* max chars in args to exec */
#define	_SC_CHILD_MAX		2	/* max child process per process */
#define	_SC_CLK_TCK		3	/* number of clock() units per second */
#define	_SC_NGROUPS_MAX		4	/* max supplementary groups per process */
#define	_SC_OPEN_MAX		5
#define _SC_STREAM_MAX		6
#define _SC_TZNAME_MAX		7
#define	_SC_JOB_CONTROL		8	/* posix job control */
#define	_SC_SAVED_IDS		9	/* saved suid/sgid per process */
#define	_SC_VERSION		10	/* this version */
#define _SC_LOGIN_NAME_MAX	11	/* max length of a login name */

/* pathconf argument */
#define _PC_LINK_MAX		1
#define _PC_MAX_CANON		2
#define _PC_MAX_INPUT		3
#define _PC_NAME_MAX		4
#define _PC_PATH_MAX		5
#define _PC_PIPE_BUF		6
#define _PC_CHOWN_RESTRICTED	7
#define _PC_NO_TRUNC		8
#define _PC_VDISABLE		9

/* standard filenos */
#define STDIN_FILENO		0
#define STDOUT_FILENO		1
#define STDERR_FILENO		2

#ifdef __cplusplus
extern "C" {
#endif

/* process primitives */
extern int execl(const char *, const char *, ...);
extern int execv(const char *, const char **);
extern int execle(const char *, const char *, const char *,  ...);
extern int execve(const char *, const char **, const char **);
extern int execlp(const char *, const char *, ...);
extern int execvp(const char *, const char **);
extern _Noreturn void _exit(int);
extern unsigned int alarm(unsigned int);
extern int pause(void);
extern unsigned int sleep(unsigned int);
#ifdef __TYPES_H
extern pid_t fork(void);
#endif

/* process environment */
extern char *getlogin(void);
extern char *cuserid(char *);
extern char *ttyname(int);
extern int isatty(int);
extern long sysconf(int);
#ifdef __TYPES_H
extern pid_t getpid(void);
extern pid_t getppid(void);
extern uid_t getuid(void);
extern uid_t geteuid(void);
extern gid_t getgid(void);
extern gid_t getegid(void);
extern int setuid(uid_t);
extern int setgid(gid_t);
extern int getgroups(int, gid_t *);
extern pid_t getpgrp(void);
extern int setpgid(pid_t, pid_t);
extern pid_t setsid(void);
#endif

/* files and directories */
extern int chdir(const char *);
extern int chroot(const char *);
extern int link(const char *, const char *);
extern char *getcwd(char *, size_t);
extern int unlink(const char *);
extern int rmdir(const char *);
extern int rename(const char *, const char *);
extern int access(const char *, int);
extern long pathconf(const char *, int);
extern long fpathconf(int, int);
#ifdef __TYPES_H
extern int chown(const char *, uid_t, gid_t);
#endif
/* symbolic links (POSIX; libbsd: 9front has none) */
#define __APE_POSIX_LINKS 1
#pragma lib "/$M/lib/ape/libbsd.a"
extern int symlink(const char *, const char *);
extern ssize_t readlink(const char *, char *, size_t);

/* input and output primitives */
extern int pipe(int *);
extern int dup(int);
extern int dup2(int, int);
extern int close(int);
extern ssize_t read(int, void *, size_t);
extern ssize_t write(int, const void *, size_t);
#ifdef __TYPES_H
extern int ftruncate(int, off_t);
extern off_t lseek(int, off_t, int);
#endif

/* device- and class-specific functions */
#ifdef __TYPES_H
extern pid_t tcgetpgrp(int);
extern int tcsetpgrp(int, pid_t);
#endif

#ifdef _REENTRANT_SOURCE
extern char *getlogin_r(char *, int);
#endif

/* berkeley specific functions */
#ifdef _BSD_EXTENSION
#include <bsd.h>
#endif

#ifdef __cplusplus
}
#endif


#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port: <unistd.h> as POSIX has it: getopt and mktemp (POSIX puts them here,
 * APE only in bsd.h; libbsd), truncate, pread, pwrite, usleep, sysconf with the
 * processors, the page size and the pages of memory (ap/gnu) */
#undef execv
#undef execle
#undef execve
#undef execvp
#undef sysconf

/* what APE has of POSIX's options (time.c, pthread.c); not
 * _POSIX_THREAD_SAFE_FUNCTIONS: APE's stdio has no locks */
#define _POSIX_TIMERS			200809L
#define _POSIX_MONOTONIC_CLOCK		200809L
#define _POSIX_CLOCK_SELECTION		200809L
#define _POSIX_THREADS			200809L
#define _POSIX_TIMEOUTS			200809L
#define _POSIX_READER_WRITER_LOCKS	200809L

#define _SC_PAGESIZE		30
#define _SC_PAGE_SIZE		_SC_PAGESIZE
#define _SC_NPROCESSORS_CONF	83
#define _SC_NPROCESSORS_ONLN	84
#define _SC_PHYS_PAGES		85
/* unknown to APE's sysconf: -1 */
#define _SC_GETPW_R_SIZE_MAX	1000
#define _SC_GETGR_R_SIZE_MAX	1001

#ifdef __cplusplus
extern "C" {
#endif
extern int	execv(const char *, char *const *);
extern int	execle(const char *, const char *, ...);
extern int	execve(const char *, char *const *, char *const *);
extern int	execvp(const char *, char *const *);
extern int	truncate(const char *, off_t);
extern ssize_t	pread(int, void *, size_t, off_t);
extern ssize_t	pwrite(int, const void *, size_t, off_t);
extern int	usleep(useconds_t);
/* ap/gnu/posix.c: a write is done when the file server answers it (fsync:
 * APE's fails);
 * note groups, not sessions (getsid, getpgid: the pid); Plan 9 gives files away
 * only through wstat by their owner (fchown: no change, no error); libbsd's
 * gethostname */
extern int	fsync(int) __asm__("__apecompat_fsync");
extern int	fdatasync(int);
extern pid_t	getsid(pid_t);
extern pid_t	getpgid(pid_t);
extern int	fchown(int, uid_t, gid_t);
extern int	gethostname(char *, int);
extern long	sysconf(int) __asm__("__apecompat_sysconf");
extern int	getpagesize(void);	/* BSD's (ap/gnu/xsysconf.c) */
extern int	getentropy(void *, size_t);	/* POSIX.1-2024's (ap/gnu/getentropy.c: /dev/random) */
extern int	pipe2(int *, int);	/* POSIX.1-2024's (ap/gnu/gnuext.c): O_CLOEXEC, O_NONBLOCK */
/* POSIX's confstr (ap/gnu/gnuext.c): _CS_PATH, where the standard programs are */
#define _CS_PATH	0
extern size_t	confstr(int, char *, size_t);
#ifndef _BSD_EXTENSION
extern int	getopt(int, char *const *, const char *);
extern char	*optarg;
extern int	optind, opterr, optopt;
extern char	*mktemp(char *);
#endif
#ifdef __cplusplus
}
#endif
#endif
#endif
