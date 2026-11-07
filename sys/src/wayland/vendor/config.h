/*
 * SPDX-License-Identifier: MIT
 *
 * libwayland's configuration for 9front (meson's config.h, by hand): APE
 * with ../9front-gcc's cross gcc; epoll, timerfd, memfd_create, sendmsg
 * with SCM_RIGHTS, open_memstream from APE (../9front-ui PLAN §12)
 */
#define PACKAGE "wayland"
#define PACKAGE_VERSION "1.26.0"
/* APE has these (gnu/mman.c, gnu/posix.c): libwayland-cursor's images in a
 * memfd, a segment wlsys can hand to wsys (a file in /tmp it refuses) */
#define HAVE_MEMFD_CREATE 1
#define HAVE_POSIX_FALLOCATE 1
