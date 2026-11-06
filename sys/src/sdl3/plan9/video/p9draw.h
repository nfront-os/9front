/*
 * SPDX-License-Identifier: Zlib
 *
 * SDL's plan9 video driver: the native Plan 9 side (p9draw.c: libdraw),
 * plain C types so that the driver's files (SDL's headers, APE) call it.
 */
#ifndef P9DRAW_H
#define P9DRAW_H

int	p9draw_init(void);	/* the display and the process's window (rio, wsys, the screen): 0, -1 */
void	p9draw_close(void);
int	p9draw_attach(void);	/* the window again (resized: rio gives it a new name) */
/* the window's contents on the screen (inside a rio window's border) */
void	p9draw_rect(int *x, int *y, int *w, int *h);
void	p9draw_screen(int *w, int *h);	/* the whole screen */
/* w x h XRGB8888 pixels, stride bytes a row, at the window's top left: 0, -1 */
int	p9draw_put(const void *pixels, int w, int h, int stride);

#endif
