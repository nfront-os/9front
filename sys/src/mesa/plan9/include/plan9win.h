/* SPDX-License-Identifier: MIT */
/*
 * The draw(3) side of Mesa's plan9 WSI (lib/plan9win.c, APE's libdraw):
 * plain C types, so Mesa's code needs no Plan 9 headers.
 * name: a window image's name; NULL: the process's window
 * (/dev/winname), or the screen when there is none.
 */
#ifndef _MESA_PLAN9WIN_H
#define _MESA_PLAN9WIN_H
#ifdef __cplusplus
extern "C" {
#endif

/* the size of the window's drawable area; -1 if there is none */
extern int plan9win_size(const char *name, int *w, int *h);

/* 1 if the window is the screen itself (no rio): nobody else's
 * windows on it, the frames can go to the display directly */
extern int plan9win_isscreen(const char *name);

/* copy w×h pixels (B,G,R,X bytes, rows stride bytes apart) to the
 * window's drawable area, clipped; -1 on error */
extern int plan9win_put(const char *name, const void *pixels, int w, int h, int stride);

/*
 * wsys(4): the window's ctl file when the window is the process's own
 * window of wsys (/dev/ctl), else -1.  Then the frames can go to wsys
 * as GPU buffers it reads itself (ctl gpubuffer, present)
 */
extern int plan9win_wsys(const char *name);

/* a request to wsys' ctl; -1 if refused */
extern int plan9win_wsysctl(int fd, const char *msg);
extern int plan9win_wsysevents(void);	/* the window's event file in wsys (overlay 0|1, b n) */
/* the system's descriptors of these two, not APE's */
extern int plan9win_wsysread(int fd, char *buf, int n);
extern void plan9win_wsysclose(int fd);

#ifdef __cplusplus
}
#endif
#endif
