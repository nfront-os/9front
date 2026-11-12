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

/* APE's system calls (its sys9.h): Plan 9's rfork and mount, which POSIX has not */
extern int _RFORK(int);
extern int _MOUNT(int, int, const char *, int, const char *);
enum {
	RFNAMEG = 1<<0,
	RFNOTEG = 1<<3,
	MREPL = 0,
};

enum {
	Nslot = 32,	/* wsys' buffers a window (wsys' MAXBUF): its surfaces' */
	Maxkeys = 64,	/* keys held at once */
};

/* a surface's role: what it is for, once given (wl_surface's role error) */
enum {
	Rnone,
	Rxdg,		/* an xdg_surface's: a toplevel's or a popup's */
	Rcursor,	/* the pointer's picture (wl_pointer.set_cursor) */
	Rsub,		/* a subsurface's (wl_subcompositor) */
	Rdndicon,	/* a drag's icon (wl_data_device.start_drag) */
};


typedef struct Rect Rect;
typedef struct Pool Pool;
typedef struct Buffer Buffer;
typedef struct Surface Surface;
typedef struct Win Win;
typedef struct Frame Frame;
typedef struct Pos Pos;
typedef struct Region Region;
typedef struct Source Source;

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
	int	freed;		/* released since its last commit */
};

enum {
	Nmime = 16,
};

/* a wl_data_source: what its client offers (a selection's or a drag's) */
struct Source {
	struct wl_resource	*res;
	char	*mime[Nmime];
	int	nmime;
	uint32_t	actions;	/* a drag's: copy, move, ask (set_actions) */
};

/* a wl_region: rectangles added and taken away, in their order */
struct Region {
	int	n;
	struct {
		int	add;
		Rect	r;
	}	*v;
};

/* an xdg_positioner's rules: where a popup goes by its parent */
struct Pos {
	int32_t	w, h;		/* the popup's size */
	int32_t	ax, ay, aw, ah;	/* the anchor rectangle, in the parent's window geometry */
	uint32_t	anchor, gravity, adjust;
	int32_t	ox, oy;		/* the offset */
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
	int	pdx, pdy;	/* attach's or offset's move of the picture */
	Region	*pinput;	/* set_input_region's (nil: all), with pinputset */
	int	pinputset;
	Region	*input;		/* where it takes the pointer; nil: everywhere */
	Rect	pdamage;	/* in the buffer's coordinates; empty: none */
	struct wl_list	pframes;

	int	role;
	unsigned char	cbits[64];	/* a cursor's: cursor(3)'s clr and set */
	int	cshrink;	/* shrunk by this much; 0: not read, the default cursor */
	Buffer	*buf;		/* the current buffer */
	struct wl_resource	*xdgsurf;
	struct wl_resource	*toplevel;
	struct wl_resource	*deco;	/* its zxdg_toplevel_decoration_v1 */
	struct wl_resource	*popup;	/* an xdg_popup's: of parent, placed by pos */
	Surface	*parent;
	Pos	pos;
	int	grab;		/* it takes the keyboard (a menu's) */
	int	popx, popy;	/* its place configured, in the parent's window geometry */
	int	dismissed;	/* popup_done sent */
	int	activated;	/* a toplevel's state sent: it or a popup of it has the focus */
	int	gx, gy;		/* its window geometry's origin in the surface */

	/*
	 * a tree of subsurfaces: stack has the surface (self) and its
	 * subsurfaces (their slink), bottom to top
	 */
	struct wl_list	stack;
	struct wl_list	self;
	struct wl_list	slink;
	Surface	*sparent;	/* a subsurface's parent */
	struct wl_resource	*sub;	/* its wl_subsurface */
	int	sx, sy;		/* its place in the parent */
	int	psx, psy;	/* set_position's, applied with the parent's commit */
	int	desync;
	/* a synchronized subsurface's commit, applied with its parent's */
	int	cached;
	Buffer	*cbuf;
	int	cattached;
	Region	*cinput;
	int	cinputset;
	int	lnew;		/* a new buffer since its window's last layers */
	int	ix, iy;		/* a drag icon's place at the pointer (attach's moves) */
	int	committed;	/* the toplevel's first commit, answered by a configure */
	uint32_t	serial;	/* the last configure sent */
	uint32_t	acked;	/* and acknowledged */
	char	*title;
	Win	*win;

	/* its scale (scale.c): buffer_scale, the viewport's destination, pending and applied */
	int	pscale, bscale;
	int	pvw, pvh, vw, vh;	/* -1: none */
	Rect	psdamage;	/* damage in surface units, made the buffer's at the commit */
	struct wl_resource	*viewport;	/* its wp_viewport */
	struct wl_resource	*frac;	/* its wp_fractional_scale_v1 */
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
	int	layered;	/* its tree went to wsys as layers last */
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
void	later(struct wl_list*);
void	sendenter(Surface*);

/* wlsys.c's regions */
Region	*regcopy(Region*);
void	regfree(Region*);
int	regin(Region*, int, int);

/* shm.c */
void	shminit(void);
Buffer	*newbuffer(struct wl_client*, uint32_t);
Buffer	*getbuffer(struct wl_resource*);
void	relbuf(Buffer*);
unsigned char	*pixels(Buffer*);
int	segname(int, char*, int);
void	poolput(Pool*);

/* dmabuf.c */
void	dmabufinit(void);

/* xdg.c */
void	xdginit(void);
void	configure(Surface*);
void	toplevelclose(Surface*);
int	popuprect(Surface*, int, int, Rect*);
void	popupforget(Surface*);
void	popupsdone(void);
void	focuschanged(void);

/* win.c */
extern int screenx, screeny, screenw, screenh;
int	wsysinit(void);
int	winrect(Win*, Rect*);
int	ctlmsg(Win*, char*, ...);
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
void	cursorcommit(Surface*, int, int, int);
void	cursorgone(Surface*);
void	pointergone(Surface*);
Win	*pointerheld(struct wl_client*, int*, int*);
void	pointerleave(void);
Win	*keywin(void);

/* sub.c */
void	subinit(void);
void	subnew(Surface*);
int	subcommit(Surface*);
void	subapply(Surface*);
void	subcommitted(Surface*, int);
int	hassubs(Surface*);
int	treelayers(Surface*, Surface**, int*, int*, int);
void	subgone(Surface*);
Surface	*pick(Surface*, int, int, int*, int*);
void	suborigin(Surface*, int*, int*);

/* data.c */
void	datainit(void);
void	selectionfocus(Win*);
struct wl_list	*datadevices(void);

/* dnd.c */
void	dragstart(struct wl_resource*, Source*, Surface*, Surface*);
int	dragpointer(Win*, int, int, int, uint32_t);
void	dragsourcegone(Source*);
void	dragsurfacegone(Surface*);
void	dragwingone(Win*);
int	iconrect(Surface*, int, int, Rect*);

/* deco.c */
void	decoinit(void);

/* scale.c */
extern double wscale;
void	scaleinit(void);
double	readscale(void);
int	outscale(void);
double	scaleof(Surface*);
double	wantscale(Surface*);
int	topx(Surface*, double);
void	scalecommit(Surface*);
void	scalegone(Surface*);
void	scalecheck(void);

/* wlsys.c */
void	rectadd(Rect*, int, int, int, int);
void	outputscale(void);

/* keymap.c */
int	mkkeymap(char*, char**);
int	runekey(uint32_t);
uint32_t	keymods(uint32_t*, int);
