#ifndef __PWD
#define __PWD
#ifndef _POSIX_SOURCE
   This header file is not defined in pure ANSI
#endif
#pragma lib "/$M/lib/ape/libap.a"
#include <sys/types.h>

struct passwd {
	char	*pw_name;
	char	*pw_passwd;
	uid_t	pw_uid;
	gid_t	pw_gid;
	char	*pw_gecos;
	char	*pw_dir;
	char	*pw_shell;
};

#ifdef __cplusplus
extern "C" {
#endif

extern struct passwd *getpwuid(uid_t);
extern struct passwd *getpwnam(const char *);

#ifdef __cplusplus
}
#endif

#if defined(__GNUC__) || defined(_APE_GNU)
/* GCC port: the reentrant lookups (ap/gnu/posix.c) */
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
extern int	getpwuid_r(uid_t, struct passwd *, char *, size_t, struct passwd **);
extern int	getpwnam_r(const char *, struct passwd *, char *, size_t, struct passwd **);
#ifdef __cplusplus
}
#endif
#endif
#endif
