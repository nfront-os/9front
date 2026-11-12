/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, sys/errno.h.  With Linux's
 * numbers, not OpenBSD's: the Linux code returns them and the file
 * servers turn them into error strings (aux/i915 fs.c errstr9).
 * OpenBSD's own names at the end.
 */
#ifndef _OBSD_SYS_ERRNO_H
#define _OBSD_SYS_ERRNO_H

#define EPERM 1
#define ENOENT 2
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENXIO 6
#define E2BIG 7
#define ENOEXEC 8
#define EBADF 9
#define ECHILD 10
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define ENOTBLK 15
#define EBUSY 16
#define EEXIST 17
#define EXDEV 18
#define ENODEV 19
#define ENOTDIR 20
#define EISDIR 21
#define EINVAL 22
#define ENFILE 23
#define EMFILE 24
#define ENOTTY 25
#define ETXTBSY 26
#define EFBIG 27
#define ENOSPC 28
#define ESPIPE 29
#define EROFS 30
#define EMLINK 31
#define EPIPE 32
#define EDOM 33
#define ERANGE 34
#define EDEADLK 35
#define ENAMETOOLONG 36
#define ENOLCK 37
#define ENOSYS 38
#define ENOTEMPTY 39
#define ELOOP 40
#define EWOULDBLOCK EAGAIN
#define ENOMSG 42
#define EIDRM 43
#define ENOSTR 60
#define ENODATA 61
#define ETIME 62
#define ENOSR 63
#define ENOLINK 67
#define EMULTIHOP 72
#define EPROTO 71
#define EBADMSG 74
#define EBADSLT 57
#define EBADE 52
#define ENOANO 55
#define EOVERFLOW 75
#define EILSEQ 84
#define ESTRPIPE 86	/* 9front: Linux's number */
#define ENOTSOCK 88
#define EMSGSIZE 90
#define EPROTONOSUPPORT 93
#define EOPNOTSUPP 95
#define ENOTSUPP 524
#define EADDRINUSE 98
#define ENETDOWN 100
#define ENETUNREACH 101
#define ECONNABORTED 103
#define ECONNRESET 104
#define ENOBUFS 105
#define EISCONN 106
#define ENOTCONN 107
#define ESHUTDOWN 108
#define ETIMEDOUT 110
#define ECONNREFUSED 111
#define EHOSTDOWN 112
#define EHOSTUNREACH 113
#define EALREADY 114
#define EINPROGRESS 115
#define ESTALE 116
#define ECHRNG 44
#define ENOPKG 65
#define EREMOTE 66
#define EREMOTEIO 121
#define EDQUOT 122
#define ENOMEDIUM 123
#define ECANCELED 125
#define ENOKEY 126
#define EKEYEXPIRED 127
#define EOWNERDEAD 130
#define ENOTRECOVERABLE 131
#define ERFKILL 132
#define EHWPOISON 133
#define ERESTARTSYS 512
#define ERESTARTNOINTR 513
#define ERESTARTNOHAND 514
#define ENOIOCTLCMD 515
#define ERESTART_RESTARTBLOCK 516
#define EPROBE_DEFER 517
#define EOPENSTALE 518
#define ENOPARAM 519
#define EBADHANDLE 521
#define ENOTSYNC 522
#define EBADCOOKIE 523
#define ETOOSMALL 525
#define ESERVERFAULT 526
#define EBADTYPE 527
#define EJUKEBOX 528
#define EIOCBQUEUED 529
#define ERECALLCONFLICT 530

/* OpenBSD's names (sys/errno.h there) */
#define ENOTSUP		EOPNOTSUPP
#define EIPSEC		EPROTO
#define EFTYPE		EINVAL
#define ERESTART	ERESTARTSYS	/* tsleep: restart the call */
/* the largest error number: Linux's MAX_ERRNO (IS_ERR) */
#define ELAST		4095
#define EJUSTRETURN	ERESTARTNOHAND

#endif
