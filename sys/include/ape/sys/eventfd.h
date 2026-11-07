/* 9front-ng: Linux's eventfd for APE (ap/gnu/epoll.c): a pipe, 8 bytes a write */
#ifndef _SYS_EVENTFD_H
#define _SYS_EVENTFD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t eventfd_t;

#define EFD_SEMAPHORE	1
#define EFD_CLOEXEC	02000000
#define EFD_NONBLOCK	04000

extern int eventfd(unsigned int, int);
extern int eventfd_read(int, eventfd_t *);
extern int eventfd_write(int, eventfd_t);

#ifdef __cplusplus
}
#endif

#endif
