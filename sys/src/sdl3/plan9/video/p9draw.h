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
int	p9draw_attach(void);
int	p9draw_border(void);	/* the window's border: the window system's, at its scale */
int	p9draw_dpi(void);	/* the window system's dots per inch (wsys' scale ×96), as of the last attach */	/* the window again (resized: rio gives it a new name) */
int	p9draw_again(void);	/* the window's wsys gone: the window in the next one: 0, -1 (the window went) */
int	p9draw_reconnect(void);	/* draw(3) anew after p9draw_again */
void	p9draw_errstr(char *buf, int n);	/* the system's error string of the last failed call */
/* the window's contents on the screen (inside a rio window's border) */
void	p9draw_rect(int *x, int *y, int *w, int *h);
void	p9draw_screen(int *w, int *h);	/* the whole screen */
/* w x h XRGB8888 pixels, stride bytes a row, at the window's top left: 0, -1 */
int	p9draw_put(const void *pixels, int w, int h, int stride);

#endif
