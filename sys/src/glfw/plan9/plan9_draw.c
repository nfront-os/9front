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
	char name[128];
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
	/* rio's windows have a border; devdraw's screen (noborder.screen.N) has none */
	if(strncmp(name, "noborder.", 9) != 0)
		winborder = Borderwidth;
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
