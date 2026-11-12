/*
 * GLFW 3.5 for 9front - www.glfw.org
 * SPDX-License-Identifier: Zlib
 *
 * The plan9 platform's libdraw side (APE's: _PLAN9_SOURCE; the rest of
 * the platform has GLFW's headers): a connection of its own to draw(3),
 * the process's window by name (/dev/winname, as a rio client finds its
 * window; the screen without rio), where it is.  Pixels are not drawn
 * here: GLFW's windows are for OpenGL and Vulkan, whose plan9 WSI draws.
 */
#define _PLAN9_SOURCE
#include <draw.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "plan9_draw.h"

static Display *disp;
static Image *win;
static char winname[128];
static int winborder;		/* a rio window's border is not ours */

static void
drawerr(Display *d, char *msg)
{
	(void)d;
	fprintf(stderr, "glfw plan9: draw: %s\n", msg);
}

int
_glfwDrawInitPlan9(void)
{
	if(disp == nil && (disp = initdisplay(nil, nil, drawerr)) == nil)
		return -1;
	return _glfwDrawAttachPlan9();
}

void
_glfwDrawClosePlan9(void)
{
	if(win != nil)
		freeimage(win);
	win = nil;
	winname[0] = 0;
	if(disp != nil)
		closedisplay(disp);
	disp = nil;
}

int
_glfwDrawAttachPlan9(void)
{
	char name[128], dpi[16];
	int fd, n;

	if(disp == nil)
		return -1;
	winborder = 0;
	if((fd = open("/dev/winname", O_RDONLY)) < 0){
		/* no window system: the screen */
		if(win != nil)
			freeimage(win);
		win = nil;
		winname[0] = 0;
		return 0;
	}
	n = read(fd, name, sizeof name - 1);
	close(fd);
	if(n <= 0)
		return -1;
	name[n] = 0;
	/*
	 * rio's windows have a border; devdraw's screen (noborder.screen.N) has
	 * none; at the window system's scale (wsys' dpi) scalesize(2)'s
	 */
	disp->dpi = DefaultDPI;
	if((fd = open("/dev/dpi", O_RDONLY)) >= 0){
		n = read(fd, dpi, sizeof dpi - 1);
		close(fd);
		if(n > 0){
			dpi[n] = 0;
			if(atoi(dpi) > 0)
				disp->dpi = atoi(dpi);
		}
	}
	if(strncmp(name, "noborder.", 9) != 0)
		winborder = scalesize(disp, Borderwidth);
	if(win != nil && strcmp(winname, name) == 0)
		return 0;
	if(win != nil)
		freeimage(win);
	if((win = namedimage(disp, name)) == nil){
		winname[0] = 0;
		return -1;
	}
	snprintf(winname, sizeof winname, "%s", name);
	return 0;
}

int
_glfwDrawDpiPlan9(void)
{
	return disp != nil && disp->dpi > 0 ? disp->dpi : DefaultDPI;
}

void
_glfwDrawRectPlan9(int *x, int *y, int *w, int *h, int *border)
{
	Rectangle r;

	r = insetrect(win != nil ? win->r : disp->image->r, winborder);
	*x = r.min.x;
	*y = r.min.y;
	*w = Dx(r);
	*h = Dy(r);
	*border = winborder;
}

void
_glfwDrawScreenPlan9(int *w, int *h)
{
	*w = Dx(disp->image->r);
	*h = Dy(disp->image->r);
}

/*
 * the window's wsys is gone (killed, crashed): the window again in the next
 * one, where it was (libdraw's wsysagain); 0 then, -1 if it was the window
 * that went. The mouse's reader asks, the main thread then reconnects.
 */
int
_glfwDrawAgainPlan9(void)
{
	char rect[64];

	if(!wsyslost())
		return -1;
	rect[0] = 0;
	if(win != nil)
		snprintf(rect, sizeof rect, "%d %d %d %d", win->r.min.x, win->r.min.y, win->r.max.x, win->r.max.y);
	return wsysagain(rect[0] ? rect : nil);
}

/* a new connection to draw(3): the old one's devdraw went with its wsys */
int
_glfwDrawReconnectPlan9(void)
{
	_glfwDrawClosePlan9();
	return _glfwDrawInitPlan9();
}
