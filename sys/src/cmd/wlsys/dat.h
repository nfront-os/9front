/*
 * SPDX-License-Identifier: MIT
 *
 * wlsys: a Wayland server as a client of wsys(4) (../9front-ui PLAN §12,
 * stage 6.5).  Each xdg toplevel is a window of wsys; a wl_shm pool is a
 * segment of segment(3) where the kernel put it (APE's memfd_create), and
 * its buffers go to wsys as they are (buffer n seg w h stride off): no copy
 * here; a dma-buf (zwp_linux_dmabuf_v1) is a GPU buffer of gpu(4), named
 * by its export token, which goes to wsys as one (gpubuffer).  Input comes from the window's files: event (the mouse, sizes,
 * focus, released buffers) and kbd (kbd(3) messages: keys down and up).
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <wayland-server.h>
#include <xdg-shell-server-protocol.h>

enum {
	Nslot = 8,	/* wsys' buffers a window */
	Maxkeys = 64,	/* keys held at once */
};

typedef struct Rect Rect;
typedef struct Pool Pool;
typedef struct Buffer Buffer;
typedef struct Surface Surface;
typedef struct Win Win;
typedef struct Frame Frame;

struct Rect {
	int	x0, y0, x1, y1;
};

/* a client's wl_shm_pool: its segment, kept named while buffers are in it */
struct Pool {
	struct wl_resource	*res;	/* nil once the client destroyed it */
	int	fd;		/* the segment's directory in #g, as the client sent it */
	char	seg[128];	/* its name */
	int32_t	size;
	int	ref;		/* res and the buffers */
};

struct Buffer {
	struct wl_resource	*res;
	Pool	*pool;		/* wl_shm's; nil for a dma-buf */
	char	token[64];	/* a dma-buf's: gpu(4)'s export token */
	int32_t	off, w, h, stride;
	uint32_t	format;
	Win	*win;		/* the window it is announced to, and as which buffer */
	int	slot;
};

/* a frame callback, waiting for its surface's picture to be shown */
struct Frame {
	struct wl_resource	*res;
	struct wl_list	link;
};

struct Surface {
	struct wl_resource	*res;
	struct wl_list	link;	/* surfaces */

	/* the state a commit applies */
	Buffer	*pbuf;
	int	pattached;
	Rect	pdamage;	/* in the buffer's coordinates; empty: none */
	struct wl_list	pframes;

	Buffer	*buf;		/* the current buffer */
	struct wl_resource	*xdgsurf;
	struct wl_resource	*toplevel;
	int	committed;	/* the toplevel's first commit, answered by a configure */
	uint32_t	serial;	/* the last configure sent */
	uint32_t	acked;	/* and acknowledged */
	char	*title;
	Win	*win;
};

/* a window of wsys for a toplevel */
struct Win {
	Surface	*s;
	int	id;
	int	ctl;		/* the clone file: closed, the window goes */
	int	ev, kbd;
	struct wl_event_source	*evsrc, *kbdsrc;
	char	evbuf[8192];
	int	nev;
	char	kbdbuf[2048];
	int	nkbd;
	int	w, h;		/* its size, as wsys has it */
	Buffer	*slot[Nslot];
	uint32_t	age[Nslot];	/* when last presented: the oldest goes first */
	int	busy[Nslot];	/* presented, not yet released by wsys */
	struct wl_list	frames[Nslot];	/* the callbacks of a present */
	int	focus;
	int	gone;		/* deleted in wsys: the client was told to close */
	int	fresh;		/* new, or a new size: the next present shows all */
};

/* wlsys.c */
extern struct wl_display *display;
extern struct wl_event_loop *loop;
extern struct wl_list surfaces;
extern int debug;
void	dprint(char*, ...);
uint32_t	msec(void);
void	surfacegone(Surface*);
void	buffergone(Buffer*);
void	frames(struct wl_list*, uint32_t);
void	sendenter(Surface*);

/* shm.c */
void	shminit(void);
Buffer	*newbuffer(struct wl_client*, uint32_t);
Buffer	*getbuffer(struct wl_resource*);

/* dmabuf.c */
void	dmabufinit(void);

/* xdg.c */
void	xdginit(void);
void	configure(Surface*);
void	toplevelclose(Surface*);

/* win.c */
extern int screenw, screenh;
int	wsysinit(void);
void	show(Surface*);
void	unshow(Surface*);
void	unbuffer(Buffer*);
void	winlabel(Win*, char*);

/* seat.c */
void	seatinit(void);
void	pointer(Win*, int, int, int, uint32_t);
void	keys(Win*, char*, int);
void	focus(Win*, int);
void	seatforget(Win*);

/* keymap.c */
int	mkkeymap(char*, char**);
int	runekey(uint32_t);
uint32_t	keymods(uint32_t*, int);
