#ifndef __ERRNO
#define __ERRNO
#pragma lib "/$M/lib/ape/libap.a"

extern int *_errnoloc;
#define errno (*_errnoloc)
#ifdef _GNU_SOURCE
/* 9front-ng: glibc's: argv[0] and its last element (plan9/callmain.c) */
extern char *program_invocation_name;
extern char *program_invocation_short_name;
#endif

#define EDOM	1000
#define ERANGE	1001
#define EPLAN9	1002

#ifdef _POSIX_SOURCE

#define E2BIG	1
#define EACCES	2
#define	EAGAIN	3
#define EWOULDBLOCK	3
#define EBADF	4
#define EBUSY	5
#define ECHILD	6
#define EDEADLK	7
#define EEXIST	8
#define EFAULT	9
#define EFBIG	10
#define EINTR	11
#define EINVAL	12
#define EIO	13
#define EISDIR	14
#define EMFILE	15
#define EMLINK	16
#define ENAMETOOLONG	17
#define ENFILE	18
#define ENODEV	19
#define ENOENT	20
#define ENOEXEC	21
#define ENOLCK	22
#define ENOMEM	23
#define ENOSPC	24
#define ENOSYS	25
#define ENOTDIR	26
#define ENOTEMPTY	27
#define ENOTTY	28
#define ENXIO	29
#define EPERM	30
#define EPIPE	31
#define EROFS	32
#define ESPIPE	33
#define ESRCH	34
#define EXDEV	35

/* bsd networking software */
#define ENOTSOCK	36
#define EPROTONOSUPPORT	37
#define EPROTOTYPE	37	/* two names for 37 */
#define ECONNREFUSED	38
#define EAFNOSUPPORT	39
#define ENOBUFS		40
#define EOPNOTSUPP	41
#define EADDRINUSE	42
#define EDESTADDRREQ	43
#define EMSGSIZE	44
#define ENOPROTOOPT	45
#define ESOCKTNOSUPPORT	46
#define EPFNOSUPPORT	47
#define EADDRNOTAVAIL	48
#define ENETDOWN	49
#define ENETUNREACH	50
#define ENETRESET	51
#define ECONNABORTED	52
#define EISCONN		53
#define ENOTCONN	54
#define ESHUTDOWN	55
#define ETOOMANYREFS	56
#define ETIMEDOUT	57
#define EHOSTDOWN	58
#define EHOSTUNREACH	59
#define EGREG		60

/* These added in 1003.1b-1993 */
#define ECANCELED	61
#define EINPROGRESS	62

#endif /* _POSIX_SOURCE */

#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port: the errno values of C99 and POSIX.1-2008 APE has not had
 * (libstdc++'s std::errc, ports); libap never sets these, strerror calls them
 * "Unknown error". Outside APE's ranges (1-62, 1000-1002). */
#ifndef EILSEQ
#define EILSEQ		1100
#endif
#ifdef _POSIX_SOURCE
#ifndef EALREADY
#define EALREADY	1101
#endif
#ifndef ECONNRESET
#define ECONNRESET	1102
#endif
#ifndef ELOOP
#define ELOOP		1103
#endif
#ifndef ENOMSG
#define ENOMSG		1104
#endif
#ifndef ENOTSUP
#define ENOTSUP		EOPNOTSUPP
#endif
#ifndef EOVERFLOW
#define EOVERFLOW	1105
#endif
#ifndef ENODATA
#define ENODATA		1106	/* 9front next generation: Mesa's RADV (amdgpu's lost contexts) */
#define EPROTO		1107	/* 9front-ng: libwayland (a protocol error) */
#endif
#endif
#endif
#endif /* __ERRNO */
