/*
 * 9front-ng: Linux's signalfd for APE: declared (libwayland's event loop
 * refers to it), not there: signalfd fails with ENOSYS (ap/gnu/epoll.c)
 */
#ifndef _SYS_SIGNALFD_H
#define _SYS_SIGNALFD_H

#include <stdint.h>
#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SFD_CLOEXEC	02000000
#define SFD_NONBLOCK	04000

struct signalfd_siginfo {
	uint32_t	ssi_signo;
	int32_t	ssi_errno;
	int32_t	ssi_code;
	uint32_t	ssi_pid;
	uint32_t	ssi_uid;
	int32_t	ssi_fd;
	uint32_t	ssi_tid;
	uint32_t	ssi_band;
	uint32_t	ssi_overrun;
	uint32_t	ssi_trapno;
	int32_t	ssi_status;
	int32_t	ssi_int;
	uint64_t	ssi_ptr;
	uint64_t	ssi_utime;
	uint64_t	ssi_stime;
	uint64_t	ssi_addr;
	uint8_t	pad[48];
};

extern int signalfd(int, const sigset_t *, int);

#ifdef __cplusplus
}
#endif

#endif
